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

/** Apply REG value to 2 chained IC 595 */
void smartgarden_apply()
{
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

void setOutput(uint8 no, uint8 value)
{
    REG[no] = value;
    smartgarden_apply();
}

void smartgarden_setup()
{
    pinMode(SERIAL_DATA, OUTPUT);
    pinMode(SERIAL_LOAD, OUTPUT);
    pinMode(SERIAL_CLOCK, OUTPUT);
    //reset
}

void smartgarden_loop()
{
    _readAllAnalog();
    smartgarden_apply();
    sensorsuhu_read();
}
