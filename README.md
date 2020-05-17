# CEnviroLib - C Enviro shield Library

This project provides simple C library for handling Enviro pHat shield on Raspberry Pi.

[Enviro pHat](https://shop.pimoroni.com/products/enviro-phat) is a Rasperry Pi compatible shield (perfectly firs Pi Zero boards) with set of sensors: thermometer, barometer, light/color sensor and motion sensor. Producer provides only [Python library](https://github.com/pimoroni/enviro-phat) so it has been decided to prepare **CEnviroLib** project to support C application development.

## Build and usage

**CEnviroLib** is prepared as a simple Makefile-based project that builds static C library. To use it you own projects just download repository and run *make*:

```bash
git clone https://github.com/markamdev/cenvirolib
make
```

In case you're downloading the project to your PC but want to use in applications launched on Raspberry Pi please define which compiler should be used. Fo example it can be ***arm-linux-gnueabihf-gcc***:

```bash
git clone https://github.com/markamdev/cenvirolib
cd cenvirolib
make CC=arm-linux-gnueabihf-gcc
```

In output directory (*./build*) you will find header file (*cenviro.h*) and library itself (*libcenviro.a*).

### Thread unsafe version

By default library is compiled in thread-safe version with mutexes used to protect critical sections. Standard *pthread* library is used for this purpose.

If for some reason it is not desired to have mutex operations enabled (ex. pthread library is not available or application is using only single thread and mutexing is a CPU time wasting) code can be compiled in thread-unsafe version by defining *DISABLE_THREADSAFE*. This definition is added in *nothreadsafe* make target:

```bash
make nothreadsafe
```

## Functionality

The goal of this project was to provide simple C library for Enviro pHat support. The intention was to make this shield easy to use - not to exhaust all possible configurations of the onboard chips. That's why only simple mode of operation is available for each of the sensors. Of course, as the full source code is available, developer can modify configuration flow to obtain desired results.

Library has 4 logical modules with one header file each:

* LED support - turning onboard white LEDs on and off (see [this chapter](#led-control))
* "Weather" support - reading envirornment temperature and atmosspheric pressure (see [this chapter](#barometer-and-thermometer))
* Light sensor support - reading lighting level and data from color sensor (see [this chapter](#light-sensor))
* Motion sensor support - reading information about orientation (compass) and acceleration (see [this chapter](#motion-sensor))
* ADC support - 4-channel analog-to-digital converter (see [this chapter](#ad-converter))

## API

To use any API described in next chapters library has to be initialized first. Initialization is done using

```c
bool cenviro_init();
```

This function returns *true* if initialization succeed, *false* otherwise. If library initialization failed then none of functions for getting/setting data can be used as it will return error or defaul value ("0" in most cases).

When library is not needed or when application finishes, it is higly recommended to call:

```c
void cenviro_deinit();
```

to properly release all initialized resources (ex. unexport GPIO pin).

### LED control

API for this module contains one function:

```C
void cenviro_led_set(bool state);
```

#### cenviro_led_set()

This function sets onboard LED to **on** or **off** state depending on provided function parameter (**true** and **false** respectively).

### Barometer and thermometer

API for this module consist of following functions:

```c
double cenviro_weather_temperature();

double cenviro_weather_pressure();

uint8_t cenviro_weather_chip_id();
```

#### cenviro_weather_temperature()

This function returns current temperature value.

#### cenviro_weather_pressure()

This function returns current pressure value.

***WARNING*** Pressure data calibration function uses value computed by temperature data calibration function. If temperature has not been fetched and computed yet then first call to *cenviro_weather_pressure()* internaly invokes temperature data fetching and calibration computation. In case of long term pressure only monitoring (when significant temperature change can take place) it is highly recommended to periodically call *cenviro_weather_temperature()*.

#### cenviro_weather_chip_id()

This function return "weather" sensor chip identifier (single byte, unsigned value).

### Light sensor

API for this module consist of following functions:

```c
cenviro_crgb_t cenviro_light_crgb_raw();

cenviro_crgb_t cenviro_light_crgb_scaled();

uint8_t cenviro_light_chip_id();

const char *cenviro_light_chip_name();
```

and one defined data type:

```c
typedef struct
{
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} cenviro_crgb_t;
```

#### cenviro_light_crgb_scaled()

This function returns *cenviro_crgb_t* structure with raw data from light sensor.

#### cenviro_light_crgb_raw()

This function returns *cenviro_crgb_t* structure with scaled RGB data (0-255) and raw C (light intensity) data.

#### cenviro_light_chip_id()

This function return light sensor chip identifier (single byte, unsigned value).

#### cenviro_light_chip_name()

This function reutrn light sensor chip name if supported by library, or *(uknown)* string otherwise.

### Motion sensor

Support for this module is **not yet implemented**.

### AD converter

Support for this module is **not yet implemented**.

## Demo and sample applications

This project provides following sample applications that show possible library use:

* cenvirodemo
  * source code in *./apps/demo*
  * shows basic usage of all implemented modules,
  * launch with *-h* to see help message
* meteo-app
  * source code in *./apps/meteo*
  * once a second, in infinite loop, reads current temperature and pressur
  * prints temperature and pressure values in top left corner of the console
* sos-blink
  * source in *./apps/sos-blink*
  * application blinks S.O.S. signal (once or infinitely)
* auto-light
  * source in *./apps/auto-light*
  * application simulates light controler - switches LED on and off based on current light intensity
  * light switching theshold can be configured by command line param
  * WARNING: onboad LEDs are detected by onboard sensor so to use this app one has to isolate sensor and LEDs

## License

Code is published under [MIT License](https://opensource.org/licenses/MIT) as it seems to be the most permissive license. If for some reason you need to have this code published with other license (ex. to reuse the code in your project) please contact [author](#author-/-contact) directly.

## Author / contact

If you need to contact me feel free to write me an email:
[markamdev.84#dontwantSPAM#gmail.com](maitlo:)
