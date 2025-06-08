#include "drivers/lis3dh.h"
#include "drivers/leds.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <math.h>

void spirit_level_task() {
    int16_t x_raw, y_raw, z_raw;
    float gx, gy, gz;

    for (int i = 0; i < 10; ++i) {  // Replace or remove loop for different timing
        LIS3DH_read_raw(&x_raw, &y_raw, &z_raw);
        LIS3DH_convert_to_g(x_raw, y_raw, z_raw, &gx, &gy, &gz);

        printf("Accel: X=%.2f Y=%.2f Z=%.2f\n", gx, gy, gz);

        reset_leds();
        if (gx < -0.3f) change_led(0, 255, 0, 0);
        else if (gx > 0.3f) change_led(1, 0, 255, 0);
        if (gy < -0.3f) change_led(2, 0, 0, 255);
        else if (gy > 0.3f) change_led(3, 255, 255, 0);
        update_leds();

        sleep_ms(200);
    }
}
