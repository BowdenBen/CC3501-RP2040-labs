#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
#include "drivers/leds.h"
#include "drivers/board.h"

#include "drivers/lis3dh.h"

// External task functions
extern void mode_switch_task();
extern int get_mode();  // Return type for enum Mode
extern void run_led_pattern();
extern void spirit_level_task();

// Mode definitions (match enum in mode_switch.cpp)
enum Mode {
    MODE_PATTERN,
    MODE_SPIRIT_LEVEL
};

int main() {
    stdio_init_all();

    // Setup GPIO15 user switch
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    leds_init(LED_PIN, NUM_LEDS);
    LIS3DH_init();

    while (true) {
        mode_switch_task();

        switch (get_mode()) {
            case MODE_PATTERN:
                run_led_pattern();
                break;
            case MODE_SPIRIT_LEVEL:
                spirit_level_task();
                break;
        }
    }
}