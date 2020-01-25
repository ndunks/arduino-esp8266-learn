#include "header.h"

void setup()
{
  Serial.begin(115200);
  detik_setup();
  display_setup();
  config_setup();
  smartgarden_setup();
  web_setup();

  // Setup Screen
  lcd.clear();
  lcd.print(F("1:    4:    Suhu Kel"
              "3:    6:            "
              "2:    5:    32c  93%"));
  ir_remote_setup();
}


void loop()
{
  ir_remote_loop();
  web_loop();
  smartgarden_loop();
  display_loop();
  delay(100);
}