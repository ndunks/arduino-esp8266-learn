#include "Arduino.h"
#include "smartgarden.h"

int value = 0;


void setup()
{
  Serial.begin(115200);
  smartgarden_setup();
}

void loop()
{
  Serial.println("----------------");
  for (int i = 0; i < 6; i++)
  {
    Serial.printf("%*d=%d ", 4, ANALOG_SENSOR[i], (ANALOG_SENSOR[i] > 50));
    REG[OUTSTART + i] = (ANALOG_SENSOR[i] > (uint8)50) ? 1 : 0;
  }
  smartgarden_apply();
  Serial.println();
  delay(500);
}