#include "header.h"
#include "ir_remote.h"
#include "smartgarden.h"
#include "sensorsuhu.h"

// Serial registers
uint8_t SERIAL_REG[16] = {};

// Analog 0 - 5    Sensor Kelembaban Tanah, tanpa DHT22
uint8_t ANALOG_SENSOR[VALVE_COUNT - 1] = {};

// Analog 6
uint8_t HUMIDITY = 0;
int8_t TEMPERATURE = 0;

// Valve yg saat ini sedang on
int VALVE_CURRENT = -1;

// List antrean valve, karena hanya 1 valve yg open dalam satu waktu
int VALVE_STACK[VALVE_STACK_MAX] = {};

unsigned long valve_next_check = 0;
unsigned long sensor_next_check = 0;
// Cek setiap sensor setiap:
unsigned long smartgarden_delay = 1000; // 1 second

unsigned long pompa_nyala_sejak = 0;
unsigned long pompa_mati_sampai = 0;

void smartgarden_setup()
{
    std::fill_n(VALVE_STACK, VALVE_STACK_MAX, -1);
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
    unsigned long now = millis();
    // Jika ada salah satu valve yg on, maka pompa juga harus on
    if (VALVE_CURRENT >= 0)
    {
        if (!SERIAL_REG[PinSerial.Pompa])
        {
            SERIAL_REG[PinSerial.Pompa] = ON;
            pompa_nyala_sejak = now;
        }
        else if (((now - pompa_nyala_sejak) / 1000UL) > config->maksimal_pompa_hidup)
        {
            P("Pompa nyala selama %d\n", static_cast<uint8_t>((now - pompa_nyala_sejak) / 1000L));
            pompa_mati_sampai = now + config->maksimal_pompa_mati * 1000UL;
            SERIAL_REG[PinSerial.Pompa] = OFF;
            valve_next_check += config->maksimal_pompa_mati * 1000UL;
            dumpSerial(PinSerial.Pompa, PinSerial.Pompa);
        }
    }
    else if (SERIAL_REG[PinSerial.Pompa])
    {
        SERIAL_REG[PinSerial.Pompa] = OFF;
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
    //dumpSerial(PinSerial.Valve_0, PinSerial.Sprayer);
}

void selectAnalog(int no)
{
    // Set bit switches on IC 4051
    SERIAL_REG[PinSerial.IC4051_SC] = (no >> 2) & 0x1;
    SERIAL_REG[PinSerial.IC4051_SB] = (no >> 1) & 0x1;
    SERIAL_REG[PinSerial.IC4051_SA] = (no >> 0) & 0x1;
    smartgarden_apply();
}

void readAllAnalog()
{
    for (int i = 0; i < (VALVE_COUNT - 1); i++)
    {
        selectAnalog(i);
        ANALOG_SENSOR[i] = static_cast<int>((system_adc_read() / 4) * (100.0f / 256.0f));
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
bool valvePush(int no)
{
    int i = -1;
    while (VALVE_STACK[++i] >= 0 && i <= VALVE_STACK_MAX)
    {
        if (VALVE_STACK[i] == no)
        {
            P("%s %s\n", RED("valvePush Exists"), RED(no));
            return false;
        }
    }
    if (i == VALVE_STACK_MAX)
    {

        Serial.println("[!] Valve stack is full");
        return false;
    }
    VALVE_STACK[i] = no;
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
        for (int i = 1; i < VALVE_STACK_MAX; i++)
        {
            if (0 > (VALVE_STACK[i - 1] = VALVE_STACK[i]))
                break;
            else
                VALVE_STACK[i] = -1;
        }
        valveDump("popped");
    }
}

void valveSwitcher()
{
    int no;
    if (VALVE_CURRENT >= 0 /* && VALVE_CURRENT != VALVE_STACK[0] */)
    {
        SERIAL_REG[VALVE_START + VALVE_CURRENT] = OFF;
        P("Turn Off %d\n", VALVE_CURRENT);
        valve_next_check = millis() + 100;
        VALVE_CURRENT = -1;
    }
    // Stillhave in stack?
    if (VALVE_STACK[0] >= 0)
    {
        no = VALVE_STACK[0];
        // check again if it still need water, or force pressed. except from DHT22
        if (no == SPRAYER_NO || no == currentButton || ANALOG_SENSOR[no] <= config->humidity_minimal[no])
        {
            SERIAL_REG[VALVE_START + no] = ON;
            VALVE_CURRENT = no;
            valve_next_check = millis() + config->valve_delay[no] * 1000UL;
            P("Turn On %d for %d second\n", no, config->valve_delay[no]);
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
        P("Suhu panas (%s >= %d)\n", YELLOW(TEMPERATURE), config->temperature_max);
        valvePush(SPRAYER_NO);
    }
    else if (HUMIDITY < config->humidity_minimal[SPRAYER_NO])
    {
        P("Kelembaban kurang (%s < %d)\n", YELLOW(HUMIDITY), config->humidity_minimal[SPRAYER_NO]);
        valvePush(SPRAYER_NO);
    }
}

void smartgarden_loop()
{
    unsigned long now = millis();
    if (ir_remote_read())
    {
        if (currentButton >= RemoteButton.BTN_1 && currentButton <= RemoteButton.BTN_6)
        {
            valvePush(currentButton);
            valveSwitcher();
            smartgarden_apply();
            return;
        }
    }

    sensorsuhu_read();
    if( sensor_next_check < now ){
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
            // SERIAL_REG[PinSerial.Pompa] = ON;
            pompa_nyala_sejak = now;
            pompaChecker();
            smartgarden_apply();
        }
        else
        {
            if (((pompa_mati_sampai - now) / 1000))
            {
                status("Auto Off %5d detik", static_cast<uint8_t>((pompa_mati_sampai - now) / 1000) + 1);
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
            status("Sprayer ON %3d detik", VALVE_CURRENT + 1, remaining + 1);
        }
        else
        {
            status("No %d ON %5d detik", VALVE_CURRENT + 1, remaining + 1);
        }
    }

    if (valve_next_check <= now)
    {
        valveChecker();
        valveSwitcher();
        //valveDump("Valve ");
        pompaChecker();
        smartgarden_apply();
        //P("%dC %d%% %s\n", TEMPERATURE, HUMIDITY, BLUE("--------------------------\n"));
    } /* else{
        P("NOT YET\n");
        dumpSerial(PinSerial.Pompa, PinSerial.Pompa);
    } */
}
