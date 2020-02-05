#include "ir_remote.h"
#include "display.h"
#include "config.h"
#include "ESP8266WiFi.h"

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
    pinMode(SENSOR_IR, INPUT);
    delay(500);
    irrecv.enableIRIn();
}

Button *readCode()
{
    currentButton = nullptr;
    if (!irrecv.decode(&results))
        return nullptr;

    irrecv.resume();

    if (results.bits <= 0)
        return nullptr;

    uint8_t index = 0U;
    do
    {
        if ((results.value & codeMaps[index].code) == codeMaps[index].code)
        {
            currentButton = &codeMaps[index];
            P("Tombol %d: %s\n", currentButton->remoteButton, RemoteButtonName[currentButton->remoteButton]);
            return currentButton;
        }
    } while (codeMaps[++index].code != RemoteButton::BTN_UNKNOWN);

    P("IR Mbuh: %s %s\n", typeToString(results.decode_type).c_str(), resultToHexidecimal(&results).c_str());
    return nullptr;
}
void do_confirm(const char *prompt, std::function<void(void)> callback)
{
    status(prompt);
    unsigned long until = millis() + 4000UL;
    do
    {
        if (readCode())
        {
            if (currentButton->remoteButton == RemoteButton::BTN_OK)
            {
                callback();
            }
            else
            {
                break;
            }
        }
        delay(10);
    } while (millis() < until);
    status("DIBATALKAN");
    delay(2000);
}

void do_special_command(uint8_t code)
{
    P("DO CODE %d\n", code);
    switch (code)
    {
    case RemoteButton::BTN_0: // *000 - Reset Factory
        do_confirm("Tekan OK untuk reset", &config_reset);
        break;
    case RemoteButton::BTN_1: // *111 - Restart

        do_confirm("Tekan OK utk restart", &system_restart);
        break;
    case RemoteButton::BTN_2: // *222 - Show IP
        if (WiFi.isConnected())
        {
            String info = WiFi.localIP().toString();
            info += " ~ ";
            info += WiFi.SSID();
            status(info.substring(0, 20).c_str());
        }
        else
        {
            status("WiFi Tidak Terhubung");
        }
        delay(3000);
        break;
    default:
        status("KODE %d TDK DIKENAL", code + 1);
        delay(2000);
        break;
    }
}
void handle_special_command()
{
    char buffer[5] = "    ";
    int counter = 0;
    uint8_t code = 0x0;
    unsigned long until = millis() + 3000UL;
    do
    {
        if (currentButton)
        {
            RemoteButton &tombol = currentButton->remoteButton;
            buffer[counter++] = RemoteButtonName[tombol][0];
            P("CODE %s\n", buffer);
            statusSmall(buffer);
            // don't include '*'
            if (counter > 1)
            {
                if (tombol < RemoteButton::BTN_1 || tombol > RemoteButton::BTN_0)
                {
                    P("Invalid button\n");
                    break;
                }
                code |= tombol;
                if (counter == 4)
                {
                    do_special_command(code);
                    break;
                }
            }
            until = millis() + 5000UL;
        }
        delay(10);
        readCode();
    } while (millis() < until);
    currentButton = nullptr;
    // CLear status small
    statusSmall();
}

void ir_remote_loop()
{
    if (!readCode())
        return;
    RemoteButton pressed = currentButton->remoteButton;
    int8_t forcedValve = -1;
    if (pressed == BTN_WILDCARD)
    {
        handle_special_command();
    }
    else
    {
        // Valve button 1 - 6
        if (pressed >= RemoteButton::BTN_1 && pressed <= RemoteButton::BTN_6)
        {
            forcedValve = pressed;
        }
        // TUrn on sprayer
        else if (pressed >= RemoteButton::BTN_7 && pressed <= RemoteButton::BTN_9)
        {
            forcedValve = SPRAYER_NO;
        }
        if (forcedValve >= 0)
        {
            P("Force on IR %d", forcedValve);
            valveOn(forcedValve);
        }
    }
}