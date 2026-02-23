# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

CibleLaser-V1 is an ESP32-C3 embedded firmware for a laser target system. It uses a photoresistor to detect light/laser hits and 5 IR LEDs for a sweeping animation, plus an RGB LED for status feedback. The project prioritizes low power consumption (80 MHz CPU, WiFi/BT disabled, pull-ups disabled).

Written in C++ with the Arduino framework, built via PlatformIO. Comments are in French.

## Build & Upload Commands

```bash
# Build firmware
pio run

# Build and upload to device
pio run -t upload

# Serial monitor (115200 baud)
pio device monitor

# Clean build artifacts
pio run -t clean
```

The board is `esp32-c3-devkitm-1` and the upload port is `/dev/cu.usbmodem1101`. USB CDC is enabled via build flags.

## Architecture

All code lives in `src/main.cpp` (single-file firmware). No custom libraries or headers yet.

**Hardware pins:**
- `A0` — Photoresistor (analog input, light detection)
- GPIO 2, 3, 4 — RGB LED (blue, green, red; inverted PWM: 255 = off)
- GPIO 8, 9, 10, 20, 21 — 5 IR/TIR LEDs (used for sweeping animation)

**Key functions:**
- `setColor(r, g, b)` — RGB LED control with inverted PWM logic
- `animIOSChargingOnce(duration)` — Blocking sweep animation across TIR LEDs using exponential intensity curve
- `animIOSCharging()` — Non-blocking version (call from `loop()`)
- `calculateAverageBrightness()` — Calibrates ambient light level at startup (10 samples)

**Startup flow:** Serial init → CPU to 80 MHz → disable WiFi/BT → init LEDs → init photoresistor → run sweep animation → calibrate brightness. The `loop()` is currently empty.
