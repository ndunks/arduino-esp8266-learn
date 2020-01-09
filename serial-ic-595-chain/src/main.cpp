#include <Arduino.h>
//#define BIT16 0xffff

const int dataPin = 13;  //D7 --> DS Outputs the byte to transfer
const int loadPin = 15;  //D8 --> RCLK Controls the internal transference of data in SN74HC595 internal registers
const int clockPin = 14; //D5 --> SRCLK Generates the clock signal to control the transference of data
int ledOn = 0;
int op = 1;
char tmp[17] = {0};
void dumpBit(uint16 bytes, int value)
{
    for (int i = 0; i < 16; i++)
    {
        tmp[i] = ((bytes >> i) & B1) ? '1' : '0';
    }
    printf("%d\t: %s\n", value, tmp);
}
void setup()
{
    Serial.begin(115200);
    pinMode(dataPin, OUTPUT);
    pinMode(loadPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    delay(1000);
}
void serialWrite(uint16 bits)
{
    digitalWrite(loadPin, LOW);
    for (int i = 15; i >= 0; i--)
    {
        digitalWrite(dataPin, (bits >> i) & B1);
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
    digitalWrite(loadPin, HIGH);
}
void blinkPararel()
{
    serialWrite(B11111111 << 8);
    delay(500);
    serialWrite(B11111111);
    delay(500);
}
void blinkFirstIC()
{
    delay(500);
    serialWrite(B11111111 << 8);
    delay(500);
    serialWrite(0);
}

void blinkSecondtIC()
{
    delay(500);
    serialWrite(B11111111);
    delay(500);
    serialWrite(0);
}
void waitKeyPress()
{
    while (!Serial.available())
    {
        delay(100);
    }
    while (Serial.available())
    {
        Serial.read();
    }
}
void sequence()
{
    for (int i = 0; i < 16; i++)
    {
        serialWrite(1 << i);
        dumpBit(1 << i, i + 1);
        //Wait enter
        delay(500);
    }
}
void loop()
{
    /* blinkFirstIC();
    blinkFirstIC();
    blinkSecondtIC();
    blinkSecondtIC(); */
    sequence();
    /* serialWrite(B1);
    delay(1000);
    serialWrite(B10);
    delay(1000);
    serialWrite(B100);
    delay(1000); */
}
