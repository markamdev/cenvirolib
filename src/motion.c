#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "cenviro.h"
#include "internal.h"
#include "logs.h"

#define LSM_ADDRESS_ID 0x0F
#define LSM_ADDRESS_TEMP_L 0x05
#define LSM_ADDRESS_TEMP_H 0x06

#define LSM_ADDRESS_CTRL_5 0x24

#define LSM_VALUE_ID 0x49
#define LSM_VALUE_TEMP_ENA 0x80
#define LSM_VALUE_MRES_LOW 0x00
#define LSM_VALUE_MRES_HIGH 0x60
#define LSM_VALUE_MODR_25HZ 0x0c

#define LSM_VALUE_AUTOINCREMENT 0x80

static bool _m_initialized = false;
static uint8_t _m_chip_id = 0x0;

static bool _initialize_LSM();
static int16_t _twos_complement(uint16_t input);

bool cenviro_motion_init()
{
    if (!_cenviro_initialized)
    {
        LOG("Library not initialized exiting\n");
        return false;
    }

    if (ioctl(_cenviro_bus_fd, I2C_SLAVE, MOTION_ADDR) < 0)
    {
        LOG("Failed to set motion sensor address\n");
        return false;
    }

    if (!_initialize_LSM())
    {
        LOG("Failed to initialize motion module\n");
        return false;
    }

    _m_initialized = true;
    return true;
}

double cenviro_motion_temperature()
{
    if (!_m_initialized)
    {
        // return empty (zeroed) result
        return 0.0;
    }
    CENVIRO_LOCK_MUTEX();
    if (ioctl(_cenviro_bus_fd, I2C_SLAVE, MOTION_ADDR) < 0)
    {
        LOG("Failed to set motion sensor address\n");
        CENVIRO_UNLOCK_MUTEX();
        return 0.0;
    }

    _cenviro_buffer[0] = LSM_ADDRESS_TEMP_L | LSM_VALUE_AUTOINCREMENT;
    ssize_t count = write(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to set LSM temp data request\n");
        CENVIRO_UNLOCK_MUTEX();
        return 0.0;
    }
    usleep(COMMAND_WAIT * 1000);

    count = read(_cenviro_bus_fd, _cenviro_buffer, 2);
    if (count != 2)
    {
        LOG("Failed to read LSM temp data\n");
        CENVIRO_UNLOCK_MUTEX();
        return 0.0;
    }
    uint16_t uitemp = _cenviro_buffer[1] << 8 | _cenviro_buffer[0];
    int16_t itemp = _twos_complement(uitemp);

    CENVIRO_UNLOCK_MUTEX();
    // TODO: Verify why correct value appears when divided by two?
    return (double)itemp / 2;
}

uint8_t cenviro_motion_chip_id()
{

    if (!_m_initialized)
    {
        return 0x00;
    }
    return _m_chip_id;
}

static bool _initialize_LSM()
{
    ssize_t count = 0;
    // configure chip (enable necessary option)
    _cenviro_buffer[0] = LSM_ADDRESS_CTRL_5;
    _cenviro_buffer[1] = LSM_VALUE_TEMP_ENA | LSM_VALUE_MRES_HIGH | LSM_VALUE_MODR_25HZ;

    count = write(_cenviro_bus_fd, _cenviro_buffer, 2);
    if (count != 2)
    {
        LOG("Failed to write config\n");
        return false;
    }
    usleep(COMMAND_WAIT * 1000);

    // read chip id
    _cenviro_buffer[0] = LSM_ADDRESS_ID;
    count = write(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to write command for chip id\n");
        return false;
    }
    usleep(COMMAND_WAIT * 1000);

    count = read(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to read chip id\n");
        return false;
    }
    _m_chip_id = _cenviro_buffer[0];
    if (_m_chip_id != LSM_VALUE_ID)
    {
        LOG("Invalid chip id read from device\n");
        return false;
    }
    return true;
}

static int16_t _twos_complement(uint16_t input)
{
    // computation for 12-bit value (like temperature from motion sensor)
#define TWO_MASK 0x0800
    if (input & TWO_MASK)
    {
        // compute negative value
        return (~(input & 0x0fff)) + 1;
    }
    else
    {
        return (int16_t)input;
    }
#undef TWO_MASK
}
