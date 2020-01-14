#include "Arduino.h"
#include "smartgarden.h"

int value = 0;

void setup()
{
  Serial.begin(2000000);
  smartgarden_setup();
}

void loop()
{
  smartgarden_loop();
}