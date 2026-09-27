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

## Milestone V0.2 — ADC Acquisition

Added analog data acquisition using the RP2350 ADC.

Verified functionality:

- ADC0 configured on GPIO26
- 12-bit ADC measurements
- Analog voltage conversion
- Potentiometer used as a controllable test input
- Live ADC values transmitted over USB serial
- Full input range tested on hardware

Current firmware version: `0.2.0`

## Project Structure

The repository will expand as additional firmware modules, host software, tests, documentation, and DSP functionality are implemented.