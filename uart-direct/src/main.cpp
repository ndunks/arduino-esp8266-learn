#include <Arduino.h>
#include <gpio.h>
#include <ESP8266WiFi.h>
#include <user_interface.h>
//#include <PolledTimeout.h>

#define TX D1
#define RX D2
volatile ulong lastClocked = 0;
volatile ulong now = 0;
volatile uint8_t bitPos = 0;
volatile int8_t bitSent = -1;
volatile uint8_t byteToSend = 0;
volatile uint64_t value = 0;
volatile uint speeds[64] = {};
static uint32_t uart_baud_rate = 9600;
/*
  80MHz (80 ticks/us)
  1 second = 1000 millisecond      (ms)
  1 millisecond = 1000 microsecond (us)
  1 microsecond = 1000 nanosecond  (ns)
*/
// in micro second
float_t bitTime = 1000000.0f / uart_baud_rate;
/**
 * TIM_DIV1     80MHz (80 ticks/us - 104857.588 us max)
 *              Under 1 second loop/timer
 * 
 * TIM_DIV16    5MHz (5 ticks/us - 1677721.4 us max)
 *              1 second = 5 * 1000 * 1000 = 5000000 tick
 * 
 * TIM_DIV256   312.5Khz (1 tick = 3.2us - 26843542.4 us max)
 *              1 second = 1000 * 1000 / 3.2 = 312500 tick
 * 
 * 1 tick = 1 us/80 = 0.0125us = 12.5ns
 * 1 s / 9600 * 1000 * 1000 = 104 us
 * --
 * 1 / 9600 * 1000 * 1000 / (1/5) = 520.8333333333334
 */
const uint32_t tickDelay = bitTime / (1.0f / 5.0f);

void ICACHE_RAM_ATTR uart_rx_down()
{
  now = system_get_time();
  printf("DOWN\n");
  switch (lastClocked)
  {
  case 0:
    lastClocked = now;
    return;
  default:
    speeds[bitPos++] = now - lastClocked;
    lastClocked = now;
    return;
  }
}
void sendBit()
{
  switch (bitSent)
  {
  case 10:
    // Idle
    bitSent = -1;
    return timer1_disable();
  case 9:
    // END BIT, HIGH
    return digitalWrite(TX, bitSent++ & B1);
  case 0:
    // START BIT, zero/LOW
    bitSent++;
    return digitalWrite(TX, 0);
  default:
    digitalWrite(TX, ((byteToSend >> (bitSent - 1)) & B1));
    bitSent++;
    return;
  }
}
void sendByte(uint8_t byte)
{
  while (bitSent >= 0)
  {
    Serial.printf("Wait %d\n", bitSent);
    delay(1);
  }

  byteToSend = byte;
  bitSent = 0;

  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
  timer1_write(tickDelay); //
}

int sendString(const char *message)
{
  uint16_t sent = 0;
  while (*(message + sent))
  {
    sendByte(*(message + (sent++)));
    if (sent >= 100)
    {
      // overflow
      return -1;
    }
  }
  return sent;
}
void setup()
{
  // turn off ESP8266 RF
  WiFi.forceSleepBegin();
  delay(1);
  Serial.begin(115200);
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);
  digitalWrite(TX, HIGH);
  delay(100);
  timer1_isr_init();
  timer1_attachInterrupt(sendBit);
  sendString("Hello World\n");
  sendString("Yes I'm\n");
}
void dump()
{
  for (int i = 0; i < 64; i++)
  {
    if (!speeds[i])
      break;

    Serial.printf("\n\t%d: %d", i, speeds[i]);
    Serial.printf(" = %d", (int)((value >> (63 - i)) & B1));
  }
  printf("\n");
}
void loop()
{

  if (bitPos >= 30)
  {
    if (bitPos == 127)
    {
      delay(1000);
      return;
    }
    printf("UART DISABLED\n");
    detachInterrupt(digitalPinToInterrupt(RX));
    dump();
    bitPos = 127;
  }
}
