#include <stdio.h>
#include "pico/stdlib.h"
#include "leds.h"
#include "drivers/logging/logging.h"
#include "drivers/board.h"
#include "microphone.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

#define SAMPLE_RATE 44100    // Desired sample rate in Hz
#define FFT_LEN 1024 // Length of the FFT


void microphone_init(void) {    
    adc_init();
    adc_gpio_init( MIC_PIN);
    adc_select_input( ADC_NUM);
    adc_fifo_get_blocking(true, true, 12, false, false); // Enable FIFO, shift 8 bits, error bit, etc


    // Set up ADC clock divider for 44.1 kHz sample rate
    uint16_t adc_clkdiv = 48000000 / SAMPLE_RATE -1; // 48 MHz system clock divided to get 44.1 kHz
    adc_set_clkdiv(adc_clkdiv);
    // Configure the ADC FIFO
    adc_fifo_setup(
        true,    // Write each completed conversion to the FIFO
        false,    // Enable DMA data request (DREQ)
        0,       // DREQ at least 1 sample in FIFO
        false,   // No error bit
        false    // No byte-shifting
    );

    printf("Microphone initialized with sample rate: 44.1 kHz\n");    
}

void __not_in_flash_func(adc_capture)(uint16_t *buf, size_t count) {
    adc_run(true);
    for (size_t i = 0; i < count; i = i + 1)
        buf[i] = adc_fifo_get_blocking();
    adc_run(false);
    adc_fifo_drain();
}


// void microphone_read(int16_t *buffer, int num_samples) { 


void microphone_shutdown(void) {
    printf("Microphone shutdown complete.\n");
}