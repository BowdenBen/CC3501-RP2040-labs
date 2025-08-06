#include "microphone.h"
#include "drivers/board.h"    // defines MIC_PIN and ADC_NUM
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

#define SAMPLE_RATE 44100U

void microphone_init(void) {
    // 1) Init ADC hardware
    adc_init();
    // 2) Configure the GPIO pin for analog input
    adc_gpio_init(MIC_PIN);
    // 3) Select the ADC input channel
    adc_select_input(ADC_NUM);

    // 4) Compute clkdiv so that T = (1+clkdiv)/48 MHz = 1/44 100 Hz
    float clkdiv = (48000000.0f / (float)SAMPLE_RATE) - 1.0f;
    adc_set_clkdiv(clkdiv);

    // 5) FIFO setup: free-run into FIFO, threshold=1, no DMA, no shift
    adc_fifo_setup(
        /* en   */ true,   // write each conversion to FIFO
        /* dreq */ false,  // don't generate DMA request
        /* thresh */ 1,    // DREQ when >=1 sample
        /* err bit */ false,
        /* shift */ false
    );

    printf("[MIC] ADC init @ %u Hz\n", SAMPLE_RATE);
}

int microphone_read_samples(int16_t *buffer, int max_samples) {
    if (!buffer || max_samples <= 0) return 0;

    // Start continuous sampling
    adc_run(true);

    // Block until we get exactly max_samples
    for (int i = 0; i < max_samples; i++) {
        uint16_t raw = adc_fifo_get_blocking();
        buffer[i] = (int16_t)raw;
    }

    // Stop sampling and clear FIFO
    adc_run(false);
    adc_fifo_drain();

    return max_samples;
}

void microphone_shutdown(void) {
    // Nothing to do on RP2040, but log for sanity
    printf("[MIC] ADC shutdown\n");
}
