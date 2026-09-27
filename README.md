# Pico 2 W Real-Time Embedded Telemetry Controller

A real-time embedded telemetry and signal-processing platform built using the Raspberry Pi Pico 2 W and the Pico SDK.

The project is being developed incrementally to demonstrate embedded firmware architecture, real-time data acquisition, communications, fault handling, testing, and later real-time DSP and digital communications.

## Current Status

### Core Embedded Telemetry System

- [x] Pico 2 W project setup using the Pico SDK
- [x] C firmware build environment
- [x] USB serial console
- [x] Firmware boot and runtime verification
- [x] ADC sensor acquisition
- [x] Timer/interrupt-driven sampling
- [x] Circular buffering
- [x] Binary telemetry packets
- [ ] CRC error detection
- [ ] Python ground station
- [ ] Real-time visualization and logging
- [ ] State-machine firmware architecture
- [ ] Watchdog and fault handling
- [ ] Fault injection and validation
- [ ] Performance measurements

### DSP / Digital Communications Extension

Planned after completion of the core telemetry system:

- Real-time ADC signal acquisition
- FIR digital filtering
- Goertzel / FFT frequency analysis
- Noise-performance testing
- BFSK modulation and demodulation
- Packet and bit error measurements
- Embedded DSP timing and resource analysis

## Hardware

- Raspberry Pi Pico 2 W
- RP2350 microcontroller
- Analog and digital sensors/test inputs added throughout development

## Software

- C / C++
- Raspberry Pi Pico SDK
- CMake / Ninja
- Visual Studio Code
- Python ground-station software

## Milestone V0.5 — Binary Telemetry and Python Receiver

Implemented a binary telemetry protocol and host-side Python decoder.

Verified functionality:

- 18-byte binary ADC telemetry packets
- Packet synchronization using `0xAA 0x55`
- Protocol version and packet type fields
- Packet sequence numbering
- 32-bit ADC sample numbering
- 12-bit ADC values transmitted as 16-bit fields
- Dropped-sample count and status flags
- Explicit little-endian serialization
- Raw binary transmission over USB CDC
- Python ground-station serial receiver
- Stream synchronization and packet reconstruction
- Live ADC and voltage decoding
- Continuous 100 Hz telemetry with zero dropped samples during normal operation

Current firmware version: `0.5.0`

## Project Structure

The repository will expand as additional firmware modules, host software, tests, documentation, and DSP functionality are implemented.