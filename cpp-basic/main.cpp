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

int main(int argc, char const *argv[])
{
    printf("MAIN RUN\n\n");
    globalBuffered();
    //constStruct();
    //arrayStack();
    return 0;
}
