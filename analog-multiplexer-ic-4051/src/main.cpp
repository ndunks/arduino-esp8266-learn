#include <Arduino.h>
/*
 ESP8266 4051 Multiplexer
 NodeMCU ->  4051
 ---------------
 D7    ->   S0 (A)
 D5    ->   S1 (B)
 D6    ->   S2 (C)
 A0    ->   Common
 3.3v  ->   VCC
 G     ->   GND
 G     ->   Inhibit
 G     ->   VEE  
*/

#define SA D7
#define SB D5
#define SC D6

void setup()
{
  Serial.begin(115200);
  pinMode(SA, OUTPUT);
  pinMode(SB, OUTPUT);
  pinMode(SC, OUTPUT);
}

int analogReadX(int bit)
{
  digitalWrite(SA, (bit >> 2) & B1); // MSB
  digitalWrite(SB, (bit >> 1) & B1);
  digitalWrite(SC, (bit >> 0) & B1); // LSB
  // Persentase
  return static_cast<int>((analogRead(A0) / 4) * (100.0f / 256.0f));
}

void loop()
{
  Serial.printf("%d\t", analogReadX(0)); // X0
  Serial.printf("%d\t", analogReadX(1)); // X1
  Serial.printf("%d\t", analogReadX(2)); // X2
  Serial.printf("%d\t", analogReadX(3)); // X3
  Serial.printf("%d\t", analogReadX(4)); // X4
  Serial.printf("%d\t", analogReadX(5)); // X5
  Serial.printf("%d\t", analogReadX(6)); // X6
  Serial.printf("%d\t", analogReadX(7)); // X7
  Serial.println();
  delay(1000);
}