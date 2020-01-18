
#include "header.h"

char device_id[7] = {0};

void setup()
{
  Serial.begin(115200);
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

/* void waitSerialInput(void callback(void))
{
  if (Serial.available())
  {
    while (Serial.available())
    {
      Serial.read();
    }
    callback();
  }
} */
void loop()
{
  //waitSerialInput(smartgarden_loop);
  delay(100);
  ir_remote_loop();
  smartgarden_loop();
  display_loop();
  web_loop();
}