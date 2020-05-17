#ifndef _AL_UTILS_H_
#define _AL_UTILS_H_

#include <stdint.h>

#include <cenviro.h>

void al_add_measurement(uint16_t value);

void al_compare_with_threshold(uint16_t thr);

void al_log_state();

#endif // _AL_UTILS_H_
