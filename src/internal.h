#ifndef _CENVIRO_INTERNAL_H_
#define _CENVIRO_INTERNAL_H_

#include <stdbool.h>

// GPIO pin number for LED control
#define LED_PIN 4

// i2c device file (based on RPi Zero)
#define I2C_BUS_FILE "/dev/i2c-1"

// i2c addresses of Enviro pHat sensors
#define WEATHER_ADDR 0x77 // temperature and pressure
#define LIGHT_ADDR 0x29   // light and color
#define MOTION_ADDR 0x1d  // position and movement
#define ADC_ADDR 0x49     // analog to digital converter

// wating time between i2c commads (ex. between write() and read() )
#define COMMAND_WAIT 5

// variables shared between different library files
extern int _cenviro_bus_fd;
extern bool _cenviro_initialized;

// functions used interally only
bool cenviro_weather_init();
bool cenviro_led_init();
void cenviro_led_deinit();
bool cenviro_light_init();

#endif // _CENVIRO_INTERNAL_H_
