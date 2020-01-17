#include "smartgarden.h"
#include "IRremoteESP8266.h"
#include "IRrecv.h"
#include "IRutils.h"

IRrecv irrecv(SENSOR_IR);
decode_results results;

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

void readCode(uint8_t *code)
{
    CodeMap *current = codeMaps;
    do
    {
        *code++;
        if ((results.value & current->code) == current->code)
        {
            P("Tombol %d: %s\n", *code, current->name);
            return;
        }
        current++;
    } while (current->code);
    *code = -1;
    P("IR Mbuh: %s %s\n", typeToString(results.decode_type).c_str(), resultToHexidecimal(&results).c_str());
}

int8_t ir_remote_read()
{
    if (!irrecv.decode(&results))
    {
        return -1;
    }
    // ignore no bits (Missed)
    uint8_t code = -1;
    if (results.bits > 0)
    {
        readCode(&code);
    }
    irrecv.resume(); // Receive the next value
    return code;
}