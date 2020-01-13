#include "smartgarden.h"

// Serial registers
uint8 REG[16] = {LOW};

// Analog 0 - 5    Sensor Kelembaban Tanah, tanpa DHT22
uint8 ANALOG_SENSOR[VALVE_COUNT - 1] = {};

// Analog 6
uint8 HUMIDITY = 0;
int8 TEMPERATURE = 0;

// Valve yg saat ini sedang on
int VALVE_CURRENT = -1;

// List antrean valve, karena hanya 1 valve yg open dalam satu waktu
int VALVE_STACK[VALVE_STACK_MAX] = {};
int VALVE_INDEX = -1;
unsigned long valve_next_switch = 0;
// Cek setiap sensor setiap:
unsigned long smartgarden_delay = 5000; // 1 second

SmartGardenConfig *smartgarden_config = nullptr;
unsigned long nextrun = 0;

void smartgarden_setup()
{
    std::fill_n(VALVE_STACK, VALVE_STACK_MAX, -1);

#ifdef DEBUG_ESP_CORE
    delay(1000);
    DEBUGV("......................\n\n");
    DEBUGV("smartgarden start\n");
#endif

    if (!smartgarden_config)
    {
        DEBUGV("No smartgarden config\n");
        smartgarden_config = new SmartGardenConfig();
        smartgarden_config->valve_delay_default = 10;
        smartgarden_config->humidity_minimal_default = 50;
        smartgarden_config->temperature_max = 30.0f;

        for (int no = 0; no < VALVE_COUNT; no++)
        {
            smartgarden_config->valve_delay[no] = smartgarden_config->valve_delay_default;
            smartgarden_config->humidity_minimal[no] = smartgarden_config->humidity_minimal_default;
        }

        // spesial untuk humdity dari sensor DHT22
        smartgarden_config->humidity_minimal[SPRAYER_NO] = 30;
    }

    DEBUGV("SMARTGARDEN CONFIG:\nvalve_delay_default: %d\n", smartgarden_config->valve_delay_default);

    pinMode(SERIAL_DATA, OUTPUT);
    pinMode(SERIAL_LOAD, OUTPUT);
    pinMode(SERIAL_CLOCK, OUTPUT);
    pinMode(SENSOR_SUHU_PIN, INPUT);
    //reset
}

/** Apply REG value to 2 chained IC 595 */
void smartgarden_apply()
{
    // Jika ada salah satu valve yg on, maka pompa juga harus on
    REG[POMPA_NO] = VALVE_INDEX >= 0;

    digitalWrite(SERIAL_LOAD, LOW);
    for (int i = 15; i >= 0; i--)
    {
        digitalWrite(SERIAL_DATA, REG[i] & B1);
        //Serial.printf("%d", REG[i] & B1);
        digitalWrite(SERIAL_CLOCK, HIGH);
        digitalWrite(SERIAL_CLOCK, LOW);
    }
    digitalWrite(SERIAL_LOAD, HIGH);
    //Serial.printf("\n");
}

// Activate analog selector
void smartgarden_set_analog(int no)
{
    // Set bit switches on IC 4051
    REG[SC] = (no >> 2) & B1;
    REG[SB] = (no >> 1) & B1;
    REG[SA] = (no >> 0) & B1;
    smartgarden_apply();
}

int smartgarden_read_analog(int no)
{
    smartgarden_set_analog(no);
    // Persentase
    return static_cast<int>((system_adc_read() / 4) * (100.0f / 256.0f));
}

void _readAllAnalog()
{
    for (int i = 0; i < 6; i++)
    {
        ANALOG_SENSOR[i] = smartgarden_read_analog(i);
    }
}

/* 
void setOutput(uint8 no, STATUS_t value)
{
    REG[no] = value;
    smartgarden_apply();
}
 */

/**
 * Call smartgarden_apply() to see changes
 */
/* void smartgarden_setValve(int no, STATUS_t status)
{
    if (no < 0 || no > VALVE_COUNT)
    {
        Serial.printf("Nomor katup salah %d\n", no);
        return;
    }
    setOutput(VALVE_START + no, status);
}

void smartgarden_setPengembunan(STATUS_t status)
{
    setOutput(SPRAYER_NO, status);
}
 */

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
                DEBUGV("valvePush exists %d\n", no);
                return false;
            };
        }
        VALVE_STACK[++VALVE_INDEX] = no;
        DEBUGV("valvePush %d\n", no);
        return true;
    }
}

bool valvePop()
{
    if (VALVE_INDEX < 0)
    {
        Serial.println("valvePop: No valve in stack");
        return false;
    }
    VALVE_STACK[VALVE_INDEX] = -1;
    return VALVE_INDEX--;
}

void valveDump(const char *prefix)
{
    Serial.printf("%s [%d]: ", prefix, VALVE_INDEX);
    int i = 0;
    while (i < VALVE_COUNT && VALVE_STACK[i] >= 0)
    {
        Serial.printf("%d ", VALVE_STACK[i++]);
    }
    Serial.println();
}

void valveSwitcher()
{
    if (VALVE_INDEX < 0)
    {
        DEBUGV("valveSwitcher: no valve in stack\n");
    }

    if (valve_next_switch > millis())
    {
        DEBUGV("valveSwitcher: not yet\n");
        return;
    }
    REG[VALVE_START + valvePop()] = OFF;
    // Stillhave in stack?
    if (VALVE_INDEX >= 0)
    {
        int no = VALVE_STACK[VALVE_INDEX];
        REG[VALVE_START + no] = ON;
        valve_next_switch = millis() + smartgarden_config->valve_delay[no];
        DEBUGV("Turn On %d for %d second\n", no, smartgarden_config->valve_delay[no] / 1000);
    }
}

void valveChecker()
{
    DEBUGV("valveChecker\n");
    bool shouldOn = false;
    for (int i = 0; i < VALVE_COUNT; i++)
    {
        // Skip check for sprayer
        if (i == SPRAYER_NO)
            continue;

        shouldOn = ANALOG_SENSOR[i] <= smartgarden_config->humidity_minimal[i];
        DEBUGV("%*d=%d ", 4, ANALOG_SENSOR[i], shouldOn);
        if (shouldOn)
        {
            valvePush(i);
        }
    }
    DEBUGV("\n");

    if (TEMPERATURE >= smartgarden_config->temperature_max || HUMIDITY < smartgarden_config->humidity_minimal[SPRAYER_NO])
    {
        valvePush(SPRAYER_NO);
    }
}

void smartgarden_loop()
{

    if (nextrun > millis())
    {
        return;
    }

    nextrun = millis() + smartgarden_delay;
    digitalWrite(LED_BUILTIN, HIGH);
    _readAllAnalog();
    sensorsuhu_read();
    valveDump("Cur");
    valveSwitcher();
    valveDump("Off");
    valveChecker();
    valveDump("On ");
    smartgarden_apply();
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
}
