#pragma once
#include <Arduino.h>
#include "constant.h"

// OUTUT/LED PIN
const uint8_t VALVE_START = PinSerial.Valve_0;

const uint8_t SPRAYER_NO = PinSerial.Sprayer - VALVE_START;
