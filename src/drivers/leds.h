#pragma once
#include <stdio.h>
#include "pico/stdlib.h"

// Initialize the LED system with a given number of LEDs and a data pin
void leds_init(uint pin, uint num_leds);

// Sets a specific LED to a color
void change_led(uint led_index, uint8_t red, uint8_t green, uint8_t blue);

// Updates the LED strip with current buffer
void update_leds(void);

// Turns off all LEDs
void reset_leds(void);

// Sets intensity [0–255] using linear scaling
void led_intensity(uint8_t intensity);

// Sets all LEDs in the strip to the same RGB color
// Parameters:
//   r - Red component (0–255)
//   g - Green component (0–255)
//   b - Blue component (0–255)
void set_all(uint8_t r, uint8_t g, uint8_t b);

// Sets the color of a specific LED using a 24-bit packed RGB value (0xRRGGBB)
// Parameters:
//   led_index - Index of the LED to change (0-based)
//   rgb       - Packed RGB value where red is in bits 23–16, green in 15–8, blue in 7–0
void change_led_hex(uint led_index, uint32_t rgb);

// Prints the RGB values of all LEDs in the buffer for debugging
void print_led_buffer();

