#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
/**
 * See on readme.md for wiring
 */
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
// Landscape orientation
uint16_t maxCursor = 0;
uint16_t *lineBuffer;
uint32_t counter = 0U;
uint8_t maxLines = 0;
uint8_t fontHeight = 0;
void setup(void)
{
  tft.init();
  tft.setRotation(1);
  tft.setTextFont(2);
  Serial.begin(115200);
  fontHeight = tft.fontHeight();
  maxCursor = tft.height() - fontHeight;
  maxLines = tft.height() / fontHeight;
  lineBuffer = (uint16_t *)calloc(tft.width() * fontHeight, sizeof(uint16_t));
  tft.fillScreen(TFT_BLACK);
}
void ICACHE_RAM_ATTR moveUp()
{
  for (int i = 1; i < maxLines; i++)
  {
    tft.readRect(0, i * fontHeight, tft.width(), fontHeight, lineBuffer);
    tft.fillRect(0, (i - 1) * fontHeight,  tft.width(), fontHeight, TFT_BLUE);
    tft.pushImage(0, (i - 1) * fontHeight, tft.width(), fontHeight, lineBuffer);
  }
  // clean last line?
}

/* void redirectSerial()
{
  char ch = 0;
  while (Serial.available())
  {
    ch = Serial.read();
    tft.print(ch);
    Serial.write(ch);
    // TODO SCROLL
  }
} */

void loop()
{
  uint8_t TOUCH_STATE = analogRead(A0) / 255;
  if (tft.getCursorY() >= tft.height())
  {
    // clear last line
    moveUp();
    tft.setCursor(0, maxCursor);
    Serial.println("Move region");
    //tft.readRect(0, 0, tft.width(), tft.height(), pixelBuff);
  }
  tft.printf("%d %d\t: ", ++counter, tft.getCursorY());
  tft.println(String(analogRead(A0)).c_str());
  Serial.printf("%d %d\t: ", counter, tft.getCursorY());
  Serial.println(String(analogRead(A0)).c_str());
  //tft.drawNumber(, 0, 0, 2); // plot value in font 2
  delay(200);
  //tft.scroll(0, 16);
}