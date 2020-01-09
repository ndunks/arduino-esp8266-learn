#include <Arduino.h>
#include <Ticker.h>
#include <EEPROM.h>

int ledPin = D4;
int btnPin = D8; // RX GPIO 03
int ledState = LOW;

Ticker ledTimer;
// fake data
struct
{
  float delayTimer = 0;
} data;

void blink()
{
  digitalWrite(ledPin, ledState = !ledState);
}

void setup()
{
  EEPROM.begin(sizeof(data));
  EEPROM.get(0, data);
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT);
  digitalWrite(ledPin, ledState);
  ledTimer.attach(data.delayTimer, blink);
  delay(1000);
  if (isnan(data.delayTimer))
  {
    data.delayTimer = 0.25f;
  }
  Serial.printf("Data (%d): %f\n", sizeof(data), data.delayTimer);
}

void loop()
{
  if (digitalRead(btnPin) == HIGH)
  {

    if (data.delayTimer >= 3.0f)
    {
      data.delayTimer = 0.0f;
    }
    data.delayTimer += 0.25;
    ledTimer.detach();
    ledTimer.attach(data.delayTimer, blink);
    Serial.printf("Delay: %f", data.delayTimer);
    Serial.println();
    EEPROM.put(0, data);
    EEPROM.commit();
    delay(500);
  }
}