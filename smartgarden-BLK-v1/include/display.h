#pragma once
#include "header.h"
#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

void status(const char *txt, ...);
void statusSmall(const char * txt = nullptr);

void display_setup();
void display_loop();
