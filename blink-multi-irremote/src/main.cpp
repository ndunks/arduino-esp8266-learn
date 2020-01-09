#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

int ledPins[6] = {4, 5, 15, 13, 12, 14};
int ledOnIndex = -1;
int lastOnIndex = -1;
unsigned long previousMillis = 0;
unsigned long interval = 3000;

uint16_t irPin = 2;
IRrecv irrecv(irPin);
decode_results results;

void setup()
{
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver
  for (int i = 0; i < 6; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void loop()
{

  unsigned long currentMillis = millis();
  if (lastOnIndex >= 0 && currentMillis - previousMillis >= interval)
  {
    digitalWrite(ledPins[lastOnIndex], LOW);
    lastOnIndex = -1;
  }

  if (!irrecv.decode(&results))
    return;

  // ignore no bits (Missed)
  if (results.bits > 0)
  {
    switch (results.value)
    {
    case 0xE0E020DF:
      ledOnIndex = 0;
      break;
    case 0xE0E0A05F:
      ledOnIndex = 1;
      break;
    case 0xE0E0609F:
      ledOnIndex = 2;
      break;
    case 0xE0E010EF:
      ledOnIndex = 3;
      break;
    case 0xE0E0906F:
      ledOnIndex = 4;
      break;
    case 0xE0E050AF:
      ledOnIndex = 5;
      break;
    case 0xE0E040BF:
      ledOnIndex = -1;
      break;
    default:
      Serial.print("Mbuh");
      Serial.println(resultToSourceCode(&results));
      ledOnIndex = -2;
      break;
    }
    if (ledOnIndex >= -1)
    {
      if (lastOnIndex >= 0)
      {
        digitalWrite(ledPins[lastOnIndex], LOW);
      }
      if (ledOnIndex >= 0)
      {
        digitalWrite(ledPins[ledOnIndex], HIGH);
        previousMillis = millis();
      }
      lastOnIndex = ledOnIndex;
    }
  }
  irrecv.resume(); // Receive the next value
}