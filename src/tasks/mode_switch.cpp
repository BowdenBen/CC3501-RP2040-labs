#include "board.h"
#include "pico/stdlib.h"
#include <stdio.h>

enum Mode {
    MODE_PATTERN,
    MODE_SPIRIT_LEVEL
};

static Mode current_mode = MODE_PATTERN;

Mode get_mode() {
    return current_mode;
}

void mode_switch_task() {
    static bool last_state = false;
    bool current_state = gpio_get(BUTTON_PIN);

    if (current_state && !last_state) {
        current_mode = (current_mode == MODE_PATTERN) ? MODE_SPIRIT_LEVEL : MODE_PATTERN;
        printf("Mode changed to: %s\n", current_mode == MODE_PATTERN ? "Pattern" : "Spirit Level");
        sleep_ms(300); // debounce
    }

    last_state = current_state;
}
