#include <Arduino.h>

const int dataPin = 12;  //Outputs the byte to transfer
const int loadPin = 13;  //Controls the internal transference of data in SN74HC595 internal registers
const int clockPin = 15; //Generates the clock signal to control the transference of data
int ledOn = 0;
int op = 1;
void setup()
{
    Serial.begin(115200);
    pinMode(dataPin, OUTPUT);
    pinMode(loadPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
}

void loop()
{

    digitalWrite(loadPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 1 << ledOn);
    digitalWrite(loadPin, HIGH);
    delay(200);
    Serial.printf("%d ", ledOn);
    if (ledOn == 7 && op > 0)
        op = -1;
    else if (ledOn == 0 && op < 0)
    {
        op = 1;
    }
    ledOn += op;
}
