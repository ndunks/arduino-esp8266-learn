#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

/**
 * See on readme.md for wiring
 */
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
uint32_t *BAUD_RATE;
uint8_t TOUCH_STATE = analogRead(A0) / 255;
char ch;
uint16_t lastCursor = 0;

void initSerial()
{
  Serial.begin(*BAUD_RATE);
  tft.printf("BAUD RATE: %u, Screen %d x %d\n", *BAUD_RATE, tft.width(), tft.height());
  lastCursor = tft.getCursorY();
}

void setup(void)
{
  EEPROM.begin(sizeof(uint32_t));
  BAUD_RATE = (uint32_t *)EEPROM.getDataPtr();

  if (*BAUD_RATE == 0 || *BAUD_RATE == 0xff)
  {
    *BAUD_RATE = 115200;
    EEPROM.commit();
  }
  tft.init();
  tft.setRotation(1);
  tft.setTextFont(2);
  tft.fillScreen(TFT_BLACK);
  initSerial();
}
void switch_baud_rate()
{
  if (*BAUD_RATE == 115200)
  {
    *((uint32_t *)EEPROM.getDataPtr()) = 9600;
  }
  else
  {
    *((uint32_t *)EEPROM.getDataPtr()) = 115200;
  }
  EEPROM.commit();
  Serial.end();
  delay(500);
  initSerial();
}

void loop()
{

  if (TOUCH_STATE == LOW)
  {
    // Touchec switch baud rate
    switch_baud_rate();
  }
  while (Serial.available())
  {
    ch = Serial.read();
    tft.print(ch);
    if (tft.getCursorY() != lastCursor)
    {
      Serial.printf("%d != %d\n", lastCursor, tft.getCursorY());
      lastCursor = tft.getCursorY();
      if (lastCursor >= tft.height())
      {
        lastCursor = 0;
        tft.setCursor(0, lastCursor);
        Serial.printf("%d >= %d\n", lastCursor, tft.height());
      }
      Serial.printf("Cursor %d\n", lastCursor);
      tft.fillRect(0, lastCursor, tft.width(), tft.fontHeight(), TFT_BLACK);
      tft.drawFastVLine(0, lastCursor, 16, TFT_GREEN);
    }

    //Serial.write(ch);
  }
}