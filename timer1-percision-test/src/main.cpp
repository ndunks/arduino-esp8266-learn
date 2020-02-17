#include <Arduino.h>
#include <ESP8266WiFi.h>

const uint8_t tick_max = 8;
volatile int32_t tick_snap[tick_max] = {};
volatile uint8_t tick_counter = 0;
volatile int8_t done = 0;
const uint32_t tick_delay = 80; // tick

// https://sub.nanona.fi/esp8266/timing-and-ticks.html
static inline int32_t asm_ccount(void)
{
  int32_t r;
  asm volatile("rsr %0, ccount"
               : "=r"(r));
  return r;
}

void ICACHE_RAM_ATTR timer_tick()
{
  do
  {
    // difference casted as uint32_t.
    // This magically compensates if etime overflows returning
    // difference of value despite which is larger and which smaller.
    // minus tick is over head, 23 overhead is based on code execution
    while (((uint32_t)(asm_ccount() - tick_snap[tick_counter - 1])) < tick_delay - 23)
    {
    }
    // Exact 80 tick delay = 1 us
    tick_snap[tick_counter] = asm_ccount();
  } while ((++tick_counter) < tick_max);
  done++;
}
void clear_tick()
{
  tick_counter = 0;
  memset((void *)tick_snap, 0, sizeof(tick_snap));
}
void start_tick(timercallback callback)
{
  clear_tick();
  timer1_attachInterrupt(callback);
  /**
   * TIM_DIV1     80MHz (80 ticks/us - 104857.588 us max)
   *              Under 1 second loop/timer
   * 
   * TIM_DIV16    5MHz (5 ticks/us - 1677721.4 us max)
   *              1 second = 5 * 1000 * 1000 = 5000000 tick
   * 
   * TIM_DIV256   312.5Khz (1 tick = 3.2us - 26843542.4 us max)
   *              1 second = 1000 * 1000 / 3.2 = 312500 tick
   */
  timer1_write(0); //80 tick at 80MHz
  tick_snap[tick_counter++] = asm_ccount();
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
}
void dump_tick(const char *title)
{
  Serial.printf("\n%s:\n  ", title);
  uint32_t gap = 0;
  float_t gap_us = 0.0f;
  for (int i = 0; i < tick_max; i++)
  {
    if (i > 0)
    {
      gap = tick_snap[i] - tick_snap[i - 1];
      gap_us = gap / 80.0f;
    }
    Serial.printf("%3u %0.2f us\t", gap, gap_us);
  }
  Serial.printf("\n");
}

void setup()
{
  Serial.begin(115200);
  timer1_isr_init();
}

void loop()
{
  switch (done)
  {
  case 0:
    if (!timer1_enabled())
    {
      start_tick(timer_tick);
    }
    break;
  case 2:
  case 1:
    dump_tick("TEST");
    done = -1;
    break;
  }
  delay(1000);
}
/*
TEST:
  0 0.00 us     296 3.70 us     178 2.22 us     178 2.22 us     178 2.22 us     178 2.22 us     178 2.22 us     178 2.22 us
*/
