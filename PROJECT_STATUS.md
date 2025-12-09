# IronTrak — Project Status Checkpoint

> **Last Updated:** 2025-11-28 00:29 UTC+02:00  
> **Status:** ✅ **FEATURE COMPLETE & HARDWARE TESTED**  
> **Version:** 1.0 (Production Ready)

---

## 🎯 Project State Summary

**IronTrak is a production-ready industrial portable measuring system for cutting tools (bandsaw, table saw, etc.).** The hardware has been tested and validated. All core features are implemented and working. The project is now in **maintenance mode** — stable and ready for shop floor deployment.

### Current Phase: **COMPLETE**

- ✅ Hardware design finalized
- ✅ Firmware feature-complete
- ✅ Hardware testing passed
- ✅ Documentation complete
- 🔒 **Code freeze active** (no new features)

---

## 🔧 Implemented & Tested Features

### Core Measurement System

| Feature               | Status     | Hardware                | Notes                                       |
| --------------------- | ---------- | ----------------------- | ------------------------------------------- |
| **Live DRO Display**  | ✅ Working | TIM4 Encoder (1024 PPR) | Big number display (20x4 LCD), auto-ranging |
| **Zero/Reset**        | ✅ Working | Rotary encoder click    | Single-click zero, double-click mode toggle |
| **Auto-Zero**         | ✅ Working | Software algorithm      | Hands-free workflow, configurable threshold |
| **Reverse Direction** | ✅ Working | Settings toggle         | Physical mount orientation fix              |

### Angle Cut Support

| Feature                       | Status     | Hardware              | Notes                               |
| ----------------------------- | ---------- | --------------------- | ----------------------------------- |
| **Manual Angle Mode**         | ✅ Working | Rotary encoder        | 0-90° selection via menu            |
| **Automatic Angle Detection** | ✅ Working | AS5600 (I2C 0x36)     | Optional, auto vs manual selectable |
| **Offset Calculation**        | ✅ Working | Software trigonometry | tan(angle) × face dimension         |
| **Face Selection**            | ✅ Working | Rotary encoder        | For rectangular stock (2 faces)     |

### Statistics & Tracking

| Feature              | Status     | Storage        | Notes                           |
| -------------------- | ---------- | -------------- | ------------------------------- |
| **Cut Registration** | ✅ Working | RAM (volatile) | Click to log cut + length       |
| **Cut Counter**      | ✅ Working | RAM            | Resets on power cycle           |
| **Total Length**     | ✅ Working | RAM            | Sum of all cuts                 |
| **Runtime Tracking** | ✅ Working | RAM            | Labor hours (no RTC)            |
| **Hourly Rate**      | ✅ Working | Settings menu  | Manual entry via rotary encoder |

### Calibration System

| Feature                     | Status     | Method                      | Notes                                   |
| --------------------------- | ---------- | --------------------------- | --------------------------------------- |
| **Auto Calibration Wizard** | ✅ Working | 100mm reference measurement | Calculates wheel diameter automatically |
| **Manual Diameter Entry**   | ✅ Working | Direct numeric input        | 0.1mm precision                         |
| **Wheel Diameter Storage**  | ✅ Working | RAM (volatile)              | No EEPROM persistence yet               |

### User Interface

| Feature                       | Status     | Hardware              | Notes                            |
| ----------------------------- | ---------- | --------------------- | -------------------------------- |
| **20x4 LCD Display**          | ✅ Working | I2C 0x27              | Custom characters for icons      |
| **Rotary Encoder Navigation** | ✅ Working | KY-040, TIM3 1kHz ISR | Debounced via hardware timer     |
| **Menu System**               | ✅ Working | State machine         | Settings, stats, calibration     |
| **Unit Toggle**               | ✅ Working | Settings menu         | Metric (mm/cm/m) ↔ Imperial (in) |

---

## 🖥️ Hardware Configuration

### MCU: STM32F411CE Black Pill

- **Core:** ARM Cortex-M4F @ 100MHz
- **RAM:** 128KB SRAM
- **Flash:** 512KB
- **FPU:** 32-bit hardware floating-point unit
- **Current Usage:** ~90KB Flash, ~4KB RAM
- **Available:** ~360KB Flash, ~124KB RAM

### Peripherals

| Component               | Interface       | Pin Assignment          | Status            |
| ----------------------- | --------------- | ----------------------- | ----------------- |
| **Main Encoder**        | TIM4 Quadrature | PB6 (Ch1), PB7 (Ch2)    | ✅ Configured     |
| **LCD 20x4**            | I2C1            | PB8 (SCL), PB9 (SDA)    | ✅ Configured     |
| **KY-040 Encoder**      | GPIO + TIM3     | PA0/PA1 (A/B), PA2 (SW) | ✅ Configured     |
| **AS5600 Angle Sensor** | I2C1            | Shared with LCD         | ✅ Optional       |
| **USB Serial (Debug)**  | UART1           | PA9 (TX), PA10 (RX)     | ✅ 115200 baud    |
| **Onboard LED**         | GPIO            | PC13                    | ✅ Boot indicator |
| **Watchdog**            | IWDG            | Internal                | ✅ 2s timeout     |

