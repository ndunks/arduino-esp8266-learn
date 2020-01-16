#pragma once

const uint8_t VALVE_COUNT = 7; // 6 penyiraman, 1 pengembunan
const uint8_t VALVE_STACK_MAX = 20;

static const char *const PinSerialNames[]= {
    "IC4051_SA",
    "IC4051_SB",
    "IC4051_SC",
    "Pompa",
    "Unused_S04",
    "Unused_S05",
    "Unused_S06",
    "Unused_S07",
    "Valve_0",
    "Valve_1",
    "Valve_2",
    "Valve_3",
    "Valve_4",
    "Valve_5",
    "Sprayer",
    "Unused_S15",
};

// Serial to Pararel
#define SERIAL_DATA D7  //D7 --> DS Outputs the byte to transfer
#define SERIAL_LOAD D8  //D8 --> RCLK Controls the internal transference of data in SN74HC595 internal registers
#define SERIAL_CLOCK D5 //D5 --> SRCLK Generates the clock signal to control the transference of data

// SENSOR SUHU
#define SENSOR_SUHU_PIN D3

const struct _Pins595
{
    // Pin of first IC 74HC595
    uint8_t IC4051_SA = 0;
    uint8_t IC4051_SB = 1;
    uint8_t IC4051_SC = 2;
    uint8_t Pompa = 3;
    //uint8_t Unused_S04 = 4;
    //uint8_t Unused_S05 = 5;
    //uint8_t Unused_S06 = 6;
    //uint8_t Unused_S07 = 7;

    // Pin of second IC 74HC595, valve harus urutan
    uint8_t Valve_0 = 8;
    uint8_t Valve_1 = 9;
    uint8_t Valve_2 = 10;
    uint8_t Valve_3 = 11;
    uint8_t Valve_4 = 12;
    uint8_t Valve_5 = 13;
    uint8_t Sprayer = 14;
    //uint8_t Unused_S15 = 15;
} PinSerial;

// Analog/Digital (de)multiplexer CD4051
const struct _Pins4051
{
    uint8_t SensorTanah_0 = 0;
    uint8_t SensorTanah_1 = 1;
    uint8_t SensorTanah_2 = 2;
    uint8_t SensorTanah_3 = 3;
    uint8_t SensorTanah_4 = 4;
    uint8_t SensorTanah_5 = 5;
    // uint8_t Unused_6 = 6;
    // uint8_t Unused_7 = 7;
} PinAnalog;
