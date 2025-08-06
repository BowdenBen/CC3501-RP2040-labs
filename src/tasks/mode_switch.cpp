#include "drivers/board.h"
#include "pico/stdlib.h"
#include <stdio.h>

enum Mode {
    MODE_PATTERN,
    MODE_SPIRIT_LEVEL
};

static Mode current_mode = MODE_PATTERN;
static absolute_time_t last_press_time;

Mode get_mode() {
    return current_mode;
}

void mode_switch_task() {
    static bool last_state = false;
    bool current_state = gpio_get(BUTTON_PIN);

    // Check for rising edge (button press)
    if (current_state && !last_state) {
        absolute_time_t now = get_absolute_time();
        if (absolute_time_diff_us(last_press_time, now) > 250000) {  // 250 ms debounce
            last_press_time = now;
            current_mode = (current_mode == MODE_PATTERN) ? MODE_SPIRIT_LEVEL : MODE_PATTERN;
            printf("Mode changed to: %s\n", current_mode == MODE_PATTERN ? "Pattern" : "Spirit Level");
        }
    }

    last_state = current_state;
}
