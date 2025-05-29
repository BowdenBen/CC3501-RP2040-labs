#include "leds.h"
#include <stdio.h>
#include "hardware/pio.h"
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/leds.h"
 
uint32_t led_data [12];
uint32_t num_leds; 

void leds_init(uint pin, uint num_leds_board) {
num_leds = num_leds_board;
    printf("Initializing LEDs on pin %d with %d LEDs\n", pin, num_leds);
    
    
    // Initialise PIO0 to control the LED chain
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, pin, 800000, false);

}

void change_led(uint led_index, uint8_t red, uint8_t green, uint8_t blue) {
    if (led_index > num_leds) {
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