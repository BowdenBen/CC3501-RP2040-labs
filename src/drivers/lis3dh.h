#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#define WHO_AM_I 0x0f
#define LIS3DH_ADDRESS 0x19 // Default I2C address for LIS3D


// Initialize the LIS3DH accelerometer
void LIS3DH_init();

// Read data from a specific register
void read_from_register(uint8_t reg, uint8_t *data, size_t length);

// Write data to a specific register
void write_to_register(uint8_t reg, uint8_t value);

// Read raw accelerometer data
void LIS3DH_read_raw(int16_t *x, int16_t *y, int16_t *z);

// Convert raw accelerometer data to g's
void LIS3DH_convert_to_g(int16_t raw_x, int16_t raw_y, int16_t raw_z, float *gx, float *gy, float *gz);
