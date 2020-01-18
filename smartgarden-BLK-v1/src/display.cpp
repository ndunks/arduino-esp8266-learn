#include "display.h"
#include "smartgarden.h"

LiquidCrystal_I2C lcd;

unsigned long next_update_display = 0;
char statusBuffer[21] = {};

void status(const char *txt, ...)
{
  va_list args;
  va_start(args, txt);
  int len = vsprintf(statusBuffer, txt, args);
  va_end(args);
  memset(&statusBuffer[len], ' ', 20 - len);
  statusBuffer[20] = 0; // make sure is null terminated
  lcd.setCursor(0, 3);
  lcd.print(statusBuffer);
}

void display_setup()
{
  if (!lcd.begin(20, 4))
  {
    Serial.println(F("LCD Not found"));
  }
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

void display_loop()
{
  if (next_update_display > millis())
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
