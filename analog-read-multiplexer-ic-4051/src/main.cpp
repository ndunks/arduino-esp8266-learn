#include <Arduino.h>
/*
 ESP8266 4051 Multiplexer by Brian Lough
 
 An example showing how to use a 4051 multiplexer with an esp8266
 to connect up to 8 analog sensors.
 Wiring:
 Wemos ->  4051
 ---------------
 D4    ->   S0 (A)
 D3    ->   S1 (B)
 D2    ->   S2 (C)
 A0    ->   Common
 3.3v  ->   VCC
 G     ->   GND
 G     ->   Inhibit
 G     ->   VEE  
 
 4051 Option pins are then wired to whatever Analog sensors required
 One thing to note: say for example if you only require 2 analog sensors,
 You can just wire up S0(A) and connect S1 & S2 to GND and you will be 
 able to switch between option 1 and option 2 pins.
 Same goes for up to 4 pins (just use S0 & S1)
*/

#define MUX_A D4
#define MUX_B D3
#define MUX_C D2

void setup()
{
  Serial.begin(115200);
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  pinMode(MUX_C, OUTPUT);
}

int analogReadX(int bit)
{
  digitalWrite(MUX_A, (bit >> 2) & B1);
  digitalWrite(MUX_B, (bit >> 1) & B1);
  digitalWrite(MUX_C, (bit >> 0) & B1);
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
  // Serial.printf("%d\t", analogReadX(6)); // X6
  // Serial.printf("%d\t", analogReadX(7)); // X7
  Serial.println();
  delay(1000);
}