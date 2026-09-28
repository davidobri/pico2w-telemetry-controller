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
- [x] CRC error detection
- [x] Python ground station
- [ ] Configuration/settings
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

## Milestone V0.6 — CRC Error Detection

Added CRC-16/CCITT-FALSE packet integrity checking to the telemetry protocol.

Verified functionality:

- CRC-16/CCITT-FALSE generation on the Pico 2 W
- CRC appended to each binary telemetry packet
- Python ground station independently recalculates CRC
- Valid packets accepted successfully
- Corrupted packets detected and rejected
- Intentional bit-flip fault injection used to validate CRC behavior
- CRC error counter implemented in the ground station
- Continuous 100 Hz telemetry maintained during normal operation

Current firmware version: `0.6.0`

## Project Structure

The repository will expand as additional firmware modules, host software, tests, documentation, and DSP functionality are implemented.