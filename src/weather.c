#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "cenviro.h"
#include "internal.h"
#include "logs.h"

// Address of used BMP registers
#define BMP_ADDRESS_ID 0xd0
#define BMP_ADDRESS_RESET 0xe0             // only 0xb6 has effect
#define BMP_ADDRRESS_CONTROL 0xf4          // temp_measure | press measure | power mode
#define BMP_ADDRESS_CALIBRATION_TEMP 0x88  // lowest address of 6B temperature calibration data
#define BMP_ADDRESS_CALIBRATION_PRESS 0x8e // lowest address of 6B pressure calibration data
#define BMP_ADDRESS_RAW_TEMP 0xfa          // lowest address of 3B temperature data
#define BMP_ADDRESS_RAW_PRESS 0xf7         // lowest address of 3B temperature data

static bool _w_initialized = false;

// forward declaration of internal functions
static bool _initialize_BMP();
static bool _read_BMP_calibration_data();
static int32_t _calibrate_temperature(int32_t adc_T);
static int32_t _calibrate_pressure(int32_t adc_P);

// temperature callibration
static uint16_t _calibration_T1 = 0;
static int16_t _calibration_T2 = 0;
static int16_t _calibration_T3 = 0;
// pressure calibration
static uint16_t _calibration_P1 = 0;
static int16_t _calibration_P2 = 0;
static int16_t _calibration_P3 = 0;
static int16_t _calibration_P4 = 0;
static int16_t _calibration_P5 = 0;
static int16_t _calibration_P6 = 0;
static int16_t _calibration_P7 = 0;
static int16_t _calibration_P8 = 0;
static int16_t _calibration_P9 = 0;

// API functions definitions
bool cenviro_weather_init()
{
    if (!_cenviro_initialized)
    {
        LOG("Library not initialized exiting");
        return false;
    }

    if (ioctl(_cenviro_bus_fd, I2C_SLAVE, WEATHER_ADDR) < 0)
    {
        LOG("Failed to set weather sensor address\n");
        return false;
    }

    if (_initialize_BMP() != true)
    {
        LOG("Chip initialization failed");
        return false;
    }
    if (_read_BMP_calibration_data() != true)
    {
        LOG("Failed to read calibration data");
        return false;
    }

    _w_initialized = true;
    return true;
}

double cenviro_weather_temperature()
{
    if (!_w_initialized)
    {
        return 0.0;
    }
    ssize_t count = 0;

    ioctl(_cenviro_bus_fd, I2C_SLAVE, WEATHER_ADDR);

    _cenviro_buffer[0] = BMP_ADDRESS_RAW_TEMP;
    count = write(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to send command for temp. reading\n");
        return 0.0;
    }

    usleep(COMMAND_WAIT * 1000);
    count = read(_cenviro_bus_fd, _cenviro_buffer, 3);
    if (count != 3)
    {
        LOG("Failed to read raw temperature data\n");
        return 0.0;
    }

    int32_t full_raw_temp = 0;
    full_raw_temp = ((int32_t)_cenviro_buffer[0]) << 12 | ((int32_t)_cenviro_buffer[1]) << 4 | ((int32_t)_cenviro_buffer[2]) >> 4;

    int32_t calibrated_temp = _calibrate_temperature(full_raw_temp);

    return ((double)calibrated_temp) / 100;
}

double cenviro_weather_pressure()
{
    if (!_w_initialized)
    {
        return 0.0;
    }

    ssize_t count = 0;

    ioctl(_cenviro_bus_fd, I2C_SLAVE, WEATHER_ADDR);

    _cenviro_buffer[0] = BMP_ADDRESS_RAW_PRESS;
    count = write(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to send command for temp. reading\n");
        return 0.0;
    }

    usleep(COMMAND_WAIT * 1000);
    count = read(_cenviro_bus_fd, _cenviro_buffer, 3);
    if (count != 3)
    {
        LOG("Failed to read raw temperature data\n");
        return 0.0;
    }

    int32_t full_raw_press = 0;
    full_raw_press = ((int32_t)_cenviro_buffer[0]) << 12 | ((int32_t)_cenviro_buffer[1]) << 4 | ((int32_t)_cenviro_buffer[2]) >> 4;

    int32_t calibrated_press = _calibrate_pressure(full_raw_press);

    // return value in hPa
    return ((double)calibrated_press) / 100;
}

// internal functions definitions

// initialize chip with simple temperature and pressure measurement in normal mode
bool _initialize_BMP()
{
#define TEMP_MEASURE 0x01
#define PRESS_MEASURE 0x01
#define POWER_MODE 0x03
    uint8_t config = (TEMP_MEASURE << 5) | (PRESS_MEASURE << 2) | POWER_MODE;

    _cenviro_buffer[0] = BMP_ADDRRESS_CONTROL;
    _cenviro_buffer[1] = config;
    ssize_t count = 0;

    // send config
    count = write(_cenviro_bus_fd, _cenviro_buffer, 2);
    if (count != 2)
    {
        LOG("Failed to write config\n");
        return false;
    }

    // set device to return config
    usleep(COMMAND_WAIT * 1000);
    count = write(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to send command for config read\n");
        return false;
    }

    _cenviro_buffer[0] = 0x00;
    usleep(COMMAND_WAIT * 1000);
    count = read(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to read config\n");
        return false;
    }
    if (config != _cenviro_buffer[0])
    {
        LOG("Sent and received config differs\n");
        return false;
    }

#undef TEMP_MEASURE
#undef PRESS_MEASURE
#undef POWER_MODE
    return true;
}

