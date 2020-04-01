#ifndef _CENVIRO_H_
#define _CENVIRO_H_

#include <stdbool.h>
#include <stdint.h>

bool cenviro_init();

void cenviro_deinit();

// led module
void cenviro_led_set(bool state);

// weather module
double cenviro_weather_temperature();

double cenviro_weather_pressure();

int8_t cenviro_weather_chip_id();

// light module
typedef struct
{
    int red;
    int green;
    int blue;
} cenviro_color_t;

int32_t cenviro_light_luminance();

cenviro_color_t cenviro_light_color();

#endif // _CENVIRO_H_
