#include "display.h"
#include "smartgarden.h"

LiquidCrystal_I2C lcd;

unsigned long next_update_display = 0;
char statusBuffer[41] = {};
bool LCD_OK = false;
// maks 8 huruf
void statusSmall(const char *txt)
{
  if (!LCD_OK)
    return;
  lcd.setCursor(12, 2);
  if (txt)
  {
    lcd.printf(txt);
  }
  else
  {
    // clear
    for (int i = 0; i < 8; i++)
    {
      lcd.write(' ');
    }
  }
}

// maks 20 huruf
void status(const char *txt, ...)
{
  if (!LCD_OK)
    return;
  va_list args;
  va_start(args, txt);
  int len = vsprintf(statusBuffer, txt, args);
  va_end(args);
  if (len > 20)
  {
    P("STATUS TERLALU PANJANG %d\n", len);
  }
  else if (len < 20)
  {
    memset(&statusBuffer[len], ' ', 20 - len);
  }
  statusBuffer[20] = 0; // make sure is null terminated
  lcd.setCursor(0, 3);
  lcd.print(statusBuffer);
}

void display_setup()
{
  if (lcd.begin(20, 4))
  {
    LCD_OK = true;
    lcd.clear();
    lcd.print(F("*    GREENHOUSE    *"));
    lcd.print(F("--------------------"));
    lcd.print(F("*   BLKP KLAMPOK   *"));
    /* for (int i = 0; i < 30; i++)
    {
      lcd.setCursor(random(0, 19), random(0, 3));
      lcd.print('*');
      delay(100);
    }
    */
  }
  else
  {
    Serial.println(F("LCD Not found"));
  }
}

void display_loop()
{
  if (!LCD_OK || next_update_display > millis())
    return;

  for (int i = 0; i < (VALVE_COUNT - 1); i++)
  {
    lcd.setCursor(2 + ((i / 3) * 6), i % 3);
    lcd.printf("%-3d", ANALOG_SENSOR[i]);
  }
  // Suhu & kelemebaban
  lcd.setCursor(12, 1);
  lcd.printf("%2dc", TEMPERATURE);
  lcd.setCursor(16, 1);
  lcd.printf("%3d%%", HUMIDITY);
  next_update_display = millis() + 2000UL;
}