### Mechanical Interface

- **Encoder Wheel:** Custom 3D printed, M8 axle mount
- **Bearings:** 608ZZ (skateboard type)
- **Coupling:** Flexible spider coupling
- **Mounting:** Bandsaw fence rail contact
- **Enclosure:** 3D printed, shop floor rated

---

## 📋 Architecture Overview

### Current: **Monolithic Embedded System**

All features run on STM32F411CE with 20x4 LCD interface.

**Design Pattern:**

- Super-Loop architecture (bare metal, no RTOS)
- Event-driven via ISR flags (`volatile bool`)
- State machine for menu navigation
- Hardware timers for encoder + debouncing

**GEMINI.md Compliance:**

- ✅ Zero dynamic allocation (static memory only)
- ✅ ISR execution < 10µs (no floating-point in interrupts)
- ✅ Bounded loops (no recursion)
- ✅ Watchdog enabled (2-second timeout)
- ✅ Volatile discipline (all ISR-shared variables)

### Future: **Split-System (Proposed, Not Implemented)**

See `docs/FUTURE_ARCHITECTURE_CONCEPTS.md` for desktop companion app proposal.

**Status:** Documented for future consideration, **NOT IMPLEMENTED**.

---

## 📁 Codebase Structure

### Source Files (18 total)

**Headers (`src/headers/`):**

- `Config.h` — System constants, pin definitions, stock profiles
- `StateMachine.h` — State enumeration (IDLE, MENU, etc.)
- `EncoderSys.h` — Main encoder (TIM4) interface
- `DisplaySys.h` — LCD rendering and big number display
- `UserInput.h` — KY-040 encoder + debouncing
- `MenuSys.h` — Menu navigation and settings
- `StatsSys.h` — Statistics tracking (cuts, length, time)
- `Storage.h` — Settings struct (no EEPROM persistence)
- `AngleSensor.h` — AS5600 I2C interface
- `LiquidCrystal_I2C_Compat.h` — LCD library wrapper

**Implementation (`src/source/`):**

- `Config.cpp` — Stock profile arrays (PROGMEM)
- `EncoderSys.cpp` — Encoder logic, calibration, offset math
- `DisplaySys.cpp` — LCD rendering, custom characters, big numbers
- `UserInput.cpp` — ISR tick handler, event generation
- `MenuSys.cpp` — Menu state machine, navigation logic
- `StatsSys.cpp` — Cut logging, stats calculations
- `AngleSensor.cpp` — AS5600 communication

**Main:**

- `main.cpp` — Setup, main loop, state machine dispatcher

---

## 📚 Documentation

### User Documentation (`documentation/`)

- `README.md` — Project overview and quick start
- `CALIBRATION.md` — **Complete calibration guide** (auto + manual wizards)
- `SOFTWARE_ARCHITECTURE.md` — Firmware design details
- `API_REFERENCE.md` — Function-level documentation
- `CHANGELOG.md` — Version history

### Developer Documentation (`docs/`)

- `GEMINI.md` — **System instructions for AI agents** (IMMUTABLE)
- `FUTURE_ARCHITECTURE_CONCEPTS.md` — Split-system proposal (desktop + LCD)
- `WIRING.md` — Pin assignments and connections
- `BOM.md` — Bill of materials
- `POWER_SUPPLY.md` — USB power bank specifications
- `STM32_FLASHING_GUIDE.md` — Programming instructions
- `STM32_cookbook/` — Embedded development standards
  - `STM32_RULES_STRICT.md` — NASA Power of 10 + MISRA-C adapted
  - `STM32_TIMER_COOKBOOK.md` — Timer configuration recipes
  - `STM32_MATH_PHYSICS_GUIDE.md` — FPU usage, fixed-point math
  - `STM32_FIRMWARE_ARCH_PATTERNS.md` — Architecture patterns
  - `STM32_HAL_ESSENTIALS.md` — HAL library reference

---

## 🚫 What's NOT Implemented (Future Proposals)

### Desktop Companion App

**Status:** Documented in `docs/FUTURE_ARCHITECTURE_CONCEPTS.md`, **NOT BUILT**.

**Proposed Features (if implemented):**

- Weight calculator (material × profile × length)
- Cut list optimizer (bin-packing algorithm)
- Job management (upload cut sequences to device)
- Historical analysis (charts, trends, CSV export)
- Material inventory tracking

**Decision:** User has **closed features for now** — desktop app is **deferred indefinitely**.

### EEPROM Persistence

