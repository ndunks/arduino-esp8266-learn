#pragma once
#include "IRremoteESP8266.h"
#include "IRrecv.h"
#include "IRutils.h"
#include "constant.h"

struct CodeMap
{
    const char *name;
    uint16 code;
};
/*********************************
 *  ir_remote.cpp
 *********************************/
void ir_remote_setup();
extern int8_t currentButton;
bool ir_remote_read();