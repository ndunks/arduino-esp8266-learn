#include <Arduino.h>
#include "smartgarden.h"
#include "constant.h"
#include "ir_remote.h"
#include "sensorsuhu.h"
#include "config.h"
#include "display.h"

// Serial registers
uint8_t SERIAL_REG[16] = {};

// Analog 0 - 5    Sensor Kelembaban Tanah, tanpa DHT22
uint8_t ANALOG_SENSOR[VALVE_COUNT - 1] = {};

// Analog 6
uint8_t HUMIDITY = 0;
int8_t TEMPERATURE = 0;

// Valve yg saat ini sedang on
int8_t VALVE_CURRENT = -1;

// List antrean valve, karena hanya 1 valve yg open dalam satu waktu
int8_t VALVE_STACK[VALVE_COUNT] = {};
uint32_t VALVE_LAST_ON[VALVE_COUNT] = {};

uint32_t valve_next_check = 0;
uint32_t sensor_next_check = 0;
// Cek setiap sensor setiap:
uint32_t smartgarden_delay = 1000; // 1 second

uint32_t pompa_nyala_sejak = 0;
uint32_t pompa_mati_sampai = 0;

void smartgarden_setup()
{
    std::fill_n(VALVE_STACK, VALVE_COUNT, -1);
    pinMode(SERIAL_DATA, OUTPUT);
    pinMode(SERIAL_LOAD, OUTPUT);
    pinMode(SERIAL_CLOCK, OUTPUT);
    pinMode(SENSOR_SUHU_PIN, INPUT);
}

void dumpSerial(int start = 0, int end = 15)
{
    P(MAGENTA("--- dumpSerial %d - %d ---\n"), start, end);
    for (int i = start; i <= end; i++)
    {
        P(" > %11s : %s\n", PinSerialNames[i], ONOFF(SERIAL_REG[i] & B1));
    }
}
void pompaChecker()
{
    // Jika ada salah satu valve yg on, maka pompa juga harus on
    if (VALVE_CURRENT >= 0)
    {
        if (SERIAL_REG[PinSerial::Pompa] == HIGH)
        {
            uint lamaPompaNyala = static_cast<uint>((millis() - pompa_nyala_sejak) / 1000);
            if (lamaPompaNyala >= config->maksimal_pompa_hidup)
            {
                P("Pompa auto off %i >= %i\n", lamaPompaNyala, config->maksimal_pompa_hidup);
                //valveForceOn(-1, config->maksimal_pompa_mati);
                pompa_mati_sampai = millis() + config->maksimal_pompa_mati * 1000UL;
                SERIAL_REG[PinSerial::Pompa] = LOW;
                SERIAL_REG[VALVE_START + VALVE_CURRENT] = LOW;
                VALVE_CURRENT = -1;
                valve_next_check += config->maksimal_pompa_mati * 1000UL;
            }
        }
        else // Nyalakan pompa sekarang
        {
            SERIAL_REG[PinSerial::Pompa] = HIGH;
            pompa_nyala_sejak = millis();
        }
    }
    else if (SERIAL_REG[PinSerial::Pompa] == HIGH)
    {
        // all off
        SERIAL_REG[PinSerial::Pompa] = LOW;
        pompa_nyala_sejak = 0;
        status(config->displayText);
    }
}
/** Apply REG value to 2 chained IC 595 */
void smartgarden_apply()
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

void selectAnalog(int no)
{
    // Set bit switches on IC 4051
    SERIAL_REG[PinSerial::IC4051_SC] = (no >> 2) & 0x1;
    SERIAL_REG[PinSerial::IC4051_SB] = (no >> 1) & 0x1;
    SERIAL_REG[PinSerial::IC4051_SA] = (no >> 0) & 0x1;
    smartgarden_apply();
}

void readAllAnalog()
{
    for (int i = 0; i < (VALVE_COUNT - 1); i++)
    {
        selectAnalog(i);
        ANALOG_SENSOR[i] = static_cast<int>((system_adc_read() / 4) * (100.0f / 256.0f));
        yield();
    }
}
void valveDump(const char *prefix)
{
    Serial.printf("%s: ", prefix);
    int i = 0;
    while (i < VALVE_COUNT && VALVE_STACK[i] >= 0)
    {
        Serial.printf("%d ", VALVE_STACK[i++]);
    }
    Serial.print('\n');
}
// Add valve to on when applied
bool valvePush(int no, bool clearOther = false)
{
    int i = -1;
    if (clearOther)
    {
        while (VALVE_STACK[++i] >= 0 && i <= VALVE_COUNT)
        {
            VALVE_STACK[i] = -1;
        }
        i = 0;
    }
    else
    {
        while (VALVE_STACK[++i] >= 0 && i <= VALVE_COUNT)
        {
            if (VALVE_STACK[i] == no)
            {
                return false;
            }
        }
        if (i == VALVE_COUNT)
        {
            Serial.println("[!] Valve stack is full");
            return false;
        }
    }
    // if no < 0, mean turn off
    if (no >= 0)
    {
        VALVE_STACK[i] = no;
    }
    valveDump("pushed");
    return true;
}

