#include <stdio.h>

#include "al-utils.h"

static uint32_t _counter = 0;
static uint16_t _samples[4] = {0};
static bool _led_state = false;
static bool _is_full = false;

void al_add_measurement(uint16_t value)
{
    _samples[_counter & 0x03] = value;
    _counter++;
    if (_counter == 4)
    {
        _is_full = true;
    }
}

static uint16_t _get_average()
{
    uint8_t count = 0;
    if (_is_full)
    {
        count = 4;
    }
    else
    {
        // in case of some error it's better to mas counter value
        count = _counter & 0x03;
    }
    uint32_t avg = 0;
    for (uint8_t i = 0; i < count; ++i)
    {
        avg += _samples[i];
    }
    avg /= count;
    return (uint16_t)avg;
}

void al_compare_with_threshold(uint16_t thr)
{
    uint16_t average = _get_average();
    if (average >= thr)
    {
        // if already turned off then no need to change anything
        // otherwise turn LEDs off
        if (_led_state == true)
        {
            _led_state = false;
            cenviro_led_set(_led_state);
        }
    }
    else
    {
        // average value below threshold - turn light on if not already on
        if (_led_state == false)
        {
            _led_state = true;
            cenviro_led_set(_led_state);
        }
    }
}

void al_log_state()
{
    uint16_t last_value = _samples[(_counter - 1) & 0x03];
    uint16_t average = _get_average();
    printf("Last measurement: %d Current average: %d Light state: %d\n", last_value, average, _led_state);
}
