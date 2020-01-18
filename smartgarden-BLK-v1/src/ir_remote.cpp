#include "ir_remote.h"

IRrecv irrecv(SENSOR_IR);
decode_results results;
Button *currentButton = nullptr;
Button codeMaps[] = {
    {RemoteButton::BTN_1, 0xA25D, RemoteButtonName[RemoteButton ::BTN_1]},
    {RemoteButton::BTN_2, 0x629D, RemoteButtonName[RemoteButton ::BTN_2]},
    {RemoteButton::BTN_3, 0xE21D, RemoteButtonName[RemoteButton ::BTN_3]},
    {RemoteButton::BTN_4, 0x22DD, RemoteButtonName[RemoteButton ::BTN_4]},
    {RemoteButton::BTN_5, 0x02FD, RemoteButtonName[RemoteButton ::BTN_5]},
    {RemoteButton::BTN_6, 0xC23D, RemoteButtonName[RemoteButton ::BTN_6]},
    {RemoteButton::BTN_7, 0xE01F, RemoteButtonName[RemoteButton ::BTN_7]},
    {RemoteButton::BTN_8, 0xA857, RemoteButtonName[RemoteButton ::BTN_8]},
    {RemoteButton::BTN_9, 0x906F, RemoteButtonName[RemoteButton ::BTN_9]},
    {RemoteButton::BTN_WILDCARD, 0x6897, RemoteButtonName[RemoteButton ::BTN_WILDCARD]},
    {RemoteButton::BTN_0, 0x9867, RemoteButtonName[RemoteButton ::BTN_0]},
    {RemoteButton::BTN_HASH, 0xB04F, RemoteButtonName[RemoteButton ::BTN_HASH]},
    {RemoteButton::BTN_UP, 0x18E7, RemoteButtonName[RemoteButton ::BTN_UP]},
    {RemoteButton::BTN_LEFT, 0x10EF, RemoteButtonName[RemoteButton ::BTN_LEFT]},
    {RemoteButton::BTN_OK, 0x38C7, RemoteButtonName[RemoteButton ::BTN_OK]},
    {RemoteButton::BTN_RIGHT, 0x5AA5, RemoteButtonName[RemoteButton ::BTN_RIGHT]},
    {RemoteButton::BTN_DOWN, 0x4AB5, RemoteButtonName[RemoteButton ::BTN_DOWN]},
    {RemoteButton::NONE, 0, RemoteButtonName[RemoteButton ::NONE]},
};
void ir_remote_setup()
{
    irrecv.enableIRIn();
}

bool readCode()
{
    uint8_t index = 0U;
    do
    {
        if ((results.value & codeMaps[index].code) == codeMaps[index].code)
        {
            currentButton = &codeMaps[index];
            P("Tombol %d: %s\n", currentButton->remoteButton, currentButton->name);
            return true;
        }
    } while (codeMaps[++index].code);

    P("IR Mbuh: %s %s\n", typeToString(results.decode_type).c_str(), resultToHexidecimal(&results).c_str());
    return false;
}

void ir_remote_loop(){
    currentButton = nullptr;
    if (irrecv.decode(&results))
    {
        irrecv.resume(); // Receive the next value
        if (results.bits > 0)
        {
            readCode();
        }
    }
}