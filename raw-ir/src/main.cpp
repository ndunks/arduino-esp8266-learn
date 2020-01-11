#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

uint16_t irPin = 4;
IRrecv irrecv(irPin);
decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop()
{
  delay(500);
  if (!irrecv.decode(&results)){
    Serial.print(".");
    return;
  }

  // ignore no bits (Missed)
  if (results.bits > 0)
  {
    switch (results.value)
    {
    default:
      Serial.print("Mbuh");
      Serial.println(resultToSourceCode(&results));
      break;
    }
  }
  irrecv.resume(); // Receive the next value
}