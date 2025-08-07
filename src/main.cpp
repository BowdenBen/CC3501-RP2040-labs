// File: src/main.cpp

#include <stdio.h>
#include "pico/stdlib.h"
#include "drivers/board.h"    // BUTTON_PIN, LED_PIN, NUM_LEDS
#include "drivers/leds.h"     // leds_init(), leds_set(), leds_write()

// Handlers from elsewhere in your repo:
extern void run_led_pattern(void);
extern void spirit_level_task(void);
extern void microphone_app_task(void);

typedef enum {
    MODE_PATTERN = 0,
    MODE_SPIRIT_LEVEL,
    MODE_MICROPHONE,
    MODE_COUNT
} Mode;

int main() {
    // — 1) Initialize stdio for printf()
    stdio_init_all();

    // — 2) Set up button input with pull-up
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // — 3) Initialize WS2812 (or whatever) LED driver
    leds_init(NUM_LEDS, LED_PIN);

    printf("Starting in PATTERN mode\n");

    // Mode state + debounce tracking
    Mode current_mode = MODE_PATTERN;
    absolute_time_t last_press_time = { 0 };
    bool last_button_state = false;
    const uint32_t DEBOUNCE_US = 250000;  // 250 ms

    while (true) {
        // — A) Read & debounce button (rising-edge)
        bool btn = gpio_get(BUTTON_PIN);
        if (btn && !last_button_state) {
            absolute_time_t now = get_absolute_time();
            int64_t dt = absolute_time_diff_us(last_press_time, now);
            if (dt > (int64_t)DEBOUNCE_US) {
                // advance mode, wrap around
                current_mode = (Mode)((current_mode + 1) % MODE_COUNT);
                last_press_time = now;

                // Log it
                const char *names[] = {
                    "PATTERN", "SPIRIT_LEVEL", "MICROPHONE"
                };
                printf("Mode → %s\n", names[current_mode]);
            }
        }
        last_button_state = btn;

        // — B) Dispatch to the selected mode
        switch (current_mode) {
            case MODE_PATTERN:
                run_led_pattern();
                break;
            case MODE_SPIRIT_LEVEL:
                spirit_level_task();
                break;
            case MODE_MICROPHONE:
                microphone_app_task();
                break;
            default:
                break;
        }

        // — C) Gentle pacing
        sleep_ms(10);
    }

    return 0;  // never reached
}
