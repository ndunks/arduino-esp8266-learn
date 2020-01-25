#include <c_types.h>
#include "Arduino.h"

/**
 * this value will overflow/reset after 136.19251950152207 years
 * or it reset when power down :-D
 */
volatile uint32_t UPTIME = 0;

void uptime_setup()
{
  timer1_isr_init();
  timer1_attachInterrupt([]() ICACHE_RAM_ATTR {
    UPTIME++;
  });
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
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
  timer1_write(312500); //1 second
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.printf("timer1_enabled: %d\n", timer1_enabled());
  uptime_setup();
  Serial.println();
  Serial.print("UPTIME USING TIMER START\n");
  Serial.printf("timer1_enabled: %d\n", timer1_enabled());
}

void loop()
{
  Serial.printf("%u", UPTIME);
  delay(100);
  Serial.printf("\r");
}
