#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "cenviro.h"
#include "internal.h"
#include "logs.h"

// control bit masks
#define TCS_COMMAND 0x80
#define TCS_AUTOINCREMENT 0x20

// addresses of used TCS3472 registers
#define TCS_ADDRESS_ENABLE 0x00
#define TCS_ADDRESS_ID 0x12

// clear, red, green, blue data addesses
#define TCS_ADDRESS_CLEAR_L 0x14
#define TCS_ADDRESS_CLEAR_H 0x15
#define TCS_ADDRESS_RED_L 0x14
#define TCS_ADDRESS_RED_H 0x15
#define TCS_ADDRESS_GREEN_L 0x14
#define TCS_ADDRESS_GREEN_H 0x15
#define TCS_ADDRESS_BLUE_L 0x14
#define TCS_ADDRESS_BLUE_H 0x15

static bool _l_initialized = false;
static uint8_t _l_chip_id = 0x0;

// internal functions forward declaration
static bool _initiaze_TCS();

bool cenviro_light_init()
{

    if (!_cenviro_initialized)
    {
        LOG("Library not initialized exiting\n");
        return false;
    }

    if (ioctl(_cenviro_bus_fd, I2C_SLAVE, LIGHT_ADDR) < 0)
    {
        LOG("Failed to set light sensor address\n");
        return false;
    }

    if (!_initiaze_TCS())
    {
        LOG("Failed to initialize ligh module\n");
        return false;
    }

    _l_initialized = true;
    return true;
}

cenviro_crgb_t cenviro_light_crgb_raw()
{
    cenviro_crgb_t result = {.clear = 0, .red = 0, .green = 0, .blue = 0};
    if (!_l_initialized)
    {
        // return empty (zeroed) result
        return result;
    }
    if (ioctl(_cenviro_bus_fd, I2C_SLAVE, LIGHT_ADDR) < 0)
    {
        LOG("Failed to set light sensor address\n");
        return result;
    }

    uint8_t buffer[8] = {0};
    ssize_t count = 0;

    // set command
    buffer[0] = TCS_COMMAND | TCS_AUTOINCREMENT | TCS_ADDRESS_CLEAR_L;

    // send config
    count = write(_cenviro_bus_fd, buffer, 1);
    if (count != 1)
    {
        LOG("Failed to crgb data request\n");
        return result;
    }
    usleep(COMMAND_WAIT * 1000);

    count = read(_cenviro_bus_fd, buffer, 8);
    if (count != 8)
    {
        LOG("Failed to read crgb data\n");
        return result;
    }

    result.clear = ((uint16_t)buffer[1]) << 8 | buffer[0];
    result.red = ((uint16_t)buffer[3]) << 8 | buffer[2];
    result.green = ((uint16_t)buffer[5]) << 8 | buffer[4];
    result.blue = ((uint16_t)buffer[7]) << 8 | buffer[6];

    // now result should have necessary data
    return result;
}

cenviro_crgb_t cenviro_light_crgb_scaled()
{
    cenviro_crgb_t result = cenviro_light_crgb_raw();
    result.red = (result.red * 255) / result.clear;
    result.green = (result.green * 255) / result.clear;
    result.blue = (result.blue * 255) / result.clear;

    return result;
}

uint8_t cenviro_light_chip_id()
{
    if (!_l_initialized)
    {
        return 0x00;
    }
    return _l_chip_id;
}

const char *cenviro_light_chip_name()
{
    switch (_l_chip_id)
    {
    case 0x44:
        return "TCS34725";
        break;
    case 0x4d:
        return "TCS34727";
        break;
    default:
        return "(uknown)";
    }
}

bool _initiaze_TCS()
{
#define ENABLE_DATA 0x03
    uint8_t buffer[3] = {0};

    buffer[0] = TCS_COMMAND | TCS_ADDRESS_ENABLE;
    buffer[1] = ENABLE_DATA;

    ssize_t count = 0;

    // send config
    count = write(_cenviro_bus_fd, buffer, 2);
    if (count != 2)
    {
        LOG("Failed to write config\n");
        return false;
    }
    usleep(COMMAND_WAIT * 1000);

    buffer[0] = TCS_COMMAND | TCS_ADDRESS_ID;
    count = write(_cenviro_bus_fd, buffer, 1);
    if (count != 1)
    {
        LOG("Failed to write command for chip id\n");
        return false;
    }
    usleep(COMMAND_WAIT * 1000);

    count = read(_cenviro_bus_fd, buffer, 1);
    if (count != 1)
    {
        LOG("Failed to read chip id\n");
        return false;
    }
    _l_chip_id = buffer[0];

    return true;
#undef ENABLE_DATA
}
