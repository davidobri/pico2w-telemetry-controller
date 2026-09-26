#include <stdio.h>
#include "pico/stdlib.h"

#define FIRMWARE_VERSION "0.1.0"

int main()
{
    stdio_init_all();

    sleep_ms(2000);

    printf("\n");
    printf("=====================================\n");
    printf(" Pico 2 W Telemetry Controller\n");
    printf(" Firmware Version: %s\n", FIRMWARE_VERSION);
    printf(" System Boot: OK\n");
    printf("=====================================\n");

    while (true) {
        printf("System running...\n");
        sleep_ms(1000);
    }
}