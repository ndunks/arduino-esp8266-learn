#include <Arduino.h>
/*  
 Test the tft.print() viz the libraries embedded write() function
 This sketch used font 2, 4, 7
 
 Make sure all the required fonts are loaded by editting the
 User_Setup.h file in the TFT_eSPI library folder.
  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################
 */

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
  }
}