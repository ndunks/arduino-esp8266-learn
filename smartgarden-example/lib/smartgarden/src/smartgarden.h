#pragma once
#include <Arduino.h>
#include <user_interface.h>

/**
 * 8 Input Analog/Digital
 * 16 Output digital 
 * 
 * OUT IC 595 (2 chain) Serial output mapping:
 *   ------------- FIRST IC 595
 * 0 - 2    To control input multiplexer IC 4051 (SA - SC)
 * 3        Pompa DC
 * 4 - 6    UNUSED
 *   ------------- SECOND IC 595
 * 8 - 14   Control Solenoid Valve/Led (see: OUTSTART)
 * 15       Solenoid pengembunan
 * 
 * IN IC 4051 Analog/Digital mapping:
 * 0 - 5    Sensor tanah (analog)
 * 6        UNUSED
 * 7        DHT22 (digital)
 */

#define SERIAL_DATA 13  //D7 --> DS Outputs the byte to transfer
#define SERIAL_LOAD 15  //D8 --> RCLK Controls the internal transference of data in SN74HC595 internal registers
#define SERIAL_CLOCK 14 //D5 --> SRCLK Generates the clock signal to control the transference of data

// OUTUT/LED PIN
#define VALVE_START 8
#define VALVE_COUNT 6
#define POMPA_NO 3
#define SPRAYER_NO 7
// GPIO
#define SENSOR_SUHU_PIN 4 // D2

// IC595 -> IC4051
#define SA 0
#define SB 1
#define SC 2

// Delay for checking input sensors, call set it before smartgarden_setup()
extern uint8 REG[];
extern unsigned long smartgarden_delay;

// Current analogs signal state
extern uint8 ANALOG_SENSOR[];
extern float HUMIDITY;
extern float TEMPERATURE;

// Setup pin and start ticker
void smartgarden_setup();
void smartgarden_loop();

// Apply REG to Serial Pararel Chained IC 595
void smartgarden_apply();

typedef enum
{
    OFF = LOW,
    ON = HIGH
} STATUS_t;

// Activate analog selector
void smartgarden_set_analog(int no);
int smartgarden_read_analog(int no);

/*  sensorsuhu.h */
typedef enum
{
    DHT11,
    DHT22
} DHT_MODEL_t;

typedef enum
{
    ERROR_NONE = 0,
    ERROR_TIMEOUT,
    ERROR_CHECKSUM
} DHT_ERROR_t;

extern DHT_MODEL_t sensorsuhu_model;
extern DHT_ERROR_t sensorsuhu_error;
void sensorsuhu_read();
