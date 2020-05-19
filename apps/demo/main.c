#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include <cenviro.h>

#define BLINK_TIME 500
#define READ_WEATHER_DELAY 1000
#define READ_MOTION_DELAY 1000

// config flags
static bool _opt_all = false;
static bool _opt_led = false;
static bool _opt_temp = false;
static bool _opt_pressure = false;
static bool _opt_light = false;
static bool _opt_motion = false;
static bool _opt_m_temp = false;

static bool _parse_options(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    printf("CEnviroLib sample app\n");

    bool status = _parse_options(argc, argv);
    if (!status)
    {
        return 0;
    }

    status = cenviro_init();
    if (!status)
    {
        printf("Failed to initialize cenviro library\n");
        return 1;
    }

    if (_opt_all || _opt_led)
    {
        printf("Blink LEDs\n");

        for (int i = 0; i < 5; ++i)
        {
            cenviro_led_set(true);
            printf("- blink -\n");
            usleep(BLINK_TIME * 1000);
            cenviro_led_set(false);
            usleep(BLINK_TIME * 1000);
        }
    }

    if (_opt_all || _opt_temp)
    {
        printf("Read temperature data\n");
        uint8_t chip_id = cenviro_weather_chip_id();
        if (chip_id != 0x58)
        {
            printf("Unsupported temp/pressure chip: 0x%2x\n", chip_id);
            cenviro_deinit();
            return 1;
        }

        for (int i = 0; i < 5; ++i)
        {
            double temp = cenviro_weather_temperature();
            printf("- current temp is: %.1f [*C]\n", temp);
            usleep(READ_WEATHER_DELAY * 1000);
        }
    }

    if (_opt_all || _opt_pressure)
    {
        printf("Read barometer data\n");
        uint8_t chip_id = cenviro_weather_chip_id();
        if (chip_id != 0x58)
        {
            printf("Unsupported temp/pressure chip: 0x%02x\n", chip_id);
            cenviro_deinit();
            return 1;
        }

        for (int i = 0; i < 5; ++i)
        {
            double press = cenviro_weather_pressure();
            printf("- current pressure is: %.1f [hPa]\n", press);
            usleep(READ_MOTION_DELAY * 1000);
        }
    }

    if (_opt_all || _opt_light)
    {
        printf("Read light sensor data\n");
        uint8_t chip_id = cenviro_light_chip_id();
        if (chip_id != 0x44 && chip_id != 0x4d)
        {
            printf("Unsupported light chip: 0x%02x\n", chip_id);
            cenviro_deinit();
            return 1;
        }
        else
        {
            printf("Chip version detected: 0x%02x (%s)\n", chip_id, cenviro_light_chip_name());
        }

        for (int i = 0; i < 5; ++i)
        {
            cenviro_crgb_t color = cenviro_light_crgb_scaled();
            printf("- detected color composition is (C [R/G/B]): %4d [%3d/%3d/%3d]\n",
                   color.clear, color.red, color.green, color.blue);
            usleep(READ_WEATHER_DELAY * 1000);
        }
    }
    if (_opt_all || _opt_motion)
    {
        printf("Read motion sensor data\n");
    }
    if (_opt_all || _opt_m_temp)
    {
        printf("Read temperature from motion sensor\n");
        uint8_t chip_id = cenviro_motion_chip_id();
        if (chip_id != 0x49)
        {
            printf("Unsupported motion chip: 0x%02x\n", chip_id);
            cenviro_deinit();
            return 1;
        }
        else
        {

            printf("Chip version detected: 0x%02x\n", chip_id);
        }
        for (int i = 0; i < 5; ++i)
        {
            double temp = cenviro_motion_temperature();
            printf("- current temp is: %.1f [*C]\n", temp);
            usleep(READ_WEATHER_DELAY * 1000);
        }
    }

    cenviro_deinit();
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
    printf("-m\tlaunch _m_otion sensor reader\n");
    printf("-e\tlaunch t_e_mperature reader using motion sensor chip\n");
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
        if (strncmp(argv[i], "-m", 2) == 0)
        {
            _opt_motion = true;
            continue;
        }
        if (strncmp(argv[i], "-e", 2) == 0)
        {
            _opt_m_temp = true;
            continue;
        }
    }
    return true;
}
