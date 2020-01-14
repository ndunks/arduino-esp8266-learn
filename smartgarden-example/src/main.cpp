#include "Arduino.h"
#include "smartgarden.h"

int value = 0;

void setup()
{
  Serial.begin(115200);
  smartgarden_setup();
  while (!Serial)
  {
  }
  smartgarden_loop();
}

void smartgarden_loop_wait()
{
  if (Serial.available())
  {
    while (Serial.available())
    {
      Serial.read();
    }
    smartgarden_loop();
  }
}

void loop()
{
  delay(100);
  smartgarden_loop();
}