/*
 NodeMCU/Wemos ESP8266 IC 4051 Analog (Input) Multiplexer
 by Mochamad Arifin
 
 Advanced example to read data from 4051 multiplexer with an esp8266
 using Direct GPIO.
 Wiring:
 NodeMCU ->  4051
 ---------------
// LSBFIRST, GPIO Must be insequence, for bitshift operation and short code!
 GPIO14 D6   ->   S0 (A)
 GPIO13 D7   ->   S1 (B)
 GPIO12 D5   ->   S2 (C)
       A0    ->   Common
       3.3v  ->   VCC
       G     ->   GND
       G     ->   Inhibit
       G     ->   VEE
*/
#include <Arduino.h>
#include <user_interface.h>

void DUMP_3BIT(const char *label, int byte, int value)
{
  printf("%s\t: %c%c%c\t= %d\n", label,
         (byte & B100) ? '1' : '0',
         (byte & B010) ? '1' : '0',
         (byte & B001) ? '1' : '0',
         value);
}

void setup()
{
  Serial.begin(115200);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D5, OUTPUT);
}

int ICACHE_RAM_ATTR analogReadX(int no)
{
  // Set bit switches on IC 4051
  GPOS = (no & B111) << 12;
  GPOC = (no ^ B111) << 12;
  // Persentase
  return static_cast<int>((system_adc_read() / 4) * (100.0f / 256.0f));
}

void loop()
{
  for (int i = 0; i < 6; i++)
  {
    Serial.printf("%d\t", analogReadX(i));
  }
  Serial.println();
  delay(1000);
}