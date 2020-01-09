#include <iostream>

int main(int argc, char const *argv[])
{
    // 32 bit to bytes --> hex
    static uint32_t num = 0b11111111000000001010101011111111;
    uint8_t bytes[4] = {num >> 24, num >> 16, num >> 8 & 0xff, num & 0xff};
    char ids[9] = {0};

    sprintf(ids, "%02X%02X%02X%02X", bytes[0], bytes[1], bytes[2], bytes[3]);
    printf("ID %s (%d %d %d %d)\n", ids, bytes[0], bytes[1], bytes[2], bytes[3]);

    return 0;
}
