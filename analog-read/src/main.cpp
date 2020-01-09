/**
 * Menggunakan Potensio Meter kaki tiga Rv09
 * Kaki tengah merupakan output data analog 0 - 1023
 * Kaki samping-samping merupakan positif dan negatif
 * Tertukar tidak masalah, hanya saja nilainya akan terbalik
 */
#include <Arduino.h>

int analogValue = 0;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  analogValue = analogRead(A0);
  Serial.println(analogValue);
  delay(700);
}