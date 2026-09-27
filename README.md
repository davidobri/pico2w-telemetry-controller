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
- [ ] Binary telemetry packets
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

## Milestone V0.4 — Circular Buffering

Implemented a circular sample buffer between the timer-driven ADC producer and main-loop consumer.

Verified functionality:

- 128-entry circular sample buffer
- Producer/consumer separation
- Head/tail index management
- Buffer full/empty detection
- Dropped-sample tracking
- Continuous 100 Hz ADC acquisition
- Zero dropped samples during normal operation
- Intentional consumer slowdown used to force buffer saturation
- Full-buffer condition correctly detected and dropped samples counted

Current firmware version: `0.4.0`

## Project Structure

The repository will expand as additional firmware modules, host software, tests, documentation, and DSP functionality are implemented.