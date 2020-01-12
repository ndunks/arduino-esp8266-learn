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
  smartgarden_loop();
  Serial.println("----------------");
  for (int i = 0; i < 6; i++)
  {
    Serial.printf("%*d=%d ", 4, ANALOG_SENSOR[i], (ANALOG_SENSOR[i] > 50));
    REG[VALVE_START + i] = (ANALOG_SENSOR[i] > (uint8)50) ? ON : OFF;
  }

  Serial.printf("\n%2fc\t%2f%%", TEMPERATURE, HUMIDITY);
  if (sensorsuhu_error != ERROR_NONE)
  {
    Serial.printf("\t(Err %d)", sensorsuhu_error);
  }

  if (TEMPERATURE > 40.0f || HUMIDITY < 40.0f)
  {
    REG[SPRAYER_NO] = ON;
  }
  else
  {
    REG[SPRAYER_NO] = OFF;
  }
  Serial.println();
  smartgarden_apply();
  delay(1000);
}