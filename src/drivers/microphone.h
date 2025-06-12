#pragma once
#include <stdio.h>
#include "pico/stdlib.h"


// Microphone initialization function
void microphone_init(void);

// Reads multiple samples from the microphone
// Returns number of samples successfully read
int microphone_read_samples(int16_t *buffer, int max_samples); 

