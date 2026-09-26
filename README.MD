# Home Automation & Security System

A simulated home safety and automation architecture built with Arduino and Proteus. The system integrates multi-sensor hazard detection (fire, gas, intrusion) with automated physical responses (door/window actuation) and secure keypad access control.

![System Schematic](./assets/arduino-home-automation.png)

## Core Features

* **Access Control:** 4x3 matrix keypad authentication with configurable PIN (Default: `1234`).
* **Fire Emergency Protocol:** Flame detection overrides security, automatically opening all doors and windows to facilitate evacuation.
* **Gas Leak Mitigation:** Gas detection triggers window actuation for room ventilation while keeping exterior doors secured.
* **Intrusion Detection:** PIR motion tracking armed post-entry timeout; triggers visual alerts on OLED and warning LEDs.
* **Real-Time Telemetry:** I2C OLED (128x64) provides live monitoring of temperature, sensor states, and keypad inputs.

## Hardware & Software Stack

| Category | Components |
| :--- | :--- |
| **Simulation** | Proteus Design Suite |
| **Controller** | Arduino Uno (ATmega328P) |
| **Language** | Embedded C++ |
| **Sensors** | PIR Motion, Flame, LM35 (Temp), Gas (MQ-Series) |
| **Actuation** | L293D H-Bridge, DC Motors (Door/Window) |
| **Display** | SSD1306 I2C OLED |

## System Logic

1. **Boot Sequence:** 1.5s initialization phase displaying system status on OLED.
2. **Armed State:** If PIR detects motion without prior keypad authorization within the 10-second entry window, an intrusion alert is triggered.
3. **Hazard Overrides:**
   * `Flame == HIGH` ➔ Open Doors + Open Windows + Fire LED
   * `Gas == HIGH` ➔ Open Windows + Warning LED
4. **Hardware Stability:** Pull-down resistors (10kΩ) are implemented on digital sensor outputs to prevent floating logic and EMI false-triggering in simulation.
