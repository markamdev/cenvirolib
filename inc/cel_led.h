#ifndef _CEL_LED_H_
#define _CEL_LED_H_

#include <stdbool.h>

bool cel_led_init();

void cel_led_set(bool state);

void cel_led_deinit();

#endif // _CEL_LED_H_