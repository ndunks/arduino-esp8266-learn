#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

void setup(void)
{
  tft.init();
  tft.setRotation(1);
  Serial.begin(115200);
  tft.fillScreen(0x0000);
}

char ch = 0;
void loop()
{
  while (Serial.available())
  {
    ch = Serial.read();
    tft.print(ch);
    Serial.write(ch);
    // TODO SCROLL
  }
}