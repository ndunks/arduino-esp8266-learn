#include "smartgarden.h"
// Serial registers
uint8 REG[16] = {LOW};

// Analog 0 - 5    Sensor Kelembaban Tanah
uint8 ANALOG_SENSOR[6] = {0};

// Analog 6
float HUMIDITY = 0.0f;
float TEMPERATURE = 0.0f;

// Cek setiap sensor setiap:
unsigned long smartgarden_delay = 1000;

// Berapa valve yg on?
STATUS_t checkPompaStatus()
{
    for (int no = VALVE_START; no < (VALVE_START + VALVE_COUNT); no++)
    {
        // Jika salah satu valve ada yang on, maka pompa harus ON
        if (REG[no])
            return ON;
    }
    return OFF;
}

/** Apply REG value to 2 chained IC 595 */
void smartgarden_apply()
{
    REG[POMPA_NO] = checkPompaStatus();

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

void setOutput(uint8 no, STATUS_t value)
{
    REG[no] = value;
    smartgarden_apply();
}

void smartgarden_setup()
{
    pinMode(SERIAL_DATA, OUTPUT);
    pinMode(SERIAL_LOAD, OUTPUT);
    pinMode(SERIAL_CLOCK, OUTPUT);
    pinMode(SENSOR_SUHU_PIN, INPUT);
    //reset
}

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

void smartgarden_loop()
{
    _readAllAnalog();
    sensorsuhu_read();
}
