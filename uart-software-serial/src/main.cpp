/**
 * UART Software Serial ESP8266 NodeMCU/WemosD1Mini
 * TX with digital output D1, RX with ANALOG input (A0).
 * 
 * TXd on USBTTL CH340g has LOW 0.1v, and HIGH 2.0v
 * impossible to use digital PIN (will always detected as low ~ undefined) without step up.
 * so I decided to use RX with Analog input instead
 */
#include <Arduino.h>
#include <gpio.h>
#include <ESP8266WiFi.h>
#include <user_interface.h>
#define TX D1
#define RX_LOW (system_adc_read() < 320)
#define RX_HIGH (system_adc_read() > 420)

volatile char *bytesToSend = 0;
const uint32_t uart_baud_rate = 9600;
/*
  80MHz (80 ticks/us)
  1 second = 1000 millisecond      (ms)
  1 millisecond = 1000 microsecond (us)
  1 microsecond = 1000 nanosecond  (ns)
*/
// in micro second
const uint8_t bitTime = 1000000 / uart_baud_rate;
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

void ICACHE_RAM_ATTR sendByte(uint8_t bits)
{
  int32_t now = esp_get_cycle_count();

  // START BIT, zero/LOW
  digitalWrite(TX, LOW);
  uint8_t bitSent = 0;

sendAgain:
  while (((uint32_t)(esp_get_cycle_count() - now)) < tickDelay)
  {
    __asm__("nop");
  }
  switch (bitSent)
  {
  case 9:
    // Idle
    bitSent = -1;
    return;
  case 8:
    // END BIT, HIGH
    digitalWrite(TX, HIGH);
    delayMicroseconds(bitTime / 2);
    break;
  default:
    digitalWrite(TX, ((bits >> bitSent) & B1));
    break;
  }
  now = esp_get_cycle_count();
  bitSent++;
  goto sendAgain;
}
void ICACHE_RAM_ATTR send()
{
  uint8_t sent = 0;
  while (*(bytesToSend + sent))
  {
    sendByte(*(bytesToSend + (sent++)));
    if (sent >= 254)
    {
      // overflow
      break;
    }
  }
  bytesToSend = 0;
}
void ICACHE_RAM_ATTR read()
{
  uint32_t now = esp_get_cycle_count();
  uint8_t value = 0;
  uint8_t bitRead = 0;
readAgain:
  // Ignore start bit
  if (bitRead > 0)
  {
    value |= (RX_HIGH ? 1 : 0) << (bitRead - 1);
  }
  while (((uint32_t)(esp_get_cycle_count() - now)) < tickDelay)
  {
    __asm__("nop");
  }
  if (bitRead++ < 8)
  {
    now = esp_get_cycle_count();
    goto readAgain;
  }
  // echo back, no Carriage return
  if (value == 13)
  {
    sendByte('\n');
  }
  else
  {
    sendByte(value);
  }
  Serial.printf("%d ", value);
}

void sendString(const char *message)
{
  while (bytesToSend)
  {
    // Waiting another jobs to done
    yield();
  }
  bytesToSend = (char *)message;
  Serial.printf("SEND: %s", bytesToSend);
  timer1_write(0);
}
void setup()
{
  // turn off ESP8266 RF
  WiFi.forceSleepBegin();
  Serial.begin(115200);
  pinMode(TX, OUTPUT);
  digitalWrite(TX, HIGH);
  delay(1000);

  Serial.printf("Baud %u, bitTime %d tickDelay %d\n", uart_baud_rate, bitTime, tickDelay);
  timer1_isr_init();
  timer1_attachInterrupt(send);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
  sendString("Hello World\nYes I'm");
  sendString("Other string\n");
}

void loop()
{
  if (RX_LOW)
  {
    read();
  }
}
