#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const int dataPin = 12;  //Outputs the byte to transfer
const int loadPin = 13;  //Controls the internal transference of data in SN74HC595 internal registers
const int clockPin = 15; //Generates the clock signal to control the transference of data
uint16_t irPin = 4;
IRrecv irrecv(irPin);
decode_results results;

void ledOn(uint8 no){
    digitalWrite(loadPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 1 << no);
    digitalWrite(loadPin, HIGH);
}
void setup()
{
    Serial.begin(115200);
    // pinMode(dataPin, OUTPUT);
    // pinMode(loadPin, OUTPUT);
    // pinMode(clockPin, OUTPUT);
    irrecv.enableIRIn(); // Start the receiver

    // turnoff all led
    //ledOn(0);
}


void loop()
{
     if (!irrecv.decode(&results))
    return;
  // ignore no bits (Missed)
  if (results.bits > 0)
  {
    switch (results.value)
    {
    case 0x20DF08F7:
      Serial.println("Remote Tombol 0");
      break;
    case 0x20DF8877:
      Serial.println("Remote Tombol 1");
      break;
    case 0x20DF48B7:
      Serial.println("Remote Tombol 2");
      break;
    case 0x20DFC837:
      Serial.println("Remote Tombol 3");
      break;
    case 0x20DF28D7:
      Serial.println("Remote Tombol 4");
      break;
    case 0x20DFA857:
      Serial.println("Remote Tombol 5");
      break;
    case 0x20DF6897:
      Serial.println("Remote Tombol 6");
      break;
    case 0x20DFE817:
      Serial.println("Remote Tombol 7");
      break;
    case 0x20DF18E7:
      Serial.println("Remote Tombol 8");
      break;
    case 0x20DF9867:
      Serial.println("Remote Tombol 9");
      break;

    default:
      Serial.print("Mbuh: ");
      serialPrintUint64(results.value, 16);
      Serial.println(resultToSourceCode(&results));
      break;
    }
  }
  irrecv.resume(); // Receive the next value
}
