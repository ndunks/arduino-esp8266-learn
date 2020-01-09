
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

int ledPin = 02; // D4
/** Pin untuk remote receiver */
//int irPin = 12
uint16_t irPin = 12;

IRrecv irrecv(irPin);

decode_results results;
int ledState = LOW;
unsigned long previousMillis = 0;
unsigned long interval = 500;

void ledRuntime()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    if (ledState == LOW)
    {
      ledState = HIGH;
    }
    else
    {
      ledState = LOW;
    }

    digitalWrite(ledPin, ledState);
  }
}
void setup()
{
  pinMode(ledPin, OUTPUT);
  //pinMode(irPin, INPUT);
  Serial.begin(115200);
  digitalWrite(ledPin, LOW);
  irrecv.enableIRIn(); // Start the receiver
}

void loop()
{
  if (interval > 0)
  {
    ledRuntime();
  }

  if (!irrecv.decode(&results))
    return;
  // ignore no bits (Missed)
  if (results.bits > 0)
  {
    switch (results.value)
    {
    case 0x20DF08F7:
      Serial.println("Remote Tombol 0");
      digitalWrite(ledPin, ledState = LOW);
      interval = 0;
      break;
    case 0x20DF8877:
      Serial.println("Remote Tombol 1");
      interval = 200;
      break;
    case 0x20DF48B7:
      Serial.println("Remote Tombol 2");
      interval = 400;
      break;
    case 0x20DFC837:
      Serial.println("Remote Tombol 3");
      interval = 600;
      break;
    case 0x20DF28D7:
      Serial.println("Remote Tombol 4");
      interval = 800;
      break;
    case 0x20DFA857:
      Serial.println("Remote Tombol 5");
      interval = 1000;
      break;
    case 0x20DF6897:
      Serial.println("Remote Tombol 6");
      interval = 1200;
      break;
    case 0x20DFE817:
      Serial.println("Remote Tombol 7");
      interval = 1400;
      break;
    case 0x20DF18E7:
      Serial.println("Remote Tombol 8");
      interval = 1500;
      break;
    case 0x20DF9867:
      Serial.println("Remote Tombol 9");
      interval = 1600;
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