void valvePop()
{
    if (VALVE_STACK[0] < 0)
    {
        P("valvePop: No valve in stack\n");
    }
    else
    {
        // unshift array
        for (int i = 1; i < VALVE_COUNT; i++)
        {
            if (0 > (VALVE_STACK[i - 1] = VALVE_STACK[i]))
                break;
            else
                VALVE_STACK[i] = -1;
        }
        valveDump("popped");
    }
}

void valveSwitcher(bool force = false)
{
    int no;
    if (VALVE_CURRENT >= 0)
    {
        SERIAL_REG[VALVE_START + VALVE_CURRENT] = LOW;
        P("Turn Off %d\n", VALVE_CURRENT);
        if (VALVE_CURRENT == VALVE_STACK[0])
        {
            valve_next_check = millis() + config->valve_gap[VALVE_CURRENT] * 1000UL;
            VALVE_CURRENT = -1;
            return;
        }
        else
        {
            valve_next_check = 0;
            VALVE_CURRENT = -1;
        }
    }
    // Stillhave in stack?
    if (VALVE_STACK[0] >= 0)
    {
        no = VALVE_STACK[0];
        bool turnItOn = false;

        // Forced on by remote ir or web
        if (force || no == SPRAYER_NO)
        {
            turnItOn = true;
        }
        // check again if it still need water, except from DHT22
        else
        {
            turnItOn = ANALOG_SENSOR[no] <= config->humidity_minimal[no];
        }
        if (turnItOn)
        {
            SERIAL_REG[VALVE_START + no] = HIGH;
            VALVE_CURRENT = no;
            VALVE_LAST_ON[VALVE_CURRENT] = millis();
            valve_next_check = VALVE_LAST_ON[VALVE_CURRENT] + config->valve_delay[no] * 1000UL;
            P("Turn On %s for %d second\n",
              PinSerialNames[VALVE_START + VALVE_CURRENT],
              config->valve_delay[no]);
        }
        valvePop();
    }
}

void valveChecker()
{
    bool shouldOn = false;
    for (int i = 0; i < VALVE_COUNT; i++)
    {
        // Skip check for sprayer
        if (i == SPRAYER_NO)
            continue;

        shouldOn = ANALOG_SENSOR[i] <= config->humidity_minimal[i];
        //P("Sensor_%d: %d = %d\n", i, ANALOG_SENSOR[i], shouldOn);
        if (shouldOn)
        {
            valvePush(i);
        }
    }

    if (TEMPERATURE >= config->temperature_max)
    {
        //P("Suhu panas (%s >= %d)\n", YELLOW(TEMPERATURE), config->temperature_max);
        valvePush(SPRAYER_NO);
    }
    else if (HUMIDITY < config->humidity_minimal[SPRAYER_NO])
    {
        //P("Kelembaban kurang (%s < %d)\n", YELLOW(HUMIDITY), config->humidity_minimal[SPRAYER_NO]);
        valvePush(SPRAYER_NO);
    }
}

void valveForceOn(int8 no, int forceOffSeconds)
{
    valvePush(no, true);
    valveSwitcher(true);
    if (no < 0)
    {
        // Force All Off
        pompa_mati_sampai = millis() + forceOffSeconds * 1000UL;
        valve_next_check = pompa_mati_sampai;
    }
    else if (VALVE_CURRENT >= 0 && pompa_mati_sampai > 0)
    {
        // force on
        pompa_mati_sampai = 0;
        pompaChecker();
    }
    smartgarden_apply();
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
        //valve_next_check = 0;
        valveForceOn(forcedValve);
    }
}
void smartgarden_loop()
{
    uint32_t now = millis();
    // check pressed button
    if (currentButton)
    {
        handle_ir_remote();
    }

    if (sensor_next_check < now)
    {
        sensorsuhu_read();
        readAllAnalog();
        sensor_next_check = now + config->sensor_delay * 1000UL;
    }

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
            smartgarden_apply();
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
        smartgarden_apply();
    }
}
