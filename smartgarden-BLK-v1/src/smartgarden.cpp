#include <Arduino.h>
#include "smartgarden.h"
#include "constant.h"
#include "ir_remote.h"
#include "sensorsuhu.h"
#include "config.h"
#include "display.h"

#define setValve(no, state) SERIAL_REG[VALVE_START + no] = state
#define getValve(no) SERIAL_REG[VALVE_START + no]
#define valveName(no) PinSerialNames[VALVE_START + no]
#define hasValveOn VALVE_CURRENT >= 0

enum WaitingState : uint8_t
{
    WAIT_NONE,
    WAIT_PUMP_AUTOOFF,
    WAIT_VALVE,
    WAIT_VALVE_GAP
};

// Serial registers
uint8_t SERIAL_REG[16] = {};

// Analog 0 - 5    Sensor Kelembaban Tanah, tanpa DHT22
uint8_t ANALOG_SENSOR[VALVE_COUNT - 1] = {};

// Analog 6
uint8_t HUMIDITY = 0;
int8_t TEMPERATURE = 0;

// Valve yg saat ini sedang on
int8_t VALVE_CURRENT = -1;
int8_t VALVE_NEXT = -1;
// store all valve conditions, if bit set mean need water
uint8_t VALVE_STATE = 0;

// Last valve turned on, in second
uint32_t VALVE_LAST_ON[VALVE_COUNT] = {};
uint32_t LAST_LOOP = 0;
/* 
uint32_t valve_next_check = 0;
uint32_t sensor_next_check = 0;
// Cek setiap sensor setiap:
uint32_t smartgarden_delay = 1000; // 1 second

uint32_t pompa_nyala_sejak = 0;
uint32_t pompa_mati_sampai = 0;
 */
/** Apply REG value to 2 chained IC 595 */
void serialApply()
{
    digitalWrite(SERIAL_LOAD, LOW);
    for (int i = 15; i >= 0; i--)
    {
        digitalWrite(SERIAL_DATA, SERIAL_REG[i] & B1);
        digitalWrite(SERIAL_CLOCK, HIGH);
        digitalWrite(SERIAL_CLOCK, LOW);
    }
    digitalWrite(SERIAL_LOAD, HIGH);
}
void sensorUpdate()
{
    // Sensor Suhu
    sensorsuhu_read();

    // readAllAnalog
    for (int i = 0; i < (VALVE_COUNT - 1); i++)
    {
        // Set bit switches on IC 4051
        SERIAL_REG[PinSerial::IC4051_SC] = (i >> 2) & 0x1;
        SERIAL_REG[PinSerial::IC4051_SB] = (i >> 1) & 0x1;
        SERIAL_REG[PinSerial::IC4051_SA] = (i >> 0) & 0x1;
        serialApply();
        ANALOG_SENSOR[i] = static_cast<int>((system_adc_read() / 4) * (100.0f / 256.0f));
    }
}
void smartgarden_setup()
{
    pinMode(SERIAL_DATA, OUTPUT);
    pinMode(SERIAL_LOAD, OUTPUT);
    pinMode(SERIAL_CLOCK, OUTPUT);
    pinMode(SENSOR_SUHU_PIN, INPUT);
    // update initial sensors
    sensorUpdate();
}

void dumpSerial(int start = 0, int end = 15)
{
    P(MAGENTA("--- dumpSerial %d - %d ---\n"), start, end);
    for (int i = start; i <= end; i++)
    {
        P(" > %11s : %s\n", PinSerialNames[i], ONOFF(SERIAL_REG[i] & B1));
    }
}


// no -1 mean turn off all
void valveOn(int8 no)
{
    // Turn off all except one
    for (int i = 0; i < VALVE_COUNT; i++)
    {
        setValve(no, i == no);
    }
    if (no >= 0)
    {
        VALVE_LAST_ON[no] = DETIK;
    }
    // turn pompa on
    SERIAL_REG[PinSerial::Pompa] = no >= 0;
    VALVE_CURRENT = no;
    status("%s ON %d dtk*", valveName(VALVE_CURRENT), config->valve_delay[no]);
    serialApply();
}

void handle_ir_remote()
{
    RemoteButton pressed = currentButton->remoteButton;
    int8_t forcedValve = -1;
    // Valve button 1 - 6
    if (pressed >= RemoteButton::BTN_1 && pressed <= RemoteButton::BTN_6)
    {
        forcedValve = pressed;
    }
    // TUrn on sprayer
    else if (pressed >= RemoteButton::BTN_7 && pressed <= RemoteButton::BTN_9)
    {
        forcedValve = SPRAYER_NO;
    }
    if (forcedValve >= 0)
    {
        VALVE_CURRENT = forcedValve;
        valveOn(VALVE_CURRENT);
    }
}

