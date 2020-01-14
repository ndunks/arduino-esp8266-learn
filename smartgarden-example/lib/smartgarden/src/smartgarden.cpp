#include "smartgarden.h"

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
        smartgarden_config->temperature_max = 30;

        for (int no = 0; no < VALVE_COUNT; no++)
        {
            smartgarden_config->valve_delay[no] = smartgarden_config->valve_delay_default;
            smartgarden_config->humidity_minimal[no] = smartgarden_config->humidity_minimal_default;

#ifdef SMARTGARDEN_DEBUG
            for (int no = 0; no < VALVE_COUNT; no++)
            {
                P("valve_delay  %d: %d\n", no, smartgarden_config->valve_delay[no]);
                P("humidity_min %d: %d\n", no, smartgarden_config->humidity_minimal[no]);
            }
#endif
        }

        // spesial untuk humdity dari sensor DHT22
        smartgarden_config->humidity_minimal[SPRAYER_NO] = 30;
    }
    pinMode(SERIAL_DATA, OUTPUT);
    pinMode(SERIAL_LOAD, OUTPUT);
    pinMode(SERIAL_CLOCK, OUTPUT);
    pinMode(SENSOR_SUHU_PIN, INPUT);
}

/** Apply REG value to 2 chained IC 595 */
void smartgarden_apply(bool debug = false)
{
    // Jika ada salah satu valve yg on, maka pompa juga harus on
    SERIAL_REG[PinSerial.Pompa] = VALVE_INDEX >= 0;
    digitalWrite(SERIAL_LOAD, LOW);
    for (int i = 15; i >= 0; i--)
    {

        digitalWrite(SERIAL_DATA, SERIAL_REG[i] & B1);
        digitalWrite(SERIAL_CLOCK, HIGH);
        digitalWrite(SERIAL_CLOCK, LOW);
        if (!debug)
            continue;

        if (i == PinSerial.Pompa)
        {
            P(" > Pompa   : %s\n", ONOFF(SERIAL_REG[i]));
        }
        else if (i == PinSerial.Sprayer)
        {
            P(" > Sprayer : %s\n", ONOFF(SERIAL_REG[i]));
        }
        else if (i >= PinSerial.Valve_0 && i <= PinSerial.Valve_5)
        {
            P(" > Valve  %d: %s\n", i - VALVE_START, ONOFF(SERIAL_REG[i]));
        }
    }
    digitalWrite(SERIAL_LOAD, HIGH);
}

// Activate analog selector
void smartgarden_set_analog(int no)
{
    // Set bit switches on IC 4051
    SERIAL_REG[PinSerial.IC4051_SA] = (no >> 2) & B1;
    SERIAL_REG[PinSerial.IC4051_SB] = (no >> 1) & B1;
    SERIAL_REG[PinSerial.IC4051_SA] = (no >> 0) & B1;
    smartgarden_apply(false);
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
} /* 
String valveName(int no)
{
    switch (no)
    {
    case POMPA_NO:
        return "Pompa";
        break;

    default:
        break;
    }
    if (i == POMPA_NO)
    {
        P(" > Pompa   : %s\n", ONOFF(SERIAL_REG[i]));
    }
    else if (i == VALVE_START + SPRAYER_NO)
    {
        P(" > Sprayer : %s\n", ONOFF(SERIAL_REG[i]));
    }
    else if (i >= VALVE_START && i <= VALVE_START + VALVE_COUNT)
    {
        P(" > Valve  %d: %s\n", i - VALVE_START, ONOFF(SERIAL_REG[i]));
    }
} */
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
                P("valvePush exists %d\n", no);
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
    Serial.printf("%s [%d]: ", prefix, VALVE_INDEX);
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
    P("valveChecker\n");
    bool shouldOn = false;
    for (int i = 0; i < VALVE_COUNT; i++)
    {
        // Skip check for sprayer
        if (i == SPRAYER_NO)
            continue;

        shouldOn = ANALOG_SENSOR[i] <= smartgarden_config->humidity_minimal[i];
        P("%*d=%d ", 4, ANALOG_SENSOR[i], shouldOn);
        if (shouldOn)
        {
            valvePush(i);
        }
    }
    P("\n\n");

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
    sensorsuhu_read();
    P("%s %s %s\n", MAGENTA("Suhu"), CYAN(TEMPERATURE), YELLOW(HUMIDITY));
    _readAllAnalog();
    valveDump("Cur");
    valveSwitcher();
    valveDump("Off");
    valveChecker();
    valveDump("On ");
    smartgarden_apply(true);
}
