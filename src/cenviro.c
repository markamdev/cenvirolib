#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>

#include "cenviro.h"

#include "internal.h"
#include "logs.h"

int _cenviro_bus_fd = 0;
bool _cenviro_initialized = false;

bool cenviro_init()
{
    int bus_file = 0;
    bool status = false;

    status = cenviro_led_init();
    if (!status)
    {
        LOG("Failed to init led module\n");
        return false;
    }

    bus_file = open(I2C_BUS_FILE, O_RDWR);
    if (bus_file < 0)
    {
        LOG("Failed to open i2c bus\n");
        goto err_led;
    }

    _cenviro_bus_fd = bus_file;
    _cenviro_initialized = true;

    status = false;
    status = cenviro_weather_init();

    if (!status)
    {
        LOG("Failed to init weather module\n");
        goto err_i2c;
    }

    status = cenviro_light_init();
    if (!status)
    {
        LOG("Failed to init light module\n");
        goto err_i2c;
    }

    return true;

err_i2c:
    close(_cenviro_bus_fd);
err_led:
    cenviro_led_deinit();
    _cenviro_initialized = false;
    return false;
}

void cenviro_deinit()
{
    if (!_cenviro_initialized)
    {
        return;
    }

    _cenviro_initialized = false;
    cenviro_led_deinit();

    if (_cenviro_bus_fd != 0)
    {
        close(_cenviro_bus_fd);
        _cenviro_bus_fd = 0;
    }
}