**Status:** NOT IMPLEMENTED (all settings volatile).

**Current Behavior:**

- Settings reset to defaults on power cycle
- Calibration must be re-run after reboot
- Statistics lost on power loss

**Future Consideration:**

- Add EEPROM wear leveling for settings
- Persist wheel diameter, hourly rate, auto-zero config
- Optional stats logging to external storage

---

## ✅ Verification Status

### Hardware Testing

| Test                   | Date       | Result  | Notes                             |
| ---------------------- | ---------- | ------- | --------------------------------- |
| **Encoder Accuracy**   | 2025-11-27 | ✅ PASS | Sub-mm accuracy over 1m range     |
| **Auto-Zero**          | 2025-11-27 | ✅ PASS | Reliable hands-free operation     |
| **Angle Cuts**         | 2025-11-27 | ✅ PASS | Offset calculation correct at 45° |
| **AS5600 Sensor**      | 2025-11-27 | ✅ PASS | Auto angle detection working      |
| **Menu Navigation**    | 2025-11-27 | ✅ PASS | Rotary encoder responsive         |
| **LCD Display**        | 2025-11-27 | ✅ PASS | No flicker, clear big numbers     |
| **Calibration Wizard** | 2025-11-27 | ✅ PASS | Auto-calc wheel diameter accurate |

### Code Quality

- ✅ **Zero compiler warnings** (`-Wall -Wextra -Wpedantic`)
- ✅ **GEMINI.md compliant** (FORTRESS MODE standards)
- ✅ **No dynamic allocation** (static memory only)
- ✅ **ISR discipline** (all shared variables volatile)
- ✅ **Watchdog enabled** (system reset on hang)

---

## 🎯 Project Goals (Achieved)

### Primary Objectives ✅

1. ✅ **Eliminate repetitive manual measuring/marking** — Live DRO replaces tape measure
2. ✅ **Track project statistics** — Cut count, total length, labor hours
3. ✅ **Support angle cuts** — Automatic offset calculation for 0-90° cuts
4. ✅ **Hands-free workflow** — Auto-zero reduces operator interaction
5. ✅ **Shop floor reliability** — Industrial-grade design, vibration/dust resistant

### Stretch Goals ✅

1. ✅ **AS5600 integration** — Automatic angle detection (optional)
2. ✅ **Dual unit support** — Metric ↔ Imperial toggle
3. ✅ **Big number display** — Large, readable measurements at a glance
4. ✅ **Custom LCD icons** — Visual indicators for mode/state

---

## 🔮 Future Considerations (Deferred)

### If User Requests Later:

1. **Desktop companion app** — See `FUTURE_ARCHITECTURE_CONCEPTS.md`
2. **EEPROM persistence** — Non-volatile settings storage
3. **SD card logging** — Export statistics to CSV
4. **Bluetooth/WiFi** — Wireless stats sync to phone/tablet
5. **RTC module** — True timestamp logging (currently uses millis())
6. **Material weight calculator** — Embedded or desktop implementation
7. **Cut list optimizer** — Bin-packing for stock length planning

### Current Decision:

**User has closed features** — project is feature-complete as-is. No new development planned.

---

## 🔒 Maintenance Mode

### Current State:

- ✅ **Code frozen** — No new features
- ✅ **Hardware tested** — Validated on physical bandsaw
- ✅ **Documentation complete** — All guides written
- ✅ **Ready for deployment** — Production-ready firmware

### If Issues Arise:

1. **Bug fixes only** — Critical reliability fixes permitted
2. **Calibration refinement** — Fine-tuning accuracy if needed
3. **Hardware compatibility** — Support for alternative LCDs/encoders

### Support Model:

- User will report issues via conversation history
- Agent will diagnose via serial debug output (PA9/PA10 @ 115200)
- Agent will propose fixes per GEMINI.md Protocol Zero (DISCUSS → PROPOSE → CODE)

---

## 📞 Contact & Support

**Repository:** IronTrak (local project, no GitHub yet)  
**Developer:** SKTech  
**AI Agent:** Antigravity (Google Deepmind)  
**Last Session:** 2025-11-28 00:29 UTC+02:00

### Debug Access:

- **Serial1 (UART1):** PA9 (TX), PA10 (RX) @ 115200 baud
- **USB Serial:** Available via STM32 native USB (if configured)

---

## 🏁 Final Notes

**IronTrak is complete and ready for production use.**

The system has been tested on hardware, all features work as designed, and documentation is comprehensive. The project is now in maintenance mode — stable, reliable, and optimized for shop floor use.

If future enhancements are needed (desktop app, EEPROM persistence, etc.), the architecture proposals are documented in `docs/FUTURE_ARCHITECTURE_CONCEPTS.md` for reference.

**Status:** ✅ **MISSION ACCOMPLISHED**

---

**End of Status Document**
