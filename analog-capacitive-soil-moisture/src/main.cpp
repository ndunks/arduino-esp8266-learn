#include <Arduino.h>
/**
 * Sensor: Capacitive Soil Moisture Sensor v.12
 * Pin Sensor -> PIN NodeMCU
 * GND        -> GND
 * VCC        -> VV (5V)
 * OUT        -> A0
 */

int analogValue = 0;
// Jika kurang dari ini, berarti tidak ada sensor dipasang
uint16_t SOIL_CALIBRATE_LOWEST = 170u;
// KERING = voltase tinggi = max 745
uint16_t SOIL_CALIBRATE_MAX = 745u;
// BASAH  = voltase rendah = min 270
uint16_t SOIL_CALIBRATE_MIN = 270u;
float_t SOIL_PERCENTAGE = 100.0f / static_cast<float_t>(SOIL_CALIBRATE_MAX - SOIL_CALIBRATE_MIN);

uint8_t persentase(int value)
{
  if (value > SOIL_CALIBRATE_MAX)
  {
    value = SOIL_CALIBRATE_MAX;
  }
  else if (value <= SOIL_CALIBRATE_LOWEST)
  {
    // tidak ada sensor dipasang
    value = SOIL_CALIBRATE_MAX;
  }
  else if (value < SOIL_CALIBRATE_MIN)
  {
    value = SOIL_CALIBRATE_MIN;
  }
  return static_cast<uint8_t>(100 - ((value - SOIL_CALIBRATE_MIN) * SOIL_PERCENTAGE));
}

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  analogValue = analogRead(A0);
  Serial.printf("%3d = %3d%%\n", analogValue, persentase(analogValue));
  delay(1000);
}