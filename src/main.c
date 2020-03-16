#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "cel_led.h"
#include "cel_weather.h"

#define BLINK_TIME 500
#define READ_WEATHER_DELAY 1000

// config flags
static bool _opt_all = false;
static bool _opt_led = false;
static bool _opt_temp = false;
static bool _opt_pressure = false;
static bool _opt_light = false;

static bool _parse_options(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    printf("CEnviroLib sample app\n");

    bool status = _parse_options(argc, argv);
    if (!status)
    {
        return 0;
    }

    if (_opt_all || _opt_led)
    {
        printf("Blink LEDs\n");
        status = cel_led_init();
        if (!status)
        {
            printf("Failed to initilize leds - exiting\n");
            return 1;
        }

        for (int i = 0; i < 5; ++i)
        {
            cel_led_set(true);
            printf("- blink -\n");
            usleep(BLINK_TIME * 1000);
            cel_led_set(false);
            usleep(BLINK_TIME * 1000);
        }

        cel_led_deinit();
    }

    if (_opt_all || _opt_temp)
    {
        printf("Read temperature sensor data\n");
        status = cel_weather_init();
        if (!status)
        {
            printf("Failed to initilize weather sensors - exiting\n");
            return 1;
        }
        uint8_t chip_id = cel_read_chip_id();
        if (chip_id != 0x58)
        {
            printf("Unsupported temp/pressure chip: 0x%2x\n", chip_id);
            return 1;
        }

        for (int i = 0; i < 5; ++i)
        {
            double temp = cel_weather_read_temp();
            printf("- current temp is: %.1f [*C]\n", temp);
            usleep(READ_WEATHER_DELAY * 1000);
        }

        cel_weather_deinit();
    }

    if (_opt_all || _opt_pressure)
    {
        printf("Read barometer data\n");
        status = cel_weather_init();
        if (!status)
        {
            printf("Failed to initilize weather sensors - exiting\n");
            return 1;
        }
        uint8_t chip_id = cel_read_chip_id();
        if (chip_id != 0x58)
        {
            printf("Unsupported temp/pressure chip: 0x%2x\n", chip_id);
            return 1;
        }

        for (int i = 0; i < 5; ++i)
        {
            double press = cel_weather_read_baro();
            printf("- current pressure is: %.1f [hPa]\n", press);
            usleep(READ_WEATHER_DELAY * 1000);
        }

        cel_weather_deinit();
    }

    if (_opt_all || _opt_light)
    {
        printf("Read light sensor data\n");
    }

    return 0;
}

void _print_help(const char *name)
{
    printf("Usage:\n");
    printf("%s [OPTIONS]\n", name);
    printf("\nAllowed options are:\n");
    printf("-h\tprint this help message\n");
    printf("-a\tlaunch _a_ll demo scenarios\n");
    printf("-l\tlaunch _L_ED blinking\n");
    printf("-t\tlaunch _t_empature reader\n");
    printf("-p\tlaunch _p_ressure reader\n");
    printf("-i\tlaunch light _i_ntensity reader\n");
}

bool _parse_options(int argc, char *argv[])
{
    if (argc == 0 || !argv)
    {
        // something is not OK - return
        return false;
    }
    if (argc == 1)
    {
        // no options specified - use all
        _opt_all = true;
        return false;
    }
    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-h", 2) == 0)
        {
            _print_help(argv[0]);
            return false;
        }
        if (strncmp(argv[i], "-a", 2) == 0)
        {
            _opt_all = true;
            // no need to check more options if -a selected
            return true;
        }
        if (strncmp(argv[i], "-l", 2) == 0)
        {
            _opt_led = true;
            continue;
        }
        if (strncmp(argv[i], "-t", 2) == 0)
        {
            _opt_temp = true;
            continue;
        }
        if (strncmp(argv[i], "-p", 2) == 0)
        {
            _opt_pressure = true;
            continue;
        }
        if (strncmp(argv[i], "-i", 2) == 0)
        {
            _opt_light = true;
            continue;
        }
    }
    return true;
}
