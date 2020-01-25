#include <Arduino.h>
#include "smartgarden.h"
#include "constant.h"
#include "ir_remote.h"
#include "sensorsuhu.h"
#include "config.h"
#include "display.h"
#define isValveManual(i) ((config->valve_manual >> i) & 0x1)

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

uint8_t VALVE_STATE = 0;

// Last valve turned on, in second
uint32_t VALVE_LAST_ON[VALVE_COUNT] = {};
uint32_t LAST_LOOP = 0;

uint32_t pompa_nyala_sejak = 0;
uint32_t pompa_mati_sampai = 0;
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
        // if pompa off, save state off high now
        if (getPompa() == LOW)
        {
            pompa_nyala_sejak = DETIK;
            pompa_mati_sampai = 0;
            setPompa(HIGH);
        }
    }
    else
    {
        setPompa(LOW);
        pompa_nyala_sejak = 0;
    }

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
        // check if is not manual mode
        if (!isValveManual(i))
        {
            VALVE_STATE |= (ANALOG_SENSOR[i] <= config->humidity_minimal[i]) << i;
        }else{
            P("Manual %d\n", i);
        }
    }
    if (!isValveManual(SPRAYER_NO))
    {
        if (TEMPERATURE >= config->temperature_max)
        {
            VALVE_STATE |= 1 << SPRAYER_NO;
        }
        else if (HUMIDITY < config->humidity_minimal[SPRAYER_NO])
        {
            VALVE_STATE |= 1 << SPRAYER_NO;
        }
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

void forceTempOff(const char *reason)
{
    setValve(VALVE_CURRENT, LOW);
    SERIAL_REG[PinSerial::Pompa] = LOW;
    serialApply();
    P(RED("forceTempOff: %s\n"), reason);
}
void forceTempOn(const char *reason)
{
    setValve(VALVE_CURRENT, HIGH);
    SERIAL_REG[PinSerial::Pompa] = HIGH;
    serialApply();
    P(BLUE("forceTempOn: %s\n"), reason);
}

void smartgarden_loop()
{
    int remain;
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
        sensorUpdate();
    }
    if (pompa_mati_sampai > 0)
    {
        remain = pompa_mati_sampai - DETIK;
        if (remain > 0)
        {
            status("Pompa off %d detik", remain);
            return;
        }
        if (hasValveCurrent)
        {
            VALVE_LAST_ON[VALVE_CURRENT] += config->maksimal_pompa_mati;
        }

        pompa_nyala_sejak = 0;
        pompa_mati_sampai = 0;
        /* else
        {
        } */
    }

    // have valve that still on
    if (hasValveCurrent && getPompa() == HIGH)
    {
        // Berapa lama lagi pompa boleh nyala
        remain = pompa_nyala_sejak + config->maksimal_pompa_hidup - DETIK;
        if (remain < 0)
        {
            pompa_mati_sampai = DETIK + config->maksimal_pompa_mati;
            forceTempOff("POMPA AUTO OFF");
            status("Pompa off %d detik*", config->maksimal_pompa_mati);
            return;
        }
        remain = config->valve_delay[VALVE_CURRENT] - (DETIK - VALVE_LAST_ON[VALVE_CURRENT]);
        if (remain > 0)
        {
            status("%s ON %d detik", valveName(VALVE_CURRENT), remain);
            P("%d\n", getValve(VALVE_CURRENT));
            return;
        }
    }

    int8_t needWater = findValveThatNeedWater();
    if (needWater >= 0)
    {
        // same valve need water, check the gap period
        if (needWater == VALVE_CURRENT && getPompa() == HIGH)
        {
            int nextOn = VALVE_LAST_ON[VALVE_CURRENT] + config->valve_delay[VALVE_CURRENT] + config->valve_gap[VALVE_CURRENT];
            int gap_remain = nextOn - DETIK;
            if (gap_remain > 0)
            {
                if (getValve(VALVE_CURRENT) == HIGH)
                {
                    pompa_mati_sampai = DETIK + config->valve_gap[VALVE_CURRENT];
                    // still high, need to turn it off
                    forceTempOff("VALVE_GAP");
                }
                status("%s delay %d dtk", valveName(VALVE_CURRENT), gap_remain);
                return;
            }
        }
    }

    valveOn(needWater);
}
