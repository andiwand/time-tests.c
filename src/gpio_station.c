#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include "gpio.h"
#include "shared.h"

void send(void);
void sig_handler(int signo);

gpio_t gpio;
int pin;

int main(int argc, char** argv) {
    char* prog = argv[0]; argv = argv + 1; argc--;

    float interval = 1;

    switch (argc) {
    case 2:
        interval = strtof(argv[1], NULL);
    case 1:
        pin = strtol(argv[0], NULL, 10);
        break;
    default:
        fprintf(stderr, "usage: %s <pin> [<interval]\n\nsends digital signals to the given pin with a constant interval\n", prog);
        return 1;
    }

    defaults();
    gpio = gpio_setup(rpi_version());

    INP_GPIO(gpio, pin);
    OUT_GPIO(gpio, pin);

    if (interval == 0) {
        signal(SIGUSR1, sig_handler);
        while (1) sleep(1);
    } else {
        while (1) {
            fsleep(interval);
            send();
        }
    }

    return 0;
}

void send(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    GPIO_SET(gpio, pin);
    usleep(10);
    GPIO_CLR(gpio, pin);

    printf("%ld.%09ld\n", ts.tv_sec, ts.tv_nsec);
}

void sig_handler(int signo) {
    send();
}
