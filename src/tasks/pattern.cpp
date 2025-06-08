#include "drivers/leds.h"
#include "pico/stdlib.h"

void run_led_pattern() {
    change_led(0, 255, 0, 0);
    update_leds();
    sleep_ms(1000);
    reset_leds();
    sleep_ms(1000);
}
