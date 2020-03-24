#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <cel_weather.h>

#define PRINT_REFRESH_TIME 2000
#define DATA_RELOAD_DELAY 1000

// message to be printed (in the same line - thus carriage return)
static const char *_meteo_message = "\rTemperature: %2.1f C\t Pressure: %4.1f hPa";

// global variables for storing meteo data (asynchronously updated)
static double temperature = 0.0;
static double pressure = 0.0;

// meteo data update thread
static void *meteo_fetcher(void *params);

int main(int argc, char *argv[])
{
    // screan cleaning
    printf("\033[2J\033[1;1H");

    printf("Sample METEO app for CEnviroLib\n\n");
    pthread_t updater = 0;

    bool status = cel_weather_init();
    if (!status)
    {
        printf("ERROR: Failed to initialize weather library - exiting\n");
        return 1;
    }

    int result = pthread_create(&updater, NULL, meteo_fetcher, NULL);
    if (result != 0)
    {
        printf("ERROR: Failed to launch updater thread - exiting...\n");
        return 1;
    }

    while (true)
    {
        printf(_meteo_message, temperature, pressure);
        fflush(stdout);
        usleep(PRINT_REFRESH_TIME * 1000);
    }

    pthread_join(updater, NULL);

    return 0;
}

static void *meteo_fetcher(void *params)
{
    while (true)
    {
        pressure = cel_weather_read_baro();
        temperature = cel_weather_read_temp();
        usleep(DATA_RELOAD_DELAY * 1000);
    }
    return NULL;
}
