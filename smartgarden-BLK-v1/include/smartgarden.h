#pragma once
#include <Arduino.h>
#include "constant.h"

// OUTUT/LED PIN
const uint8_t VALVE_START = PinSerial::Valve_0;

const uint8_t SPRAYER_NO = PinSerial::Sprayer - VALVE_START;

// Delay for checking input sensors, call set it before smartgarden_setup()
extern uint8 SERIAL_REG[];
extern unsigned long smartgarden_delay;

// Current analogs signal state
extern uint8 ANALOG_SENSOR[];
extern uint8 HUMIDITY;
// in celcius
extern int8 TEMPERATURE;

// Setup pin and start ticker
void smartgarden_setup();
void smartgarden_loop();

// Apply REG to Serial Pararel Chained IC 595
void smartgarden_apply();
