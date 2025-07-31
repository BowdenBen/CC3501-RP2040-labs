#pragma once

#include "pico/stdlib.h"

#define HDC2010_ADDR 0x40  // Default I2C address

void HDC2010_init();
bool HDC2010_read(float* temperature_C, float* humidity_percent);
