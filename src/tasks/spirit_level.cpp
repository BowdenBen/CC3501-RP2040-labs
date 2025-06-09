#include "drivers/lis3dh.h"
#include "drivers/leds.h"
#include "board.h"
#include "pico/stdlib.h"
#include <math.h>

#define MAX_TILT_G 1.5f
#define INTENSITY_SCALE 255.0f

// Predefined unit direction vectors for each LED (in U shape)
static float LED_VECTORS[12][2] = {
    { 1,  1},  // LED 0: used to be up-left → now right-up
    { 1,  0},  // LED 1: up → now right
    { 1, -1},  // LED 2: up-right → now right-down
    { 1, -1},  // LED 3: same as 2
    { 0, -1},  // LED 4: right → now down
    { 0, -1},  // LED 5: right → now down
    { 0, -1},  // LED 6: right → now down
    { 0, -1},  // LED 7: right → now down
    {-1, -1},  // LED 8: bottom-right → now left-down
    {-1,  0},  // LED 9: bottom → now left
    {-1,  1},  // LED 10: bottom-left → now left-up
    {-1,  1}   // LED 11: same as 10
};


// Normalize LED vectors to unit length
static void normalize_vectors() {
    static bool normalized = false;
    if (normalized) return;
    for (int i = 0; i < 12; ++i) {
        float x = LED_VECTORS[i][0];
        float y = LED_VECTORS[i][1];
        float mag = sqrtf(x * x + y * y);
        if (mag > 0.01f) {
            LED_VECTORS[i][0] /= mag;
            LED_VECTORS[i][1] /= mag;
        }
    }
    normalized = true;
}

// Converts tilt angle to LED brightness
uint8_t vector_to_brightness(float gx, float gy, float dx, float dy) {
    float tilt_mag = sqrtf(gx * gx + gy * gy);
    if (tilt_mag < 0.01f) return INTENSITY_SCALE;  // perfectly level

    float dot = (gx * dx + gy * dy) / tilt_mag;  // cos(angle) between gravity and LED
    dot = fmaxf(0.0f, dot);                      // only light "downward" LEDs
    return (uint8_t)(dot * INTENSITY_SCALE);
}

void spirit_level_task() {
    int16_t x_raw, y_raw, z_raw;
    float gx, gy, gz;

    normalize_vectors();  // run once

    LIS3DH_read_raw(&x_raw, &y_raw, &z_raw);
    LIS3DH_convert_to_g(x_raw, y_raw, z_raw, &gx, &gy, &gz);

    float tilt_mag = sqrtf(gx * gx + gy * gy); // To calculate the length (or "tilt strength") of that 2D vector, use the Pythagorean theorem

    if (tilt_mag < 0.05f) {  // ≈ Level (within ±2.8° tilt),θ=sin^(-1)(0.05)≈2.87°
        set_all(0, 255, 0);   // Bright green
        update_leds();
        sleep_ms(200);        // Stay on for visual feedback
        return;
    }

    reset_leds();

    for (int i = 0; i < 12; ++i) {
        float dx = LED_VECTORS[i][0];
        float dy = LED_VECTORS[i][1];
        uint8_t brightness = vector_to_brightness(gx, gy, dx, dy);
        change_led(i, brightness, brightness, brightness);
    }

    update_leds();
    sleep_ms(100); // Update rate for spirit level task, 50ms = very responsive, 100ms = smoother but less responsive.
}
