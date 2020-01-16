/***************************************************************************************************/
/*
  This is an Arduino sketch for LiquidCrystal_I2C library.

  This chip uses I2C bus to communicate, specials pins are required to interface
  Board:                                    SDA                    SCL                    Level
  Uno, Mini, Pro, ATmega168, ATmega328..... A4                     A5                     5v
  Mega2560................................. 20                     21                     5v
  Due, SAM3X8E............................. 20                     21                     3.3v
  Leonardo, Micro, ATmega32U4.............. 2                      3                      5v
  Digistump, Trinket, ATtiny85............. 0/physical pin no.5    2/physical pin no.7    5v
  Blue Pill, STM32F103xxxx boards.......... PB7                    PB6                    3.3v/5v
  ESP8266 ESP-01........................... GPIO0/D5               GPIO2/D3               3.3v/5v
  NodeMCU 1.0, WeMos D1 Mini............... GPIO4/D2               GPIO5/D1               3.3v/5v
  ESP32.................................... GPIO21/D21             GPIO22/D22             3.3v

  Frameworks & Libraries:
  ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
  ESP32 Core            - https://github.com/espressif/arduino-esp32
  ESP8266 Core          - https://github.com/esp8266/Arduino
  ESP8266 I2C lib fixed - https://github.com/enjoyneering/ESP8266-I2C-Driver
  STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

  GNU GPL license, all text above must be included in any redistribution, see link below for details:
  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define COLUMS 20
#define ROWS 4

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  //Wire.begin(SDA, SCL);
  Serial.write("Checking LCD...\n");
  while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS, SDA, SCL) != 1) //colums - 20, rows - 4
  {
    Serial.println("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal.");
    delay(5000);
  }
  Serial.println("PCF8574 is OK...");
  lcd.clear();
  lcd.print("PCF8574 is OK...");
  lcd.setCursor(0, 1); //set 1-st colum & 2-nd row, 1-st colum & row started at zero
  lcd.print("Hello world!");
  lcd.setCursor(0, 2); //set 1-st colum & 3-rd row, 1-st colum & row started at zero
  lcd.print("LCD OKKK!");
  lcd.setCursor(0, 3); //set 1-st colum & 3-rd row, 1-st colum & row started at zero
  lcd.print("More LINES!");
}

uint8 buff[255] = {};
uint8 current = 0;
uint8 len = 0;
void loop()
{
  // when characters arrive over the serial port...
  while (Serial.available())
  {
    current = Serial.read();
    if (current == '\n')
    {
      buff[len] = 0; // null terminated
      len--;         // Remove null
      // clear the screen
      lcd.clear();
      lcd.backlight();
      Serial.printf("\nWrite %d char: %s", len, buff);
      for (int i = 0; i < len; i++)
      {
        lcd.write(buff[i]);
      }
      len = 0;
      //Serial.println("Cursor: %d", getCursorPosition)
    }
    else
    {
      lcd.write(len);
      buff[len++] = current;
      lcd.noBacklight();
    }
    Serial.write(current);
  }
}
