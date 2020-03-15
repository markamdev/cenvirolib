#include <stdio.h>
#include <unistd.h>

#include "cel_led.h"

#define BLINK_TIME 500

int main(int argc, char* argv[]) {
    printf("CEnviroLib sample app\n");

    bool status = cel_led_init();
    if (!status) {
        printf("Failed to initilize leds - exiting\n");
        return 1;
    }

    printf("1. Blink LEDs 5 times\n");
    for (int i = 0; i < 5; ++i) {
        cel_led_set(true);
        printf("- blink -\n");
        usleep(BLINK_TIME * 1000);
        cel_led_set(false);
        usleep(BLINK_TIME * 1000);
    }

    cel_led_deinit();

    return 0;
}