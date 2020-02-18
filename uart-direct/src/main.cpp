#include <Arduino.h>
#include <gpio.h>
#include <ESP8266WiFi.h>
//#include <Esp.h>
#include <user_interface.h>
//#include <PolledTimeout.h>

#define TX D1
#define RX D2
volatile ulong lastClocked = 0;
volatile uint8_t bitPos = 0;
volatile int8_t bitSent = -1;
volatile uint8_t byteToSend = 0;
volatile uint64_t value = 0;
volatile uint speeds[64] = {};
volatile int8_t bitRead = -1;
volatile uint8_t byteRead = 0;

static uint32_t uart_baud_rate = 9600;
/*
  80MHz (80 ticks/us)
  1 second = 1000 millisecond      (ms)
  1 millisecond = 1000 microsecond (us)
  1 microsecond = 1000 nanosecond  (ns)
*/
// in micro second
uint8_t bitTime = 1000000 / uart_baud_rate;
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
const uint32_t tickDelay = bitTime / (1.0f / 80.0f);

void ICACHE_RAM_ATTR uart_rx_down()
{
  uint32_t now = esp_get_cycle_count();
  if (bitRead >= 0)
  {
    Serial.printf("UART DOWN BUT STILL READING");
    return;
  }
  ETS_GPIO_INTR_DISABLE();
  uint16_t gaps[10] = {};
  uint8_t bits[10] = {};
  bitRead = 0;
readAgain:
  bits[bitRead] = digitalRead(RX);
  gaps[bitRead] = now;
  while (((uint32_t)(esp_get_cycle_count() - now)) < tickDelay)
  {
    __asm__("nop");
  }
  if (bitRead++ < 8)
  {
    now = esp_get_cycle_count();
    goto readAgain;
  }
  ETS_GPIO_INTR_ENABLE();
  /// DUMP
  Serial.printf("\nREAD %d:\n  ", bitRead);
  uint32_t gap = 0;
  float_t gap_us = 0.0f;
  for (int i = 0; i < bitRead; i++)
  {
    if (i > 0)
    {
      gap = gaps[i] - gaps[i - 1];
      gap_us = gap / 80.0f;
    }
    Serial.printf("%2d %3u %0.2f us\n", bits[i], gap, gap_us);
  }
  Serial.printf("\n");
}
void sendBit()
{
  int32_t now = esp_get_cycle_count();
  // START BIT, zero/LOW
  digitalWrite(TX, LOW);
  bitSent++;
sendAgain:
  while (((uint32_t)(esp_get_cycle_count() - now)) < tickDelay)
  {
    __asm__("nop");
  }
  switch (bitSent)
  {
  case 10:
    // Idle
    bitSent = -1;
    return;
  case 9:
    // END BIT, HIGH
    digitalWrite(TX, HIGH);
    delayMicroseconds(bitTime / 3);
    break;
  case 0:

  default:
    digitalWrite(TX, ((byteToSend >> (bitSent - 1)) & B1));
    break;
  }
  now = esp_get_cycle_count();
  bitSent++;
  goto sendAgain;
}
void sendByte(uint8_t byte)
{
  while (bitSent >= 0)
  {
    __asm__("nop");
  }
  byteToSend = byte;
  bitSent = 0;
  timer1_write(0);
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
  delay(1000);
  Serial.printf("bitTime %d tickDelay %d\n", bitTime, tickDelay);
  //attachInterrupt(digitalPinToInterrupt(RX), uart_rx_down, FALLING);
  timer1_isr_init();
  timer1_attachInterrupt(sendBit);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
  sendString("Hello World\nYes I'm\n");
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
    printf("UART DISABLED %u\n", tickDelay);
    detachInterrupt(digitalPinToInterrupt(RX));
    dump();
    bitPos = 127;
  }
}
