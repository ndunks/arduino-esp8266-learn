#include "Arduino.h"
#include "smartgarden.h"

LiquidCrystal_I2C lcd;
char statusBuffer[21] = {};

void setup()
{
  Serial.begin(115200);
  if (!lcd.begin(20, 4))
  {
    Serial.println(F("LCD Not found"));
  }
  lcd.clear();
  smartgarden_setup();
  smartgarden_loop();
  /* lcd.print(F("--------------------"));
  lcd.print(F("|   SMART GARDEN   |"));
  lcd.print(F("|   BLKP KLAMPOK   |"));
  lcd.print(F("--------------------"));

  for (int i = 0; i < 30; i++)
  {
    lcd.setCursor(random(0, 19), random(0, 3));
    lcd.print('*');
    delay(100);
  }
  lcd.clear(); */
  // Setup Screen
  lcd.print(F("1:    4:    Suhu Kel"
              "3:    6:            "
              "2:    5:    32c  93%"));
}
void displaySetNo(uint8 no, uint8 value)
{
}
void smartgarden_loop_wait()
{
  if (Serial.available())
  {
    while (Serial.available())
    {
      Serial.read();
    }
    smartgarden_loop();
  }
}
void display_loop()
{
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
}

// Write status text
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

void waitSerialInput(void callback(void))
{
  if (Serial.available())
  {
    while (Serial.available())
    {
      Serial.read();
    }
    callback();
  }
}
void loop()
{
  delay(100);
  display_loop();
  int8_t code = ir_remote_read();
  if (code != RemoteButton.NONE)
  {
    P("PRESSED: %s\n", codeMaps[code].name);
  }
  //waitSerialInput(smartgarden_loop);
  smartgarden_loop();
}