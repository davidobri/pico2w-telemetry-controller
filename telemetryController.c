#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/adc.h"
#include "hardware/sync.h"

#define FIRMWARE_VERSION "0.5.0"

#define ADC_GPIO 26
#define ADC_CHANNEL 0

#define SAMPLE_RATE_HZ 100
#define SAMPLE_PERIOD_US 10000

#define SAMPLE_BUFFER_SIZE 128

// Telemetry protocol
#define PACKET_SYNC_0 0xAA
#define PACKET_SYNC_1 0x55

#define PROTOCOL_VERSION 0x01
#define PACKET_TYPE_ADC_SAMPLE 0x01

#define ADC_PAYLOAD_LENGTH 11
#define TELEMETRY_PACKET_SIZE 18

#define STATUS_BUFFER_OVERRUN 0x01


typedef struct
{
    uint16_t adc_value;
    uint32_t sample_number;
} sample_t;


// Circular sample buffer
volatile sample_t sample_buffer[SAMPLE_BUFFER_SIZE];

volatile uint32_t buffer_head = 0;
volatile uint32_t buffer_tail = 0;

volatile uint32_t sample_count = 0;
volatile uint32_t dropped_samples = 0;

uint16_t packet_sequence = 0;


/*
 * Return next circular-buffer index.
 */
static inline uint32_t buffer_next_index(uint32_t index)
{
    return (index + 1) % SAMPLE_BUFFER_SIZE;
}


/*
 * Write 16-bit value in little-endian format.
 */
void write_u16_le(uint8_t *buffer, uint16_t value)
{
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
}


/*
 * Write 32-bit value in little-endian format.
 */
void write_u32_le(uint8_t *buffer, uint32_t value)
{
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
    buffer[2] = (uint8_t)((value >> 16) & 0xFF);
    buffer[3] = (uint8_t)((value >> 24) & 0xFF);
}


/*
 * Timer callback.
 *
 * Producer side of the circular buffer.
 * Samples ADC at 100 Hz.
 */
bool sample_timer_callback(struct repeating_timer *t)
{
    uint16_t adc_sample = adc_read();

    sample_count++;

    uint32_t next_head =
        buffer_next_index(buffer_head);

    if (next_head == buffer_tail) {
        dropped_samples++;
        return true;
    }

    sample_buffer[buffer_head].adc_value =
        adc_sample;

    sample_buffer[buffer_head].sample_number =
        sample_count;

    buffer_head = next_head;

    return true;
}


/*
 * Remove one sample from circular buffer.
 */
bool sample_buffer_pop(sample_t *sample)
{
    uint32_t interrupt_state =
        save_and_disable_interrupts();

    if (buffer_head == buffer_tail) {

        restore_interrupts(interrupt_state);

        return false;
    }

    sample->adc_value =
        sample_buffer[buffer_tail].adc_value;

    sample->sample_number =
        sample_buffer[buffer_tail].sample_number;

    buffer_tail =
        buffer_next_index(buffer_tail);

    restore_interrupts(interrupt_state);

    return true;
}


/*
 * Safely read dropped-sample counter.
 */
uint32_t get_dropped_sample_count(void)
{
    uint32_t interrupt_state =
        save_and_disable_interrupts();

    uint32_t dropped = dropped_samples;

    restore_interrupts(interrupt_state);

    return dropped;
}


/*
 * Build binary ADC telemetry packet.
 */
void build_adc_packet(
    uint8_t *packet,
    const sample_t *sample,
    uint32_t dropped)
{
    uint8_t status_flags = 0;

    if (dropped > 0) {
        status_flags |= STATUS_BUFFER_OVERRUN;
    }

    // Header
    packet[0] = PACKET_SYNC_0;
    packet[1] = PACKET_SYNC_1;
    packet[2] = PROTOCOL_VERSION;
    packet[3] = PACKET_TYPE_ADC_SAMPLE;
    packet[4] = ADC_PAYLOAD_LENGTH;

    write_u16_le(
        &packet[5],
        packet_sequence
    );

    // Payload
    write_u32_le(
        &packet[7],
        sample->sample_number
    );

    write_u16_le(
        &packet[11],
        sample->adc_value
    );

    write_u32_le(
        &packet[13],
        dropped
    );

    packet[17] = status_flags;

    packet_sequence++;
}


/*
 * Send raw binary packet over USB stdio.
 *
 * CR/LF translation is disabled because this
 * is binary data, not human-readable text.
 */
void send_packet(const uint8_t *packet)
{
    stdio_put_string(
        (const char *)packet,
        TELEMETRY_PACKET_SIZE,
        false,
        false
    );
}


int main()
{
    stdio_init_all();

    // Allow USB CDC time to enumerate.
    sleep_ms(2000);

    // ADC setup
    adc_init();
    adc_gpio_init(ADC_GPIO);
    adc_select_input(ADC_CHANNEL);

    // Start periodic sampling
    struct repeating_timer sample_timer;

    bool timer_started =
        add_repeating_timer_us(
            -SAMPLE_PERIOD_US,
            sample_timer_callback,
            NULL,
            &sample_timer
        );

    /*
     * Do not print startup text here.
     *
     * From this point forward the USB stream
     * is reserved for binary telemetry packets.
     */

    if (!timer_started) {

        /*
         * Later this will become part of the
         * system fault-handling architecture.
         */
        while (true) {
            tight_loop_contents();
        }
    }

    sample_t sample;

    uint8_t packet[TELEMETRY_PACKET_SIZE];

    while (true) {

        while (sample_buffer_pop(&sample)) {

            uint32_t dropped =
                get_dropped_sample_count();

            build_adc_packet(
                packet,
                &sample,
                dropped
            );

            send_packet(packet);
        }

        tight_loop_contents();
    }
}