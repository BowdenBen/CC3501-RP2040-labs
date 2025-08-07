#include "drivers/microphone.h"
#include "drivers/leds.h"       // leds_set()/leds_write()
#include "arm_math.h"           // CMSIS-DSP
#include "hann_window.h"        // your 1024-point Q15 Hanning window

#define FFT_LEN     1024
#define SHIFT_BITS   5        // adjust 3–5 to taste (Q15 shift = 15–12 = 3)
#define THRESHOLD 100000U     // energy threshold per band

// Log-spacing as per matlab: ceil(logspace(log10(5), log10(512), 13))
static const uint16_t bin_edges[13] = {6, 8, 11, 16, 24, 35, 51, 75, 110, 161, 237, 349, 513};

static q15_t fft_buffer[FFT_LEN];
static q15_t fft_output[FFT_LEN];
static arm_rfft_instance_q15 fft_inst;

void microphone_app_task(void) {
    static bool inited = false;
    if (!inited) {
        // Step 3.1: one-time init
        microphone_init();
        arm_rfft_init_q15(&fft_inst, FFT_LEN, /*ifft=*/0, /*bit_reverse=*/1);
        inited = true;
    }

    // 3.1: Read raw ADC
    uint16_t raw_adc[FFT_LEN];
    microphone_read_samples((int16_t*)raw_adc, FFT_LEN);

    // 3.2: Compute DC bias
    int32_t sum = 0;
    for (int i = 0; i < FFT_LEN; i++) sum += raw_adc[i];
    int16_t bias = sum / FFT_LEN;

    // 3.3 + 3.4: Build Q15 signal and apply window
    for (int i = 0; i < FFT_LEN; i++) {
        // subtract bias, shift up to Q15
        int32_t v = ((int32_t)raw_adc[i] - bias) << SHIFT_BITS;
        // apply window: (v * hann_window[i]) >> 15
        fft_buffer[i] = (q15_t)((v * hann_window[i]) >> 15);
    }

    // 3.5: FFT
    arm_rfft_q15(&fft_inst, fft_buffer, fft_output);

    // 3.6: Magnitude-squared → energy spectrum
    arm_cmplx_mag_squared_q15(fft_output, fft_output, FFT_LEN);

    // 3.7: Map log-spaced bins → LEDs
    for (int led = 0; led < NUM_LEDS; led++) {
        uint32_t energy = 0;
        for (int j = bin_edges[led]; j < bin_edges[led+1]; j++) {
            energy += fft_output[j];
        }
        leds_set(led, energy > THRESHOLD);
    }
    leds_write();
}
