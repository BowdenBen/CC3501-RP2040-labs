#include <stdio.h>
#include "pico/stdlib.h"
#include "drivers/logging/logging.h"
#include "drivers/board.h"
#include "lis3dh.h"
#include "hardware/i2c.h"



void LIS3DH_init() {
    i2c_init(I2c_INSTANCE, 400 * 1000); // Initialize I2C at 400 kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    // Check WHO_AM_I register (should be 0x33 for LIS3DH)
    uint8_t who_am_i = 0;
    read_from_register(WHO_AM_I, &who_am_i, 1);
    printf("Who Am I %u\n", who_am_i);

    write_to_register(0x20, 0x77);
    printf("LIS3DH initialized, WHO_AM_I=0x%02X\n", who_am_i);
    
    // CTRL_REG1: Enable X/Y/Z, 100 Hz data rate, normal mode
    // ODR = 400 Hz => 0111 << 7 => 0x70
    // Enable XYZ (bits 0–2 = 1) => 0x07
    write_to_register(0x20, 0x77); // 0x40 | 0x07 = 0x47


    printf("LIS3DH configuration complete.\n");
}

void read_from_register(uint8_t reg, uint8_t *data, size_t length) {

    // checvk iof reasding multiple bytes
    if (length > 1) {
        reg |= 0x80; // Set auto-increment bit for multiple byte reads
    }
    i2c_write_blocking(I2c_INSTANCE, LIS3DH_ADDRESS, &reg, 1, false);
    i2c_read_blocking(I2c_INSTANCE, LIS3DH_ADDRESS, data, length, false);
}

void write_to_register(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    i2c_write_blocking(I2c_INSTANCE, LIS3DH_ADDRESS, buffer, 2, false);
}

void LIS3DH_read_raw(int16_t* x, int16_t* y, int16_t* z) {
    uint8_t buffer[6];
    read_from_register(0x28, buffer, 6);  // auto-increment

    *x = (int16_t)((buffer[1] << 8) | buffer[0]) >> 4;
    *y = (int16_t)((buffer[3] << 8) | buffer[2]) >> 4;
    *z = (int16_t)((buffer[5] << 8) | buffer[4]) >> 4;
}

void LIS3DH_convert_to_g(int16_t x_raw, int16_t y_raw, int16_t z_raw, float* gx, float* gy, float* gz) {
    const float sensitivity = 0.001f;  // 1mg/LSB for ±2g
    *gx = x_raw * sensitivity;
    *gy = y_raw * sensitivity;
    *gz = z_raw * sensitivity;
}

void LIS3DH_output_raw_uart() {
    int16_t x, y, z;
    LIS3DH_read_raw(&x, &y, &z);

    char buffer[64];
    sprintf(buffer, "X:%d Y:%d Z:%d\n\r", x, y, z);
    uart_puts(uart0, buffer);
}
