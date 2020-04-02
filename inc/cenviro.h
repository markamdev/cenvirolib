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

uint8_t cenviro_weather_chip_id();

// light module
typedef struct
{
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} cenviro_crgb_t;

cenviro_crgb_t cenviro_light_crgb_raw();

cenviro_crgb_t cenviro_light_crgb_scaled();

uint8_t cenviro_light_chip_id();

const char *cenviro_light_chip_name();

#endif // _CENVIRO_H_
