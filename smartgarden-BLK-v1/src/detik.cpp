#include "detik.h"
/*
 * this value will overflow/reset after 136.19251950152207 years
 * or it reset when power down :-D
 * 
 * is use firmware timer1, make sure no other library use it!
 * 
 * by: klampok.child@gmail.com
 */

/*
 * Detik sejak perangkat dinyalakan
 */
volatile uint32_t DETIK = 0;

void ICACHE_RAM_ATTR detik_tick()
{
    DETIK++;
}

void detik_setup()
{
    timer1_attachInterrupt(&detik_tick);

    /*
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
};