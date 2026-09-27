#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define FIRMWARE_VERSION "0.2.0"

#define ADC_GPIO 26
#define ADC_CHANNEL 0

int main()
{
    stdio_init_all();

    // Give USB serial time to connect
    sleep_ms(2000);

    // Initialize the ADC hardware
    adc_init();

    // Configure GPIO26 for ADC input
    adc_gpio_init(ADC_GPIO);

    // Select ADC0
    adc_select_input(ADC_CHANNEL);

    printf("\n");
    printf("=====================================\n");
    printf(" Pico 2 W Telemetry Controller\n");
    printf(" Firmware Version: %s\n", FIRMWARE_VERSION);
    printf(" System Boot: OK\n");
    printf(" ADC0 / GPIO26: Initialized\n");
    printf("=====================================\n");

    while (true) {

        // Read the 12-bit ADC value
        uint16_t raw_adc = adc_read();

        // Convert the ADC value to voltage
        float voltage = raw_adc * (3.3f / 4095.0f);

        printf("ADC Raw: %4u | Voltage: %.3f V\n",
               raw_adc,
               voltage);

        sleep_ms(250);
    }
}