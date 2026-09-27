#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/adc.h"
#include "hardware/sync.h"

#define FIRMWARE_VERSION "0.3.0"

#define ADC_GPIO 26
#define ADC_CHANNEL 0

#define SAMPLE_RATE_HZ 100
#define SAMPLE_PERIOD_US 10000

#define PRINT_EVERY_N_SAMPLES 25


// Shared between the timer callback and main loop
volatile uint16_t latest_adc_sample = 0;
volatile uint32_t sample_count = 0;
volatile bool sample_ready = false;
volatile uint32_t sample_overruns = 0;


/*
 * Timer callback
 *
 * Runs every 10 ms (100 Hz).
 * Keep this function short and predictable.
 */
bool sample_timer_callback(struct repeating_timer *t)
{
    uint16_t sample = adc_read();

    if (sample_ready) {
        sample_overruns++;
    }

    latest_adc_sample = sample;
    sample_count++;
    sample_ready = true;

    return true;
}


int main()
{
    stdio_init_all();

    // Give USB serial time to connect
    sleep_ms(2000);

    // Initialize ADC hardware
    adc_init();
    adc_gpio_init(ADC_GPIO);
    adc_select_input(ADC_CHANNEL);

    // Create repeating timer
    struct repeating_timer sample_timer;

    bool timer_started = add_repeating_timer_us(
        -SAMPLE_PERIOD_US,
        sample_timer_callback,
        NULL,
        &sample_timer
    );

    printf("\n");
    printf("=====================================\n");
    printf(" Pico 2 W Telemetry Controller\n");
    printf(" Firmware Version: %s\n", FIRMWARE_VERSION);
    printf(" System Boot: OK\n");
    printf(" ADC0 / GPIO26: Initialized\n");
    printf(" Sample Rate: %d Hz\n", SAMPLE_RATE_HZ);
    printf(" Timer: %s\n", timer_started ? "Started" : "FAILED");
    printf("=====================================\n");

    if (!timer_started) {
        while (true) {
            printf("FAULT: Sampling timer failed to start\n");
            sleep_ms(1000);
        }
    }

    while (true) {

        if (sample_ready) {

            uint32_t interrupt_state = save_and_disable_interrupts();

            uint16_t raw_adc = latest_adc_sample;
            uint32_t current_sample = sample_count;
            uint32_t overruns = sample_overruns;

            sample_ready = false;

            restore_interrupts(interrupt_state);

            float voltage = raw_adc * (3.3f / 4095.0f);

            // ADC is sampled at 100 Hz,
            // but only print every 25 samples = 4 times/second.
            if ((current_sample % PRINT_EVERY_N_SAMPLES) == 0) {

                printf(
                    "Sample: %lu | ADC: %4u | Voltage: %.3f V | Overruns: %lu\n",
                    (unsigned long)current_sample,
                    raw_adc,
                    voltage,
                    (unsigned long)overruns
                );
            }
        }

        tight_loop_contents();
    }
}