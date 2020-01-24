#pragma once

const uint8_t VALVE_COUNT = 7; // 6 penyiraman, 1 pengembunan

static const PROGMEM char *const PinSerialNames[] = {
    "IC4051_SA",
    "IC4051_SB",
    "IC4051_SC",
    "Pompa",
    "Unused_S04",
    "Unused_S05",
    "Unused_S06",
    "Unused_S07",
    "Meja 1",
    "Meja 2",
    "Meja 3",
    "Meja 4",
    "Meja 5",
    "Meja 6",
    "Sprayer",
    "Unused_S15",
    0};

// Serial to Pararel
#define SERIAL_DATA D7  //D7 --> DS Outputs the byte to transfer
#define SERIAL_LOAD D8  //D8 --> RCLK Controls the internal transference of data in SN74HC595 internal registers
#define SERIAL_CLOCK D5 //D5 --> SRCLK Generates the clock signal to control the transference of data

// SENSOR SUHU
#define SENSOR_SUHU_PIN D3
#define SENSOR_IR D4

enum PinSerial : uint8_t
{
    // Pin of first IC 74HC595
    /* 0 */ IC4051_SA,
    /* 1 */ IC4051_SB,
    /* 2 */ IC4051_SC,
    /* 3 */ Pompa,
    /* 4 */ Unused_S04,
    /* 5 */ Unused_S05,
    /* 6 */ Unused_S06,
    /* 7 */ Unused_S07,

    // Pin of second IC 74HC595, valve harus urutan
    /* 8 */ Valve_0,
    /* 9 */ Valve_1,
    /* 10 */ Valve_2,
    /* 11 */ Valve_3,
    /* 12 */ Valve_4,
    /* 13 */ Valve_5,
    /* 14 */ Sprayer,
    /* 15 */ Unused_S15,
};

// Analog/Digital (de)multiplexer CD4051
enum PinAnalog : uint8_t
{
    /* 0 */ SensorTanah_0 = 0,
    /* 1 */ SensorTanah_1,
    /* 2 */ SensorTanah_2,
    /* 3 */ SensorTanah_3,
    /* 4 */ SensorTanah_4,
    /* 5 */ SensorTanah_5,
    /* 6 */ Unused_6,
    /* 7 */ Unused_7,
};

#define COLOR_BLACK 30
#define COLOR_RED 31
#define COLOR_GREEN 32
#define COLOR_YELLOW 33
#define COLOR_BLUE 34
#define COLOR_MAGENTA 35
#define COLOR_CYAN 36

#ifdef SMARTGARDEN_DEBUG
#define P(fmt, ...) ::printf(fmt, ##__VA_ARGS__)

#define ONOFF(val) (String("\x1b[") +                       \
                    (val ? COLOR_GREEN : COLOR_RED) +       \
                    "m" + (val ? "ON" : "OFF") + "\x1b[0m") \
                       .c_str()
#define BLACK(str) (String("\x1b[") + COLOR_BLACK + "m" + str + "\x1b[0m").c_str()
#define RED(str) (String("\x1b[") + COLOR_RED + "m" + str + "\x1b[0m").c_str()
#define GREEN(str) (String("\x1b[") + COLOR_GREEN + "m" + str + "\x1b[0m").c_str()
#define YELLOW(str) (String("\x1b[") + COLOR_YELLOW + "m" + str + "\x1b[0m").c_str()
#define BLUE(str) (String("\x1b[") + COLOR_BLUE + "m" + str + "\x1b[0m").c_str()
#define MAGENTA(str) (String("\x1b[") + COLOR_MAGENTA + "m" + str + "\x1b[0m").c_str()
#define CYAN(str) (String("\x1b[") + COLOR_CYAN + "m" + str + "\x1b[0m").c_str()

#endif
#ifndef SMARTGARDEN_DEBUG
#define P(...)   \
    do           \
    {            \
        (void)0; \
    } while (0)

#define ONOFF(...) P()
#define BLACK(str) P()
#define RED(str) P()
#define GREEN(str) P()
#define YELLOW(str) P()
#define BLUE(str) P()
#define MAGENTA(str) P()
#define CYAN(str) P()
#endif
