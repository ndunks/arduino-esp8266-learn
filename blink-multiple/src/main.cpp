#include <Arduino.h>

int ledPins[6] = {1, 3, 15, 13, 12, 14};
void setup()
{
  for (int i = 0; i < 6; i++)
  {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop()
{
  for (int i = 0; i < 6; i++)
  {
    // Harus pakai resistor pull-up
    digitalWrite(ledPins[i], HIGH);
    delay(200);
    digitalWrite(ledPins[i], LOW);
  }
}