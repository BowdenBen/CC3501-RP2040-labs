#include "leds.h"
#include <stdio.h>
#include "hardware/pio.h"
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/leds.h"
#include "drivers/board.h"
 
static uint32_t led_data [NUM_LEDS];
static uint32_t num_leds; 

void leds_init(uint pin, uint num_leds_board) {
    num_leds = num_leds_board;
    printf("Initializing LEDs on pin %d with %d LEDs\n", pin, num_leds);
    
    
    // Initialise PIO0 to control the LED chain
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, pin, 800000, false);

}

void change_led(uint led_index, uint8_t red, uint8_t green, uint8_t blue) {
    if (led_index >= num_leds) {
        printf("Error: LED index %d out of range. Only one LED is supported.\n", led_index);
        return;
    }

    // Prepare the data for the LED
    led_data[led_index] = (red << 24) | (green << 16) | (blue << 8);
    
}

void update_leds() {
    // Send the data to the PIO state machine
    for (uint i = 0; i < num_leds; i++) {
        pio_sm_put_blocking(pio0, 0, led_data[i]);
    }
    sleep_us(500);
}

void reset_leds() {
    // Reset all LEDs to off
    for (uint i = 0; i < num_leds; i++) {
        led_data[i] = 0;
    }
    update_leds();
}

void led_intensity(uint8_t intensity) {
    // Adjust the intensity of all LEDs
    for (uint i = 0; i < num_leds; i++) {
        uint32_t color = led_data[i];
        uint8_t red = (color >> 24) & 0xFF;
        uint8_t green = (color >> 16) & 0xFF;
        uint8_t blue = (color >> 8) & 0xFF;

        // Scale the color values by the intensity
        red = (red * intensity) / 255;
        green = (green * intensity) / 255;
        blue = (blue * intensity) / 255;

        led_data[i] = (red << 24) | (green << 16) | (blue << 8);
    }
    update_leds();
}


// bit shift version, lower control of intensity
// void led_intensity(uint8_t level) {
//     // Clamp level to 0–7 (3-bit control)
//     if (level > 7) level = 7;

//     // Compute shift amount: max 7 means shift by 0 (full brightness)
//     uint8_t shift = 7 - level;

//     for (uint i = 0; i < num_leds; i++) {
//         uint32_t color = led_data[i];
//         uint8_t red   = (color >> 24) & 0xFF;
//         uint8_t green = (color >> 16) & 0xFF;
//         uint8_t blue  = (color >> 8)  & 0xFF;

//         // Apply bit-shift for brightness scaling
//         red   = red >> shift;
//         green = green >> shift;
//         blue  = blue >> shift;

//         // Repack into 32-bit color (alpha byte left zero)
//         led_data[i] = (red << 24) | (green << 16) | (blue << 8);
//     }

//     update_leds();
// }


// Set all LEDs to the same RGB color
void set_all(uint8_t r, uint8_t g, uint8_t b) {
    // Loop over every LED and apply the same color
    for (uint i = 0; i < num_leds; ++i) {
        change_led(i, r, g, b);
    }
}

// Set a specific LED's color using a 0xRRGGBB hexadecimal value
void change_led_hex(uint led_index, uint32_t rgb) {
    // Ensure the index is within the valid range
    if (led_index >= num_leds) return;

    // Extract red, green, blue components from packed RGB
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8)  & 0xFF;
    uint8_t b =  rgb        & 0xFF;

    // Set the LED color using the extracted values
    change_led(led_index, r, g, b);
}

// Print the current contents of the LED data buffer for debugging
// Each LED's color will be shown as its Red, Green, and Blue component values
void print_led_buffer() {
    for (uint i = 0; i < num_leds; ++i) {
        // Fetch the packed 32-bit RGB color for this LED
        uint32_t c = led_data[i];

        // Extract individual color components using bit shifts and masks
        uint8_t red   = (c >> 24) & 0xFF;
        uint8_t green = (c >> 16) & 0xFF;
        uint8_t blue  = (c >> 8)  & 0xFF;

        // Print the LED index and its RGB values to the console
        printf("LED[%d] = R:%d G:%d B:%d\n", i, red, green, blue);
    }
}
