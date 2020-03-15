#include <stdio.h>
#include <unistd.h>

#include "cel_led.h"
#include "cel_weather.h"

#define BLINK_TIME 500
#define READ_WEATHER_DELAY 1000

int main(int argc, char *argv[])
{
    printf("CEnviroLib sample app\n");

    bool status = false;

    printf("1. Blink LEDs\n");
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

    printf("2. Read temperature sensor data\n");
    status = cel_weather_init();
    if (!status)
    {
        printf("Failed to initilize weather sensors - exiting\n");
        return 1;
    }
    uint8_t chip_id = cel_read_chip_id();
    printf("Received chip id is: 0x%2x\n", chip_id);

    for (int i = 0; i < 5; ++i)
    {
        double temp = cel_weather_read_temp();
        printf("- current temp is: %.1f\n", temp);
        usleep(READ_WEATHER_DELAY * 1000);
    }

    printf("3. Read barometer data\n");

    printf("4. Read light sensor data\n");

    // deinitialize all library modules
    cel_led_deinit();
    cel_weather_deinit();

    return 0;
}
