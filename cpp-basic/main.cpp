#include <iostream>
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
void arrayStack()
{
    uint stackLen = 5;
    int stack[stackLen] = {};
    std::fill_n(stack, stackLen, -1);
    int *stackPointer = nullptr;
    printAllStack(stack, stackLen);
}
int main(int argc, char const *argv[])
{
    printf("MAIN RUN\n\n");
    arrayStack();
    return 0;
}
