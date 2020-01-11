
#include "DHTesp.h"
#include <Ticker.h>
#include <user_interface.h>
#include "smartgarden.h"

Ticker ticker;
DHTesp dht;

// Serial registers
uint8 REG[16] = {LOW};

// Analog 0 - 5    Sensor Kelembaban Tanah
uint8 ANALOG_SENSOR[6] = {0};

// Analog 6
float HUMIDITY = 0.0f;
float TEMPERATURE = 0.0f;

// Cek setiap sensor setiap:
unsigned long smartgarden_delay = 500;


/** Apply REG value to 2 chained IC 595 */
void smartgarden_apply()
{
  digitalWrite(loadPin, LOW);
  for (int i = 15; i >= 0; i--)
  {
    digitalWrite(dataPin, REG[i] & B1);
    LOG("%d", REG[i] & B1);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }
  digitalWrite(loadPin, HIGH);
  LOG("\n");
}


int _readAnalog(int no)
{
  // Set bit switches on IC 4051
  REG[SC] = (no >> 2) & B1;
  REG[SB] = (no >> 1) & B1;
  REG[SA] = (no >> 0) & B1;
  smartgarden_apply();
  // Persentase
  return static_cast<int>((system_adc_read() / 4) * (100.0f / 256.0f));
}

void _readAllAnalog()
{
  for (int i = 0; i < 6; i++)
  {
    ANALOG_SENSOR[i] = _readAnalog(i);
  }
}

void _readDHT22(){

}

void _runtime(){
    _readAllAnalog();
    _readDHT22();
}

void setOutput(uint8 no, uint8 value)
{
  REG[no] = value;
  smartgarden_apply();
}

void smartgarden_setup()
{
  pinMode(dataPin, OUTPUT);
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  ticker.attach_ms(smartgarden_delay, _runtime);
  LOG("Smart Garden Start %d\n", milis());
}