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
make CC=arm-linux-gnueabihf-gcc
```

In output directory (*./build*) you will find necessary header files (*cel_\*.h*) and library itself (*libcenviro.a*).

## Functionality

The goal of this project was to provide simple C library for Enviro pHat support. The intention was to make this shield easy to use - not to exhaust all possible configurations of the onboard chips. That's why only simple mode of operation is available for each of the sensors. Of course, as the full source code is available, developer can modify configuration flow to obtain desired results.

Library has 4 logical modules with one header file each:

* LED support - turning onboard white LEDs on and off (see [this chapter](#led-control))
* "Weather" support - reading envirornment temperature and atmosspheric pressure (see [this chapter](#barometer-and-thermometer))
* Light sensor support - reading lighting level and data from color sensor (see [this chapter](#light-sensor))
* Motion sensor support - reading information about orientation (compass) and acceleration (see [this chapter](#motion-sensor))

## API

### LED control

Available functions are declared in *cel_leds.h*.

### Barometer and thermometer

Available functions are declared in *cel_weather.h*.

### Light sensor

This module is *not yet implemented*.

### Motion sensor

This module is *not yet implemented*.

## Demo and sample applications

This project provides some sample applications that uses selected CEnviroLib features:

## License

Code is published under [MIT License](https://opensource.org/licenses/MIT) as it seems to be the most permissive license. If for some reason you need to have this code published with other license (ex. to reuse the code in your project) please contact [author](#author-/-contact) directly.

## Author / contact

If you need to contact me feel free to write me an email:
[markamdev.84#dontwantSPAM#gmail.com](maitlo:)