void dumpValveState()
{
    P(RED("VALVE STATE:\n"));
    for (int i = 0; i < VALVE_COUNT; i++)
    {
        P("%s\t%d\n",
          valveName(i),
          (VALVE_STATE >> i) & 1);
    }
}
// Check valve that need to turn on
int8_t findValveThatNeedWater()
{
    // reset state
    VALVE_STATE = 0;
    // 1 111 111
    // All Analog sensors, except DHT22
    int8_t needWater = -1;
    for (int i = 0; i < VALVE_COUNT - 1; i++)
    {
        VALVE_STATE |= (ANALOG_SENSOR[i] <= config->humidity_minimal[i]) << i;
    }

    if (TEMPERATURE >= config->temperature_max)
    {
        VALVE_STATE |= 1 << SPRAYER_NO;
    }
    else if (HUMIDITY < config->humidity_minimal[SPRAYER_NO])
    {
        VALVE_STATE |= 1 << SPRAYER_NO;
    }
    if (VALVE_STATE > 0)
    {
        // Find that oldest start
        for (int i = 0; i < VALVE_COUNT; i++)
        {
            if (VALVE_STATE & 1 << i)
            {
                if (needWater < 0 ||
                    // whois oldest
                    VALVE_LAST_ON[i] < VALVE_LAST_ON[needWater])
                {
                    needWater = i;
                }
            }
        }
        dumpValveState();
        P("Should ON %s\n", PinSerialNames[VALVE_START + needWater]);
    }
    else
    {
        P("Nothing that need water\n");
    }
    return needWater;
}

void smartgarden_loop()
{
    // check pressed button
    if (currentButton)
    {
        handle_ir_remote();
    }
    // Masih di detik yg sama, abaikan
    if (DETIK - LAST_LOOP <= 0)
    {
        return;
    }

    // Update last loop
    LAST_LOOP = DETIK;

    if ((DETIK % config->sensor_delay) == 0)
    {
        P(YELLOW("sensorUpdate\n"));
        sensorUpdate();
    }

    // have valve that still on
    if (hasValveOn)
    {
        int remain = config->valve_delay[VALVE_CURRENT] - (DETIK - VALVE_LAST_ON[VALVE_CURRENT]);
        if (remain > 0)
        {
            status("%s ON %d dtk", valveName(VALVE_CURRENT), remain);
            // no need more time
            return;
        }
    }
    int8_t needWater = findValveThatNeedWater();
    if (needWater >= 0)
    {
        // same valve need water, check the gap period
        if (needWater == VALVE_CURRENT)
        {
            int nextOn = VALVE_LAST_ON[VALVE_CURRENT] + config->valve_delay[VALVE_CURRENT] + config->valve_gap[VALVE_CURRENT];
            int gap_remain = nextOn - DETIK;
            if (gap_remain > 0)
            {
                if (getValve(VALVE_CURRENT) == HIGH)
                {
                    // still high, need to turn it off
                    setValve(VALVE_CURRENT, LOW);
                    SERIAL_REG[PinSerial::Pompa] = LOW;
                    dumpSerial(PinSerial::Pompa, VALVE_START+VALVE_CURRENT);
                    serialApply();
                }
                status("%s wait %3d dtk", valveName(VALVE_CURRENT), gap_remain);
                return;
            }
        }
        /* else
        {
            valveOn(needWater)
        } */
    }
    else
    {
    }
    valveOn(needWater);

    /* 
    if (pompa_mati_sampai > 0)
    {
        if (now >= pompa_mati_sampai)
        {
            pompa_mati_sampai = 0;
            //status("Pompa ON");
            P("POMPA ON\n");
            // SERIAL_REG[PinSerial::Pompa] = ON;
            pompa_nyala_sejak = now;
            if (VALVE_CURRENT >= 0)
            {
                // turn back on
                SERIAL_REG[VALVE_START + VALVE_CURRENT] = HIGH;
            }
            pompaChecker();
            serialApply();
        }
        else
        {
            uint remaining = (pompa_mati_sampai - now) / 1000;
            if (remaining >= 0)
            {
                status("Auto Off %5d detik", remaining + 1);
            }
        }
    }
    else if (VALVE_CURRENT >= 0)
    {
        uint8_t remaining = 0;
        if (now < valve_next_check)
        {
            remaining = static_cast<uint8_t>((valve_next_check - now) / 1000);
        }
        if (VALVE_CURRENT == SPRAYER_NO)
        {
            status("Sprayer ON %3d detik", remaining + 1);
        }
        else
        {
            status("No %d ON %6d detik", VALVE_CURRENT + 1, remaining + 1);
        }
    }
    else
    {
        status(config->displayText);
    }

    if (valve_next_check <= now)
    {
        valveChecker();
        valveSwitcher();
        pompaChecker();
        serialApply();
    } */
}
