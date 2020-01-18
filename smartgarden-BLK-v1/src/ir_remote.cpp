#include "ir_remote.h"

IRrecv irrecv(SENSOR_IR);
decode_results results;
Button *currentButton = nullptr;
Button codeMaps[] = {
    {RemoteButton::BTN_1, 0xA25D},
    {RemoteButton::BTN_2, 0x629D},
    {RemoteButton::BTN_3, 0xE21D},
    {RemoteButton::BTN_4, 0x22DD},
    {RemoteButton::BTN_5, 0x02FD},
    {RemoteButton::BTN_6, 0xC23D},
    {RemoteButton::BTN_7, 0xE01F},
    {RemoteButton::BTN_8, 0xA857},
    {RemoteButton::BTN_9, 0x906F},
    {RemoteButton::BTN_WILDCARD, 0x6897},
    {RemoteButton::BTN_0, 0x9867},
    {RemoteButton::BTN_HASH, 0xB04F},
    {RemoteButton::BTN_UP, 0x18E7},
    {RemoteButton::BTN_LEFT, 0x10EF},
    {RemoteButton::BTN_OK, 0x38C7},
    {RemoteButton::BTN_RIGHT, 0x5AA5},
    {RemoteButton::BTN_DOWN, 0x4AB5},
    {RemoteButton::BTN_UNKNOWN, 0},
};
void ir_remote_setup()
{
    irrecv.enableIRIn();
}

void readCode()
{
    uint8_t index = 0U;
    do
    {
        if ((results.value & codeMaps[index].code) == codeMaps[index].code)
        {
            currentButton = &codeMaps[index];
            P("Tombol %d: %s\n", currentButton->remoteButton, RemoteButtonName[currentButton->remoteButton]);
            return;
        }
    } while (codeMaps[++index].code != RemoteButton::BTN_UNKNOWN);

    P("IR Mbuh: %s %s\n", typeToString(results.decode_type).c_str(), resultToHexidecimal(&results).c_str());
    return;
}

void ir_remote_loop(){
    currentButton = nullptr;
    // TODO: Save Recent Button, display on LCD
    if (irrecv.decode(&results))
    {
        irrecv.resume(); // Receive the next value
        if (results.bits > 0)
        {
            readCode();
        }
    }
}