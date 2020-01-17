#include <stdint.h>
#include <stdio.h>

void bitOp()
{
    // 32 bit to bytes --> hex
    static uint32_t num = 0b11111111000000001010101011111111;
    uint8_t bytes[4] = {
        static_cast<uint8_t>(num >> 24),
        static_cast<uint8_t>(num >> 16),
        static_cast<uint8_t>(num >> 8 & 0xff),
        static_cast<uint8_t>(num & 0xff)};
    char ids[9] = {0};

    sprintf(ids, "%02X%02X%02X%02X", bytes[0], bytes[1], bytes[2], bytes[3]);
    printf("ID %s (%d %d %d %d)\n", ids, bytes[0], bytes[1], bytes[2], bytes[3]);
}
void pointer()
{
    int *value = nullptr;
    int realValue = 0xaaaa;
    printf("value ptr: %x %x\n", *&value, value);
    value = &realValue;
    printf("%x %x %x\n%x %x %x\n", *&value, *value, value, *&realValue, &realValue, realValue);
}
void printAllStack(int *stack, int len)
{
    int stack_index = 0;
    while (stack_index < len)
    {
        printf("%d: %d\n", stack_index, stack[stack_index]);
        stack_index++;
    }
}
/* void arrayStack()
{
    uint8_t stackLen = 5;
    int stack[stackLen] = {};
    std::fill_n(stack, stackLen, -1);
    int *stackPointer = nullptr;
    printAllStack(stack, stackLen);
} */
void constStruct()
{
    static const uint8_t S00 = 0;
    static const uint8_t S01 = 1;
    static const uint8_t S02 = 2;
    const struct PINS
    {
        uint8_t LED_1 = 0;
        uint8_t LED_2 = 12;
        uint16_t LED_3 = 12;
    } Pins;

    printf("PIN %d %d %d\n", sizeof(PINS), sizeof(Pins), Pins.LED_2);
}

char buffer[21] = {};
void globalBufferedPrint()
{
    printf("BUFF:\n");
    printf(buffer);
    printf("\n");
}
void globalBuffered()
{
    
    globalBufferedPrint();
    sprintf(buffer, "HELLO WORLD");
    globalBufferedPrint();
    sprintf(buffer, "AAA");
    globalBufferedPrint();
    sprintf(buffer, "LEN %d", sizeof(buffer));
    globalBufferedPrint();
}


struct CodeMap
{
    const char *name;
    uint16_t code;
};
extern CodeMap codeMaps[];
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

const struct _RemoteButton
{
    int8_t NONE = -1;
    int8_t BTN_1 = 0;
    int8_t BTN_2 = 1;
    int8_t BTN_3 = 2;
    int8_t BTN_4 = 3;
    int8_t BTN_5 = 4;
    int8_t BTN_6 = 5;
    int8_t BTN_7 = 6;
    int8_t BTN_8 = 7;
    int8_t BTN_9 = 8;
    int8_t BTN_WILDCARD = 9;
    int8_t BTN_0 = 10;
    int8_t BTN_HASH = 11;
    int8_t BTN_UP = 12;
    int8_t BTN_LEFT = 13;
    int8_t BTN_OK = 14;
    int8_t BTN_RIGHT = 15;
    int8_t BTN_DOWN = 16;
} RemoteButton;

void constStructAccess()
{
    int8_t code = RemoteButton.BTN_OK;
    printf("Code %d %s\n", code, codeMaps[code].name);
}

int main(int argc, char const *argv[])
{
    printf("MAIN RUN\n\n");
    constStructAccess();
    //globalBuffered();
    //constStruct();
    //arrayStack();
    return 0;
}
