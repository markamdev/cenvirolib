#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <cenviro.h>

#include "al-utils.h"

#define RECHECK_INTERVAL 5 // interval in [s]

static bool _verbose = false;
static bool _help = false;
static int _level = 50;

static int _check_params(int count, const char **params);
static void _print_help(char *name);
static void _sigin_handler(int signal);

int main(int argc, char **argv)
{
    int par_check = _check_params(argc, (const char **)argv);
    if (par_check != 0)
    {
        return 1;
    }
    if (_help)
    {
        _print_help(argv[0]);
        return 0;
    }
    if (_verbose)
    {
        printf("CEnviro auto light app\n");
        printf("- using light level threshold: %d\n", _level);
    }
    // initialize cenviro library if no issues till now
    bool result = cenviro_init();
    if (!result)
    {
        printf("Unable to initialize cenviro library\n");
        return 1;
    }
    // register signal handle
    signal(SIGINT, _sigin_handler);
    // start light measurement in infinite loop
    while (true)
    {
        cenviro_crgb_t measurement = cenviro_light_crgb_scaled();

        al_add_measurement(measurement.clear);

        al_compare_with_threshold(_level);

        al_log_state();
        sleep(RECHECK_INTERVAL);
    }
    return 0;
}

static int _check_params(int count, const char **params)
{
    if (count == 0 || params == NULL)
    {
        // this should never happen
        return 1;
    }
    if (count == 1)
    {
        return 0;
    }
    // skip app name
    ++params;
    for (int i = 1; i < count; ++i, ++params)
    {
        if (!strncmp("-v", *params, 2))
        {
            _verbose = true;
            continue;
        }
        if (!strncmp("-h", *params, 2))
        {
            _help = true;
            break;
        }
        if (!strncmp("-l", *params, 2))
        {
            ++i;
            ++params;
            if (i == count)
            {
                // error - no value after 'l'
                return 1;
            }
            int newlevel = 0;
            if (sscanf(*params, "%d", &newlevel) != 1)
            {
                if (_verbose)
                {
                    printf("Invalid level value: %s\n", *params);
                    return 1;
                }
            }
            if (newlevel < 0)
            {
                // threshold cannot be < 0
                if (_verbose)
                {
                    printf("Negative level value: %s\n", *params);
                    return 1;
                }
            }
            _level = newlevel;
            continue;
        }
    }
    return 0;
}

static void _print_help(char *name)
{
    printf("Usage:\n%s [options]\n\n", name);
    printf("Possible options are:\n-h\t\tprint help message\n-v\t\trun in verbose mode (with console output)\n");
    printf("-l value\tset light switch threshold to value\n");
}

static void _sigin_handler(int signal)
{
    cenviro_deinit();
    exit(0);
}
