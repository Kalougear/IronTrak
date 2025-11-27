# IronTrak - Precision Bandsaw Fence Position Tracker

**Professional-grade linear measurement system for metalworking bandsaws**

[![Platform](https://img.shields.io/badge/Platform-STM32F411-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f411.html)
[![Framework](https://img.shields.io/badge/Framework-Arduino-00979D.svg)](https://www.arduino.cc/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Quick Start](#quick-start)
- [Documentation Index](#documentation-index)
- [Project Status](#project-status)
- [Contributing](#contributing)

---

## 🎯 Overview

**IronTrak** is a precision measurement tool designed for machine shop environments. It mounts to a bandsaw fence and tracks cut positions with sub-millimeter accuracy using an industrial optical encoder.

**Use Case:**  
When cutting multiple pieces of stock material to identical lengths, IronTrak displays real-time fence position, compensates for blade angles, tracks cutting statistics, and enables repeatable measurements.

**Environment:**  
Designed for harsh shop conditions - vibration, metal dust, intermittent power, and operator-with-greasy-hands reliability.

---

## ✨ Features

### Core Functionality
- **High-Precision Tracking:** 1024 PPR optical encoder (0.038mm resolution after quadrature)
- **Real-Time Display:** 20x4 LCD with large custom numerals
- **Unit Support:** Metric (mm/cm/m) and Imperial (inches/feet) with auto-ranging
- **Angle Compensation:** Automatic offset calculation for 0-90° cuts
- **Auto-Zero:** Intelligent automatic measurement reset after stillness detection

### Advanced Features
- **Angle Sensor Integration:** Optional AS5600 magnetic encoder for automatic angle detection
- **Statistics Tracking:** Total cuts, total length, per-project tracking, time tracking, cost calculation
- **Stock Library:** Pre-defined dimensional profiles for rectangular, angle iron, and cylindrical stock
- **Menu System:** Hierarchical settings interface with rotary encoder navigation
- **Fail-Safe Design:** Hardware watchdog, interrupt-driven encoder capture, debounced inputs

### Display Modes
- **Idle Screen:** Current measurement, target dimension, stock info, cut angle indicator
- **Menu System:** Calibration wizard, stock selection, angle mode, statistics, settings
- **Hidden Info:** Long-press diagnostic screen showing kerf, wheel diameter, system status

---

## 🔧 Hardware Requirements

### Core Components
| Component | Specification | Purpose |
|-----------|---------------|---------|
| **MCU** | STM32F411CE Black Pill | 100MHz ARM Cortex-M4, 128KB RAM, 512KB Flash |
| **Encoder** | 1024 PPR Optical Rotary | Industrial-grade quadrature encoder (NPN/PNP) |
| **Display** | 20x4 I2C LCD (0x27) | Character display with backlight |
| **Menu Input** | KY-040 Rotary Encoder | Push-button rotary encoder for navigation |
| **Power** | USB Power Bank (5V) | Portable 5V power source |

### Optional Components
| Component | Specification | Purpose |
|-----------|---------------|---------|
| **Angle Sensor** | AS5600 Magnetic Encoder | 12-bit absolute angle measurement (I2C 0x36) |

### Mechanical Parts
- **Mounting:** Custom 3D-printed enclosure (STL files in `/hardware` - future)
- **Drive Coupling:** M8 axle, 608ZZ bearings, flexible spider coupling
- **Attachment:** Mounts to bandsaw fence rail

### Wiring
See [HARDWARE.md](HARDWARE.md) for complete pinout and wiring diagrams.

---

## 🚀 Quick Start

### 1. Hardware Assembly
```
1. Mount encoder to bandsaw fence using 3D-printed bracket
2. Connect encoder wheel to fence drive mechanism via spider coupling
3. Wire components per HARDWARE.md pinout table
4. Connect USB power bank
```

### 2. Software Flash
```bash
# Clone repository
git clone https://github.com/yourusername/IronTrak.git
cd IronTrak

# Install PlatformIO if not already installed
# https://platformio.org/install

# Compile and upload (ST-Link programmer)
pio run --target upload

# OR use DFU mode (USB programming)
# 1. Set BOOT0 jumper to 1
# 2. Press reset
pio run --target upload --upload-port /dev/ttyACM0
# 3. Set BOOT0 back to 0
# 4. Press reset
```

### 3. Initial Calibration
```
1. Power on device (3 LED blinks = boot OK)
2. Long-press menu button (500ms)
3. Navigate to "Calibration" → "Auto Calibration Wizard"
4. Follow on-screen instructions:
   - Measure 100mm of fence travel
   - System calculates wheel diameter
   - Confirm and save
```

### 4. First Cut
```
1. Zero measurement (single click)
2. Move fence to desired position
3. Make cut
4. Click button to register cut and auto-zero
```

---

## 📚 Documentation Index

### For Users
- **[CALIBRATION.md](CALIBRATION.md)** - Setup, calibration procedures, and accuracy tuning
- **[USER_GUIDE.md](USER_GUIDE.md)** - Operation, menu navigation, cutting workflows *(future)*
- **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** - Common issues and solutions *(future)*

### For Builders
- **[HARDWARE.md](HARDWARE.md)** - Complete hardware specifications, pinout, wiring diagrams
- **[BOM.md](docs/BOM.md)** - Bill of materials with part numbers and sources *(existing)*
- **[BUILD_GUIDE.md](BUILD_GUIDE.md)** - Step-by-step assembly instructions *(future)*

### For Developers
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Software design, subsystem architecture, state machines
- **[API_REFERENCE.md](API_REFERENCE.md)** - Class documentation, method signatures, usage examples
- **[DEVELOPMENT.md](DEVELOPMENT.md)** - Dev environment setup, coding standards, contribution guide *(future)*

### Reference
- **[SETTINGS_REFERENCE.md](SETTINGS_REFERENCE.md)** - Complete settings parameter documentation *(future)*
- **[CHANGELOG.md](CHANGELOG.md)** - Version history and release notes *(future)*

---

## 📊 Project Status

### Current Version: **v1.0.0-RC** (Release Candidate)

| Feature | Status | Notes |
|---------|--------|-------|
| Core Measurement | ✅ Production | Fully tested, sub-mm accuracy |
| Display System | ✅ Production | 20x4 LCD with custom icons |
| Menu System | ✅ Production | Multi-level navigation |
| Auto-Zero | ✅ Production | Intelligent stillness detection |
| Angle Compensation | ✅ Production | Manual & sensor-based |
| Statistics | ✅ Production | Cut tracking, time, cost |
| Angle Sensor | ✅ Production | AS5600 optional integration |
| **Settings Persistence** | ⚠️ **Not Implemented** | Settings lost on power cycle |
| Hardware Watchdog | ✅ Production | 2-second timeout |
| I2C Communication | ✅ Production | Robust multi-device bus |

### Known Limitations
1. **No EEPROM/Flash Persistence:** Settings revert to defaults on reboot (wheel diameter, stats, preferences)
2. **No External Documentation Yet:** Assembly/build guides pending
3. **No Case STL Files:** 3D-printable enclosure design in progress

---

## 🛠️ Technology Stack

- **Platform:** STM32F411CE (100MHz ARM Cortex-M4)
- **Framework:** Arduino (PlatformIO)
- **Key Libraries:**
  - `LiquidCrystal_I2C` (fdebrabander) - LCD driver
  - `LCDBigNumbers` (ArminJo) - Large numeral display
  - STM32 `HardwareTimer` - Encoder capture & 1kHz tick
  - `IWatchdog` - Hardware watchdog timer
- **Toolchain:** PlatformIO, ST-Link V3 programmer
- **Language:** C++17

---

## 🏗️ Architecture Highlights

### Hardware Timers
- **TIM4:** Dedicated encoder quadrature capture (no CPU overhead)
- **TIM3:** 1kHz interrupt for menu encoder debouncing
- **Watchdog:** 2-second hardware timeout for crash recovery

### Software Modules
```
main.cpp
├── EncoderSys    → Hardware encoder interface (TIM4)
├── DisplaySys    → 20x4 LCD driver with caching
├── UserInput     → KY-040 rotary encoder with debouncing
├── MenuSys       → Hierarchical menu state machine
├── StatsSys      → Cut statistics and time tracking
├── AngleSensor   → Optional AS5600 magnetic encoder
└── Storage       → Settings structure (persistence TODO)
```

---

## 🤝 Contributing

This is a personal shop tool project, but contributions are welcome!

**Before contributing:**
1. Read [ARCHITECTURE.md](ARCHITECTURE.md) to understand the design
2. Review [GEMINI.md](GEMINI.md) for coding standards and project philosophy
3. Follow **Protocol Zero:** Discuss → Propose → Code (no surprise PRs)

**Priority Areas for Contribution:**
- Settings persistence implementation (EEPROM/Flash)
- 3D-printable enclosure design
- Assembly documentation with photos
- User manual with cutting workflow examples

---

## 📄 License

MIT License - See [LICENSE](LICENSE) for details

---

## 🙏 Acknowledgments

- **Paul Stoffregen** - Original Encoder library inspiration
- **fdebrabander** - LiquidCrystal_I2C library
- **ArminJo** - LCDBigNumbers library
- **STMicroelectronics** - STM32 HAL and ecosystem

---

## 📞 Support

- **Issues:** [GitHub Issues](https://github.com/yourusername/IronTrak/issues)
- **Documentation:** See [Documentation Index](#documentation-index)
- **Contact:** [Your contact method]

---

**Built with 🔧 for the shop, by the shop.**
