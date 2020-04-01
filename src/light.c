#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "cenviro.h"
#include "internal.h"
#include "logs.h"

static bool _l_initialized = false;

bool cenviro_light_init()
{

    if (!_cenviro_initialized)
    {
        LOG("Library not initialized exiting");
        return false;
    }

    if (ioctl(_cenviro_bus_fd, I2C_SLAVE, LIGHT_ADDR) < 0)
    {
        LOG("Failed to set weather sensor address\n");
        return false;
    }

    _l_initialized = true;
    return true;
}

int32_t cenviro_light_luminance()
{
    if (!_l_initialized)
    {
        return 0;
    }
    return 0;
}

cenviro_color_t cenviro_light_color()
{
    cenviro_color_t result = {.red = 0, .green = 0, .blue = 0};
    if (!_l_initialized)
    {
        // return empty (zeroed) result
        return result;
    }

    // now result should have necessary data
    return result;
}
