#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include <cenviro.h>

#define DOT_TIME 300
#define DOT_PAUSE 300
#define DASH_TIME 800
#define DASH_PAUSE 800
#define CHAR_PAUSE 800
#define MESSAGE_PAUSE 1200

static void _show_dot();
static void _show_dash();
static void _print_help(const char *name);

void signal_handler(int signal)
{
    cenviro_deinit();
    exit(0);
}

int main(int argc, char *argv[])
{
    bool status = false;
    bool help = false;
    bool infinite = false;

    printf("SOS blinker\n");

    if (argc > 2)
    {
        _print_help(argv[0]);
        return 1;
    }
    if (argc == 2)
    {
        if (strncmp(argv[1], "-i", 2) == 0)
        {
            infinite = true;
        }
        else
        {
            help = true;
        }
    }
    if (help)
    {
        _print_help(argv[0]);
        return 0;
    }

    signal(SIGINT, signal_handler);
    status = cenviro_init();
    if (!status)
    {
        printf("Initialization error...\n");
        return 1;
    }

    do
    {
        for (int i = 0; i < 3; ++i)
        {
            _show_dot();
        }

        usleep(CHAR_PAUSE * 1000);
        for (int i = 0; i < 3; ++i)
        {
            _show_dash();
        }
        usleep(CHAR_PAUSE * 1000);

        for (int i = 0; i < 3; ++i)
        {
            _show_dot();
        }
        usleep(MESSAGE_PAUSE * 1000);

        printf("\n");
    } while (infinite);

    cenviro_deinit();
    return 0;
}

static void _show_dot()
{
    printf(".");
    fflush(stdout);
    cenviro_led_set(true);
    usleep(DOT_TIME * 1000);
    cenviro_led_set(false);
    usleep(DOT_PAUSE * 1000);
}

static void _show_dash()
{
    printf("-");
    fflush(stdout);
    cenviro_led_set(true);
    usleep(DASH_TIME * 1000);
    cenviro_led_set(false);
    usleep(DASH_PAUSE * 1000);
}

static void _print_help(const char *name)
{
    printf("Usage:\n%s [option]\n\n", name);
    printf("Where possinle options are:\n-h\tprint help message\n-i\t repeat infinitely\n\n");
}
