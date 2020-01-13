#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

uint16_t irPin = D2; // GPIO 04
IRrecv irrecv(irPin);
decode_results results;
struct CodeMap
{
  const char *name;
  uint16 code;
};
CodeMap codeMaps[] = {
    {"1", 0xA25D},
    {"2", 0x629D},
    {"3", 0xE21D},
    {"4", 0x22DD},
    {"5", 0x02FD},
    {"6", 0xC23D},
    {"7", 0xE01F},
    {"8", 0xA857},
    {"9", 0x906F},
    {"*", 0x6897},
    {"0", 0x9867},
    {"#", 0xB04F},
    {"UP", 0x18E7},
    {"LEFT", 0x10EF},
    {"OK", 0x38C7},
    {"RIGHT", 0x5AA5},
    {"DOWN", 0x4AB5},
    {0, 0}};

void setup()
{
  Serial.begin(2000000);
  irrecv.enableIRIn(); // Start the receiver
}
bool dumpCodeMap()
{
  if (!results.bits)
    return false;

  CodeMap *current = codeMaps;
  do
  {
    if ((results.value & current->code) == current->code)
    {
      Serial.printf("Tombol: %s\n", current->name);
      return true;
    }
    current++;
  } while (current->code);

  return false;
}
void loop()
{
  if (!irrecv.decode(&results))
  {
    return;
  }
  delay(500);
  // ignore no bits (Missed)
  if (results.bits > 0 && !dumpCodeMap())
  {
    Serial.printf("Mbuh: %s %s\n", typeToString(results.decode_type).c_str(), resultToHexidecimal(&results).c_str());
  }

  irrecv.resume(); // Receive the next value
}