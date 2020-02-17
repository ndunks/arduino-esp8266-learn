#include <Arduino.h>
#include <ESP8266WiFi.h>
const uint8_t tick_max = 50;
volatile int32_t tick_snap[tick_max] = {};
volatile int8_t tick_counter = 0;

void record_loop()
{
  // https://sub.nanona.fi/esp8266/timing-and-ticks.html
  asm volatile("rsr %0, ccount"
               : "=r"(tick_snap[tick_counter++]));
}

void setup()
{
  WiFi.begin();
  Serial.begin(115200);
  record_loop();
}
void dump_tick(const char *title)
{
  Serial.printf("\n%s:\n", title);
  uint32_t gap = 0;
  float_t gap_us = 0.0f;
  for (int i = 0; i < tick_max; i++)
  {
    if (i > 0)
    {
      gap = tick_snap[i] - tick_snap[i - 1];
      gap_us = gap / 80.0f;
    }
    Serial.printf("%3u %0.2f us\n", gap, gap_us);
  }
  Serial.printf("\n");
}

void loop()
{
  if (tick_counter >= tick_max)
  {
    tick_counter = -1;
    dump_tick("DONE");
  }
  else
  {
    if (tick_counter < 0)
    {
      delay(1000);
    }
    else
    {
      record_loop();
    }
  }
}
/* 
WITHOUT WIFI    WITH WIFI
0 0.00 us       0 0.00 us
1010 12.62 us   695 8.69 us
1540 19.25 us   3753 46.91 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
595 7.44 us     595 7.44 us
*/