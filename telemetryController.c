#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/adc.h"
#include "hardware/sync.h"

#define FIRMWARE_VERSION "0.4.0"

#define ADC_GPIO 26
#define ADC_CHANNEL 0

#define SAMPLE_RATE_HZ 100
#define SAMPLE_PERIOD_US 10000

#define PRINT_EVERY_N_SAMPLES 25

#define SAMPLE_BUFFER_SIZE 128


typedef struct
{
    uint16_t adc_value;
    uint32_t sample_number;
} sample_t;


// Circular sample buffer
volatile sample_t sample_buffer[SAMPLE_BUFFER_SIZE];

// Producer writes at head
volatile uint32_t buffer_head = 0;

// Consumer reads from tail
volatile uint32_t buffer_tail = 0;

// Total number of samples generated
volatile uint32_t sample_count = 0;

// Number of samples that could not be stored
volatile uint32_t dropped_samples = 0;


/*
 * Get next circular-buffer index.
 */
static inline uint32_t buffer_next_index(uint32_t index)
{
    return (index + 1) % SAMPLE_BUFFER_SIZE;
}


/*
 * Timer callback
 *
 * Runs every 10 ms = 100 Hz.
 * This is the producer side of the buffer.
 */
bool sample_timer_callback(struct repeating_timer *t)
{
    uint16_t adc_sample = adc_read();

    sample_count++;

    uint32_t next_head = buffer_next_index(buffer_head);

    /*
     * If the next head position equals tail,
     * the buffer is full.
     */
    if (next_head == buffer_tail) {
        dropped_samples++;
        return true;
    }

    sample_buffer[buffer_head].adc_value = adc_sample;
    sample_buffer[buffer_head].sample_number = sample_count;

    buffer_head = next_head;

    return true;
}


/*
 * Remove one sample from the circular buffer.
 *
 * Returns true if a sample was available.
 * Returns false if the buffer was empty.
 */
bool sample_buffer_pop(sample_t *sample)
{
    uint32_t interrupt_state = save_and_disable_interrupts();

    if (buffer_head == buffer_tail) {
        restore_interrupts(interrupt_state);
        return false;
    }

    sample->adc_value =
        sample_buffer[buffer_tail].adc_value;

    sample->sample_number =
        sample_buffer[buffer_tail].sample_number;

    buffer_tail = buffer_next_index(buffer_tail);

    restore_interrupts(interrupt_state);

    return true;
}


int main()
{
    stdio_init_all();

    sleep_ms(2000);

    // Initialize ADC
    adc_init();
    adc_gpio_init(ADC_GPIO);
    adc_select_input(ADC_CHANNEL);

    // Create repeating sampling timer
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
    printf(" Sample Buffer: %d entries\n", SAMPLE_BUFFER_SIZE);
    printf(" Stress Test: ENABLED\n");
    printf(" Consumer Delay: 20 ms/sample\n");
    printf(" Timer: %s\n", timer_started ? "Started" : "FAILED");
    printf("=====================================\n");

    if (!timer_started) {
        while (true) {
            printf("FAULT: Sampling timer failed to start\n");
            sleep_ms(1000);
        }
    }

    sample_t sample;

    while (true) {

        /*
         * Read samples from circular buffer.
         */
        while (sample_buffer_pop(&sample)) {

            float voltage =
                sample.adc_value * (3.3f / 4095.0f);

            if ((sample.sample_number % PRINT_EVERY_N_SAMPLES) == 0) {

                printf(
                    "Sample: %lu | ADC: %4u | Voltage: %.3f V | Dropped: %lu\n",
                    (unsigned long)sample.sample_number,
                    sample.adc_value,
                    voltage,
                    (unsigned long)dropped_samples
                );
            }
        }

        tight_loop_contents();
    }
}