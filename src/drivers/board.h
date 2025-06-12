#pragma once

// Hardware-specific configuration
#define LED_PIN       14
#define NUM_LEDS      12

// I2C configuration for Accelerometer
#define I2C_SDA_PIN  16
#define I2C_SCL_PIN  17
#define I2c_INSTANCE i2c0

// User button configuration
#define BUTTON_PIN    15

// microphone configuration
#define ADC_NUM        0
#define MIC_PIN        26
#define MIC_SAMPLE_RATE 44100
#define MIC_BUFFER_SIZE 1024
