#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#include "cenviro.h"
#include "internal.h"
#include "logs.h"

// number of retrials when accessing /sys/class/gpio/*
#define RETRIAL_COUNT 3
// time for delay between retrials (in [ms])
#define DELAY_TIME 100

static bool _led_initialized = false;
static int _led_fd = 0;

#define BUFFER_MAX 3
#define DIRECTION_MAX 35
#define VALUE_MAX 30

static bool _gpio_export();
static bool _gpio_set_output();
static bool _gpio_open_output_file();
static bool _gpio_set_value(bool value);
static bool _gpio_unexport();

bool cenviro_led_init()
{
    if (!_gpio_export())
    {
        LOG("GPIO export failed\n");
        return false;
    }
    if (!_gpio_set_output())
    {
        LOG("GPIO direction set failed\n");
        return false;
    }
    if (!_gpio_open_output_file())
    {
        LOG("LED GPIO ouptup file opening failed\n");
        return false;
    }
    _led_initialized = true;
    return true;
}

void cenviro_led_set(bool state)
{
    if (!_led_initialized)
    {
        return;
    }
    if (!_gpio_set_value(state))
    {
        LOG("GPIO value set failed\n");
    }
}

void cenviro_led_deinit()
{
    if (!_led_initialized)
    {
        return;
    }
    _gpio_unexport();
    if (_led_fd != 0)
    {
        close(_led_fd);
        _led_fd = 0;
    }
    _led_initialized = false;
}

bool _gpio_export()
{
    int fd = 0;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd)
    {
        LOG("Failed to open export file\n");
        return false;
    }

    char buffer[BUFFER_MAX];
    ssize_t string_len = 0;
    string_len = snprintf(buffer, BUFFER_MAX, "%d", LED_PIN);
    ssize_t retval = write(fd, buffer, string_len);
    if (retval != string_len)
    {
        LOG("Failed to export LED pin number\n");
        return false;
    }

    close(fd);
    return true;
}

static bool _gpio_set_output()
{
    char path[DIRECTION_MAX];
    int fd = 0;
    int retrial_left = RETRIAL_COUNT;

    snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", LED_PIN);
    while (retrial_left > 0)
    {
        LOG("Trying to open direction file\n");
        fd = open(path, O_WRONLY);
        if (fd >= 0)
        {
            // successfully opened the file
            break;
        }
        if (0 == retrial_left)
        {
            // no more retrials left while error still occurs
            LOG("Failed to open direction file\n");
            return false;
        }
        usleep(DELAY_TIME * 1000);
        --retrial_left;
    }

    retrial_left = RETRIAL_COUNT;
    while (retrial_left > 0)
    {
        LOG("Trying to write direction\n");
        ssize_t retval = write(fd, "out", 3);
        if (3 == retval)
        {
            break;
        }
        if (0 == retrial_left)
        {
            LOG("Failed to write direction\n");
            return false;
        }
        usleep(DELAY_TIME * 1000);
        --retrial_left;
    }

    close(fd);
    return true;
}

static bool _gpio_open_output_file()
{
    char path[VALUE_MAX];
    int fd = 0;
    int retrial_left = RETRIAL_COUNT;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", LED_PIN);
    while (retrial_left > 0)
    {
        LOG("Trying to open value file\n");
        fd = open(path, O_WRONLY);
        if (fd >= 0)
        {
            // successfully opened the file
            break;
        }
        if (0 == retrial_left)
        {
            // no more retrials left while error still occurs
            LOG("Failed to open value file\n");
            return false;
        }
        usleep(DELAY_TIME * 1000);
        --retrial_left;
    }

    _led_fd = fd;
    return true;
}

static bool _gpio_set_value(bool value)
{
    static const char s_values_str[] = "01";
    int retrial_left = RETRIAL_COUNT;

    while (retrial_left > 0)
    {
        LOG("Trying to write value\n");
        ssize_t retval = write(_led_fd, &s_values_str[value == true ? 1 : 0], 1);
        if (1 == retval)
        {
            // write succeeded - break the loop
            break;
        }
        if (0 == retrial_left)
        {
            LOG("Failed to write value\n");
            return false;
        }
        usleep(DELAY_TIME * 1000);
        --retrial_left;
    }

    return true;
}

bool _gpio_unexport()
{
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd)
    {
        LOG("Failed to open unexport file\n");
        return false;
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", LED_PIN);
    write(fd, buffer, bytes_written);
    close(fd);

    return true;
}
