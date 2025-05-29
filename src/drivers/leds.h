#pragma once
#include <stdio.h>
#include "pico/stdlib.h"

// Initialize the LED system with a given number of LEDs and a data pin
void leds_init(uint pin, uint num_leds);

void change_led(uint led_index, uint8_t red, uint8_t green, uint8_t blue);

void update_leds();

void reset_leds();