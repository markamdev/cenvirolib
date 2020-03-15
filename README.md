# CEnviroLib - C library for Enviro pHat shield

This project provides simple C library for handling Enviro pHat shield on Raspberry Pi.

[Enviro pHat](https://shop.pimoroni.com/products/enviro-phat) is a Rasperry Pi compatible shield (perfectly firs Pi Zero boards) with set of sensors: thermometer, barometer, light/color sensor and motion sensor. Producer provides only [Python library](https://github.com/pimoroni/enviro-phat) so it has been decided to prepare **CEnviroLib** project to support C application development.

## Build and usage

**CEnviroLib** is prepared as a simple Makefile-based project that builds static C library. To use it you own projects just download repository and run *make*:

```bash
git clone https://github.com/markamdev/cenvirolib
make
```

In case you're downloading the project to your PC but want to use in applications launched on Raspberry Pi please define which compiler should be used. In my case it's *arm-linux-gnueabihf-gcc*:

```bash
git clone https://github.com/markamdev/cenvirolib
make CC=arm-linux-gnueabihf-gcc
```

In output directory (*./build*) you will find necessary header files (*cel_\*.h*) and library itself (*cenvirolib.a*).

## Functionality

## API

### LED control

### Barometer and thermometer

### Motion sensor

## License

Code is published under [MIT License](https://opensource.org/licenses/MIT) as it seems to be the most permissive license. If for some reason you need to have this code published with other license (ex. to reuse the code in your project) please contact [author](#author-/-contact) directly.

## Author / contact

If you need to contact me feel free to write me an email:
[markamdev.84#dontwantSPAM#gmail.com](maitlo:)