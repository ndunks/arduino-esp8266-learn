#include "smartgarden.h"

// OUTUT/LED PIN
const uint8_t VALVE_START = PinSerial.Valve_0;

const uint8_t SPRAYER_NO = PinSerial.Sprayer - VALVE_START;

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
int VALVE_INDEX = -1;
unsigned long valve_next_switch = 0;
// Cek setiap sensor setiap:
unsigned long smartgarden_delay = 1000; // 1 second

SmartGardenConfig *smartgarden_config = nullptr;
unsigned long nextrun = 0;

void smartgarden_setup()
{
    std::fill_n(VALVE_STACK, VALVE_STACK_MAX, -1);

#ifdef SMARTGARDEN_DEBUG
    delay(1000);
#endif

    if (!smartgarden_config)
    {
        P("smartgarden config use default\n");
        smartgarden_config = new SmartGardenConfig();
        smartgarden_config->valve_delay_default = 5;
        smartgarden_config->humidity_minimal_default = 50;
        smartgarden_config->temperature_max = 32;

        for (int no = 0; no < VALVE_COUNT; no++)
        {
            smartgarden_config->valve_delay[no] = smartgarden_config->valve_delay_default;
            smartgarden_config->humidity_minimal[no] = smartgarden_config->humidity_minimal_default;

#ifdef SMARTGARDEN_DEBUG
            P("valve_delay  %d: %d\n", no, smartgarden_config->valve_delay[no]);
            P("humidity_min %d: %d\n", no, smartgarden_config->humidity_minimal[no]);
#endif
        }

        // spesial untuk humdity dari sensor DHT22
        smartgarden_config->humidity_minimal[PinSerial.Sprayer - VALVE_START] = 30;
    }
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

void setAnalog(int no)
{
    // Set bit switches on IC 4051
    SERIAL_REG[PinSerial.IC4051_SA] = (no >> 2) & 0x1;
    SERIAL_REG[PinSerial.IC4051_SB] = (no >> 1) & 0x1;
    SERIAL_REG[PinSerial.IC4051_SC] = (no >> 0) & 0x1;
    smartgarden_apply();
    dumpSerial(PinSerial.IC4051_SA, PinSerial.IC4051_SC);
}

int readAnalog(int no)
{
    setAnalog(no);
    // Persentase
    return static_cast<int>((system_adc_read() / 4) * (100.0f / 256.0f));
}

void readAllAnalog()
{
    for (int i = 0; i < 6; i++)
    {
        ANALOG_SENSOR[i] = readAnalog(i);
    }
    yield();
}

// Add valve to on when applied
bool valvePush(int no)
{
    if (VALVE_INDEX == VALVE_STACK_MAX - 1)
    {
        Serial.println("[!] Valve stack is full");
        return false;
    }
    else
    {
        int i = 0;
        while (i < VALVE_COUNT && VALVE_STACK[i] >= 0)
        {
            if (VALVE_STACK[i++] == no)
            {
                P(YELLOW("valvePush exists %d\n"), no);
                return false;
            };
        }
        VALVE_STACK[++VALVE_INDEX] = no;
        P("valvePush %d\n", no);
        return true;
    }
}

int valvePop()
{
    if (VALVE_INDEX < 0)
    {
        P("valvePop: No valve in stack\n");
        return -1;
    }
    else
    {
        P("valvePop: %d\n", VALVE_STACK[VALVE_INDEX]);
        VALVE_STACK[VALVE_INDEX] = -1;
        return VALVE_INDEX--;
    }
}

void valveDump(const char *prefix)
{
    Serial.printf("%s [%d]: ", prefix, VALVE_INDEX+1);
    int i = 0;
    while (i < VALVE_COUNT && VALVE_STACK[i] >= 0)
    {
        Serial.printf("%d ", VALVE_STACK[i++]);
    }
    Serial.print('\n');
}

void valveSwitcher()
{
    if (VALVE_INDEX < 0)
    {
        P("valveSwitcher: no valve in stack\n");
        return;
    }

    if (valve_next_switch > millis())
    {
        P("valveSwitcher: not yet\n");
        return;
    }
    int no;
    if (VALVE_CURRENT >= 0)
    {
        no = valvePop();
        SERIAL_REG[VALVE_START + no] = OFF;
        P("Turn Off %d\n", no);
    }
    // Stillhave in stack?
    if (VALVE_INDEX >= 0)
    {
        no = VALVE_STACK[VALVE_INDEX];
        SERIAL_REG[VALVE_START + no] = ON;
        VALVE_CURRENT = no;
        valve_next_switch = millis() + smartgarden_config->valve_delay[no] * 1000L;
        P("Turn On %d for %d second\n", no, smartgarden_config->valve_delay[no]);
    }
    else
    {
        valve_next_switch = 0;
        VALVE_CURRENT = -1;
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

        shouldOn = ANALOG_SENSOR[i] <= smartgarden_config->humidity_minimal[i];
        P("Sensor_%d: %d = %d\n", i, ANALOG_SENSOR[i], shouldOn);
        if (shouldOn)
        {
            valvePush(i);
        }
    }

    if (TEMPERATURE >= smartgarden_config->temperature_max)
    {
        P("Suhu panas (%s >= %d)\n", YELLOW(TEMPERATURE), smartgarden_config->temperature_max);
        valvePush(SPRAYER_NO);
    }
    else if (HUMIDITY < smartgarden_config->humidity_minimal[SPRAYER_NO])
    {
        P("Kelembaban kurang (%s < %d)\n", YELLOW(HUMIDITY), smartgarden_config->humidity_minimal[SPRAYER_NO]);
        valvePush(SPRAYER_NO);
    }
}

void smartgarden_loop()
{

    // if (nextrun > millis())
    // {
    //     return;
    // }
    nextrun = millis() + smartgarden_delay;
    sensorsuhu_read();
    readAllAnalog();
    valveChecker();
    valveSwitcher();
    valveDump("Valve ");
    // Jika ada salah satu valve yg on, maka pompa juga harus on
    SERIAL_REG[PinSerial.Pompa] = VALVE_INDEX >= 0;
    smartgarden_apply();
    dumpSerial(PinSerial.Valve_0, PinSerial.Sprayer);
}
