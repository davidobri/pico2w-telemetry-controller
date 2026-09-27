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
- [ ] Timer/interrupt-driven sampling
- [ ] Circular buffering
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

## Milestone V0.3 — Deterministic Timer-Driven Sampling

Implemented fixed-rate ADC sampling using a repeating timer.

Verified functionality:

- 100 Hz ADC sampling
- Timer-driven acquisition
- Separation of sampling from USB output
- Shared-state synchronization between timer callback and main loop
- Sample counter
- Basic overrun detection
- Zero overruns during normal testing

Current firmware version: `0.3.0`

## Project Structure

The repository will expand as additional firmware modules, host software, tests, documentation, and DSP functionality are implemented.