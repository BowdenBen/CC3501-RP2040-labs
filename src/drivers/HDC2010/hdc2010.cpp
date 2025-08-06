#include "hdc2010.h"
#include "drivers/board.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include <stdint.h>   

#define TEMP_LOW   0x00
#define HUMID_LOW  0x02
#define MEAS_CONF  0x0F
#define CONFIG     0x0E

void HDC2010_init() {
    i2c_init(I2c_INSTANCE, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    sleep_ms(15);  // Power-up delay

    // Set measurement config: both temperature and humidity, no trigger required
    uint8_t config = 0x00;  // Auto mode, no interrupt, 14-bit
    uint8_t buf[] = { CONFIG, config };
    i2c_write_blocking(I2c_INSTANCE, HDC2010_ADDR, buf, 2, false);

    // Trigger measurement
    uint8_t trigger[] = { MEAS_CONF, 0x01 };
    i2c_write_blocking(I2c_INSTANCE, HDC2010_ADDR, trigger, 2, false);
    sleep_ms(20);  // Wait for measurement to complete
}

bool HDC2010_read(float* temperature_C, float* humidity_percent) {
    uint8_t reg = TEMP_LOW;
    uint8_t data[4];

    // Read temperature and humidity (2 bytes each)
    if (i2c_write_blocking(I2c_INSTANCE, HDC2010_ADDR, &reg, 1, true) != 1)
        return false;
    if (i2c_read_blocking(I2c_INSTANCE, HDC2010_ADDR, data, 4, false) != 4)
        return false;

    uint16_t temp_raw = (data[1] << 8) | data[0];
    uint16_t humid_raw = (data[3] << 8) | data[2];

    *temperature_C = (temp_raw / 65536.0f) * 165.0f - 40.0f;
    *humidity_percent = (humid_raw / 65536.0f) * 100.0f;

    return true;
}
