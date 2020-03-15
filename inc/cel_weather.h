#ifndef _CEL_WEATHER_H_
#define _CEL_WEATHER_H_

#include <stdbool.h>
#include <stdint.h>

bool cel_weather_init();

double cel_weather_read_temp();

double cel_weather_read_baro();

void cel_weather_deinit();

// temporary helper functions - will be removed after full implementation
// temporary helper functions
int8_t cel_read_chip_id();

#endif // _CEL_WEATHER_H_
