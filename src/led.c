#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#include "cel_led.h"
#include "logs.h"

#define LED_PIN 4

static bool _led_initialized = false;

#define BUFFER_MAX 3
#define DIRECTION_MAX 35
#define VALUE_MAX 30

static bool _gpio_export();
static bool _gpio_set_output();
static bool _gpio_set_value(bool value);
static bool _gpio_unexport();

bool cel_led_init() {
    if (!_gpio_export()) {
        LOG("GPIO export failed\n");
        return false;
    }
    if (!_gpio_set_output()) {
        LOG("GPIO direction set failed\n");
        return false;
    }
    _led_initialized = true;
    return true;
}

void cel_led_set(bool state){
    if (!_led_initialized) {
        return;
    }
    if (!_gpio_set_value(state)) {
        LOG("GPIO value set failed\n");
    }
}

void cel_led_deinit() {
    if (!_led_initialized) {
        return;
    }
    _gpio_unexport();
}

bool _gpio_export() {
	int fd = 0;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
        LOG("Failed to open export file\n");
		return false;
	}

	char buffer[BUFFER_MAX];
	ssize_t string_len;
	string_len = snprintf(buffer, BUFFER_MAX, "%d", LED_PIN);
	write(fd, buffer, string_len);
	close(fd);
	return true;
}

static bool _gpio_set_output()
{
	char path[DIRECTION_MAX];
	int fd = 0;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", LED_PIN);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
        LOG("Failed to open direction file\n");
		return false;
	}

	if (-1 == write(fd, "out", 3)) {
        LOG("Failed to write direction\n");
		return false;
	}

	close(fd);
	return true;
}


static bool _gpio_set_value(bool value) {
	static const char s_values_str[] = "01";

	char path[VALUE_MAX];
	int fd = 0;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", LED_PIN);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
        LOG("Failed to open value file\n");
		return false;
	}

	if (1 != write(fd, &s_values_str[value == true ? 1 : 0], 1)) {
        LOG("Failed to write value\n");
		return false;
	}

	close(fd);
    return true;
}

bool _gpio_unexport() {
    char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		LOG("Failed to open unexport file\n");
		return false;
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", LED_PIN);
	write(fd, buffer, bytes_written);
	close(fd);

	return true;
}