#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/leds.h"
#include "drivers/board.h"


int main() {
    stdio_init_all();

    leds_init(LED_PIN, NUM_LEDS);

    // First light pattern
    change_led(3, 255, 0, 0);
    change_led(10, 0, 255, 0);
    print_led_buffer();  // Print the current LED buffer for debugging
    update_leds();
    sleep_ms(1000);
    set_all(0, 0, 0);  // Turn off all LEDs

    // Second light pattern
    change_led(5, 0, 255, 0);
    change_led(11, 0, 0, 255);
    print_led_buffer();  // Print the current LED buffer for debugging
    update_leds();
    sleep_ms(3000);  // Give it time to be visible
    reset_leds();
    sleep_ms(1000);             // Pause before loop

    while (true) {
        change_led(0, 0, 0, 255);  // Blue
        update_leds();
        sleep_ms(500);

        reset_leds();              // Off
        sleep_ms(500);
    }
}
