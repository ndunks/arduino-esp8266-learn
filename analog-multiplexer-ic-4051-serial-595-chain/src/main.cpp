/**
 * 8 Input Analog
 * 16 Output digital 
 * 
 */
#include <Arduino.h>
#include <user_interface.h>
#define dataPin 13  //D7 --> DS Outputs the byte to transfer
#define loadPin 15  //D8 --> RCLK Controls the internal transference of data in SN74HC595 internal registers
#define clockPin 14 //D5 --> SRCLK Generates the clock signal to control the transference of data
// IC595 -> IC4051
#define SA 0
#define SB 1
#define SC 2

// OUTUT/LED PIN
#define OUTSTART 8

// Serial registers
uint8 REG[16] = {LOW};

int value = 0;

void DUMP_3BIT(const char *label, int byte, int value)
{
  printf("%s\t: %c%c%c\t= %d\n", label,
         (byte & B100) ? '1' : '0',
         (byte & B010) ? '1' : '0',
         (byte & B001) ? '1' : '0',
         value);
}

void setup()
{
  Serial.begin(115200);
  pinMode(dataPin, OUTPUT);
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}
/** Apply REG value to 2 chained IC 595 */
void apply()
{
  digitalWrite(loadPin, LOW);
  for (int i = 15; i >= 0; i--)
  {
    digitalWrite(dataPin, REG[i] & B1);
    Serial.printf("%d", REG[i] & B1);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }
  digitalWrite(loadPin, HIGH);
  Serial.println();
}

void setReg(uint8 no, uint8 value)
{
  REG[no] = value;
  apply();
}

int getAnalog(int no)
{
  // Set bit switches on IC 4051
  REG[SC] = (no >> 2) & B1;
  REG[SB] = (no >> 1) & B1;
  REG[SA] = (no >> 0) & B1;
  apply();
  // Persentase
  return static_cast<int>((system_adc_read() / 4) * (100.0f / 256.0f));
}
void getAllAnalog(uint8 *values)
{
  for (int i = 0; i < 8; i++)
  {
    values[i] = getAnalog(i);
  }
}

void turnOnAllSequence()
{
  for (int i = 0; i < 16; i++)
  {
    setReg(i, HIGH);
    delay(500);
    setReg(i, LOW);
  }
}
void turnOnWhenAnalogHigh()
{
  uint8 values[8] = {LOW};
  getAllAnalog(values);

  for (int i = 0; i < 8; i++)
  {
    Serial.printf("%*d=%d ", 4, values[i], (values[i] > (uint8)50));
    REG[OUTSTART + i] = (values[i] > (uint8)50) ? 1 : 0;
  }
  Serial.println();
  apply();
}
void simpleAnalogOutput()
{
  for (int i = 0; i < 8; i++)
  {
    value = getAnalog(i);
    Serial.printf("%d: %d\n", i, value);
  }
}
void loop()
{
  Serial.println("----------------");
  //turnOnAllSequence();
  //simpleAnalogOutput();
  turnOnWhenAnalogHigh();
  delay(500);
}