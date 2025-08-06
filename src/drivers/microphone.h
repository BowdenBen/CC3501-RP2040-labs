#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


void microphone_init(void);


int microphone_read_samples(int16_t *buffer, int max_samples);


void microphone_shutdown(void);

#ifdef __cplusplus
}
#endif