bool _read_BMP_calibration_data()
{
    ssize_t count = 0;

    _cenviro_buffer[0] = BMP_ADDRESS_CALIBRATION_TEMP;
    count = write(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to send command for temperature calibration read\n");
        return false;
    }

    usleep(COMMAND_WAIT * 1000);
    count = read(_cenviro_bus_fd, _cenviro_buffer, 6);
    if (count != 6)
    {
        LOG("Failed to read pressure calibration data\n");
        return false;
    }
    _calibration_T1 = ((uint16_t)_cenviro_buffer[1]) << 8 | (uint16_t)_cenviro_buffer[0];
    _calibration_T2 = ((int16_t)_cenviro_buffer[3]) << 8 | (int16_t)_cenviro_buffer[2];
    _calibration_T3 = ((int16_t)_cenviro_buffer[5]) << 8 | (int16_t)_cenviro_buffer[4];

    _cenviro_buffer[0] = BMP_ADDRESS_CALIBRATION_PRESS;
    count = write(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to send command for pressure calibration read\n");
        return false;
    }

    usleep(COMMAND_WAIT * 1000);
    count = read(_cenviro_bus_fd, _cenviro_buffer, 18);
    if (count != 18)
    {
        LOG("Failed to read pressure calibration data\n");
        return false;
    }

    _calibration_P1 = ((uint16_t)_cenviro_buffer[1]) << 8 | (uint16_t)_cenviro_buffer[0];
    _calibration_P2 = ((int16_t)_cenviro_buffer[3]) << 8 | (int16_t)_cenviro_buffer[2];
    _calibration_P3 = ((int16_t)_cenviro_buffer[5]) << 8 | (int16_t)_cenviro_buffer[4];
    _calibration_P4 = ((int16_t)_cenviro_buffer[7]) << 8 | (int16_t)_cenviro_buffer[6];
    _calibration_P5 = ((int16_t)_cenviro_buffer[9]) << 8 | (int16_t)_cenviro_buffer[8];
    _calibration_P6 = ((int16_t)_cenviro_buffer[11]) << 8 | (int16_t)_cenviro_buffer[10];
    _calibration_P7 = ((int16_t)_cenviro_buffer[13]) << 8 | (int16_t)_cenviro_buffer[12];
    _calibration_P8 = ((int16_t)_cenviro_buffer[15]) << 8 | (int16_t)_cenviro_buffer[14];
    _calibration_P9 = ((int16_t)_cenviro_buffer[17]) << 8 | (int16_t)_cenviro_buffer[16];

    return true;
}

// temporary helper functions definitions
uint8_t cenviro_weather_chip_id()
{
    if (!_w_initialized)
    {
        return 0x00;
    }

    if (ioctl(_cenviro_bus_fd, I2C_SLAVE, WEATHER_ADDR) < 0)
    {
        LOG("Failed to set weather sensor address\n");
        return false;
    }

    _cenviro_buffer[0] = BMP_ADDRESS_ID;
    ssize_t count = 0;

    count = write(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to write command for chip id\n");
        return 0x00;
    }

    usleep(COMMAND_WAIT * 1000);
    count = read(_cenviro_bus_fd, _cenviro_buffer, 1);
    if (count != 1)
    {
        LOG("Failed to read chip id\n");
        return 0x00;
    }

    return _cenviro_buffer[0];
}

// compute temperature

int32_t t_fine = 0;

// _calibrate_temperature() is a compensation function from Bosh specification for BMP280
int32_t _calibrate_temperature(int32_t adc_T)
{
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)_calibration_T1 << 1))) * ((int32_t)_calibration_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)_calibration_T1)) * ((adc_T >> 4) - ((int32_t)_calibration_T1))) >> 12) *
            ((int32_t)_calibration_T3)) >>
           14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// _calibrate_pressure() is a compensation function from Bosh specification for BMP280
int32_t _calibrate_pressure(int32_t adc_P)
{
    int32_t var1 = 0, var2 = 0;
    uint32_t p = 0;

    if (t_fine == 0)
    {
        // this param is computed during temperature computation but needed for pressure calibration
        // if not set yet then force one temperature reading
        cenviro_weather_temperature();
    }

    var1 = (((int32_t)t_fine) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)_calibration_P6);
    var2 = var2 + ((var1 * ((int32_t)_calibration_P5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)_calibration_P4) << 16);
    var1 = (((_calibration_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)_calibration_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)_calibration_P1)) >> 15);
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
    if (p < 0x80000000)
    {
        p = (p << 1) / ((uint32_t)var1);
    }
    else
    {
        p = (p / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)_calibration_P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(p >> 2)) * ((int32_t)_calibration_P8)) >> 13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + _calibration_P7) >> 4));
    return p;
}
