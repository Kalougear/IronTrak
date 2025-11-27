# IronTrak Software Architecture

**Design principles, subsystem architecture, and implementation details**

---

## 📋 Table of Contents

- [Design Philosophy](#design-philosophy)
- [System Architecture](#system-architecture)
- [Subsystem Details](#subsystem-details)
- [State Machines](#state-machines)
- [Timing & Interrupts](#timing--interrupts)
- [Memory Management](#memory-management)
- [Data Flow](#data-flow)

---

## 🎯 Design Philosophy

### FORTRESS MODE Principles

The IronTrak codebase follows **"Fortress Mode"** - a shop-floor reliability philosophy:

1. **Reliability is Paramount**  
   This code runs on a vibrating bandsaw in a dusty metalworking environment. System crashes = lost measurements = wasted material.

2. **Interrupt-Driven Critical Paths**  
   Encoder tracking uses hardware timers (TIM4). No polling. Zero missed pulses.

3. **Defensive State Management**  
   State transitions are explicit. Accidental button presses during a cut cannot corrupt measurements.

4. **Fail-Safe Defaults**  
   Watchdog timer ensures system recovery from lockups. I2C bus errors trigger reinit, not crashes.

5. **No Over-Engineering**  
   Simple, readable C++. No templates. No dynamic allocation in main loop. Predictable execution.

---

## 🏗️ System Architecture

### High-Level Overview

```
┌────────────────────────────────────────────────────────────┐
│                       main.cpp                             │
│  ┌──────────────────────────────────────────────────────┐ │
│  │         Main Loop (Infinite)                         │ │
│  │  1. Reload Watchdog                                  │ │
│  │  2. Poll UserInput for events                        │ │
│  │  3. Execute State Machine                            │ │
│  │  4. Update subsystems                                │ │
│  └──────────────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────────────┘
                          │
         ┌────────────────┼────────────────┐
         │                │                │
         ▼                ▼                ▼
    ┌─────────┐     ┌───────────┐   ┌──────────┐
    │ Encoder │     │  Display  │   │   Menu   │
    │  Sys    │     │    Sys    │   │   Sys    │
    └─────────┘     └───────────┘   └──────────┘
         │                │                │
         ▼                ▼                ▼
    Hardware         I2C LCD        State Machine
     Timer                          Multi-level Nav
    (TIM4)
```

### Module Dependency Graph

```
main.cpp
  │
  ├─► Config.h (constants, pin definitions, stock libraries)
  │
  ├─► StateMachine.h (SystemState enum)
  │
  ├─► Storage.h (SystemSettings struct)
  │
  ├─► EncoderSys ──► Config.h
  │                  HardwareTimer (TIM4)
  │
  ├─► DisplaySys ──► Config.h
  │                  LiquidCrystal_I2C
  │                  LCDBigNumbers
  │
  ├─► UserInput ──► Config.h
  │                 HardwareTimer (TIM3)
  │
  ├─► MenuSys ──► Config.h, Storage.h, StatsSys, AngleSensor
  │               DisplaySys, EncoderSys
  │
  ├─► StatsSys ──► Storage.h
  │
  └─► AngleSensor ──► Wire (I2C)
```

**Dependency Rules:**
- Low-level modules (EncoderSys, DisplaySys, UserInput) have NO dependencies on high-level modules
- MenuSys is the **only** high-level coordinator (allowed to depend on everything)
- No circular dependencies

---

## 🔧 Subsystem Details

### 1. EncoderSys (Measurement Core)

**File:** `EncoderSys.cpp`, `EncoderSys.h`

**Responsibility:** Hardware encoder interface and distance calculation

**Key Features:**
- TIM4 hardware quadrature capture (no CPU overhead)
- Configurable wheel diameter for mm/pulse conversion
- Offset management for angle correction
- Direction reversal support

**API:**
```cpp
void init();                           // Initialize TIM4 encoder mode
void update();                         // Call in main loop
float getDistanceMM();                 // Current position in mm
void reset();                          // Zero position
void setOffset(float mm);              // Set angle compensation offset
void setWheelDiameter(float mm);       // Calibration
long getRawCount();                    // Direct pulse count
```

**Hardware Configuration:**
```cpp
// TIM4 in Encoder Mode (X4 quadrature)
TIM4->ARR = 0xFFFF;                   // 16-bit counter
HardwareTimer::ENCODER_MODE_TI12      // Both edges, both channels
```

---

### 2. DisplaySys (UI Rendering)

**File:** `DisplaySys.cpp`, `DisplaySys.h`

**Responsibility:** 20x4 LCD management with flicker-free updates

**Key Features:**
- Line-level caching (only redraws changed content)
- Custom character management (8 slots for icons)
- Big number display (LCDBigNumbers integration)
- Auto-ranging unit display (mm → cm → m, in → ft)

**Display Modes:**
```cpp
void showIdle(...)       // Main measurement screen
void showMenu(...)       // Single-line menu display
void showMenu4(...)      // Full 4-line menu
void showHiddenInfo(...) // Diagnostic screen (10s long-press)
void showError(...)      // Error state
```

**Custom Characters (Icons):**
| Slot | Icon | Usage |
|------|------|-------|
| 0-3 | Big Numbers | Large numeral segments (LCDBigNumbers lib) |
| 4 | Circular Saw Blade | Auto-zero, blade thickness settings |
| 5 | Gear | Settings, units, configuration menus |
| 6 | Angle Icon | Cut mode, angle compensation |
| 7 | (Reserved) | Future use |

**Caching Strategy:**
```cpp
String _lastLine[4];  // Track previous content per line
if (newLine != _lastLine[row]) {
    lcd->setCursor(0, row);
    lcd->print(newLine);
    _lastLine[row] = newLine;
}
```

---

### 3. UserInput (Event System)

**File:** `UserInput.cpp`, `UserInput.h`

**Responsibility:** Debounced rotary encoder and button input

**Key Features:**
- 1kHz ISR-driven debouncing (TIM3)
- Long-press detection (500ms, 10s)
- Quadrature decoding for encoder
- Event queue (1-deep buffer)

**Input Events:**
```cpp
enum InputEvent {
    EVENT_NONE,
    EVENT_CW,              // Clockwise rotation
    EVENT_CCW,             // Counter-clockwise
    EVENT_CLICK,           // Button press
    EVENT_LONG_PRESS,      // 500ms (menu entry/back)
    EVENT_SUPER_LONG_PRESS // 10s (hidden diagnostics)
};
```

**Debounce Algorithm:**
```cpp
// 16-bit shift register (1kHz sampling)
_btnState = (_btnState << 1) | digitalRead(PIN_MENU_SW);

// Stable LOW = 16 consecutive zeros
if (_btnState == 0x0000) → Button pressed

// Stable HIGH = 16 consecutive ones
if (_btnState == 0xFFFF) → Button released
```

---

### 4. MenuSys (Navigation & Settings)

**File:** `MenuSys.cpp`, `MenuSys.h`

**Responsibility:** Hierarchical menu system and settings management

**Key Features:**
- 3-level menu depth (main → submenu → sub-submenu)
- Edit mode with value adjustment
- Auto-calibration wizard
- Stock library selection
- Statistics viewing

**Menu Structure:**
```
Main Menu
├── Calibration
│   ├── Auto Calibration Wizard
│   ├── Manual Wheel Diameter Edit
│   └── Reverse Direction Toggle
├── Stock Selection
│   ├── Stock Type (Rect/Angle/Cylinder)
│   ├── Stock Size (dimensional library)
│   └── Face Selection (rectangular only)
├── Angle Mode
│   ├── Cut Mode (0-90°)
│   └── Angle Sensor Calibration (if AS5600 present)
├── Settings
│   ├── Units (Metric/Imperial)
│   ├── Blade Kerf
│   ├── Auto-Zero Enable
│   ├── Auto-Zero Threshold
│   └── Hourly Rate
└── Statistics
    ├── Project Stats (cuts, length, time, cost)
    ├── Total Stats (lifetime)
    └── Reset Project
```

**State Machine:**
```cpp
enum MenuLevel {
    LEVEL_MAIN,        // Top-level selection
    LEVEL_SUBMENU,     // Secondary level
    LEVEL_SUB_SUBMENU, // Tertiary level
    LEVEL_EDIT         // Value editing mode
};
```

---

### 5. StatsSys (Data Tracking)

**File:** `StatsSys.cpp`, `StatsSys.h`

**Responsibility:** Cut statistics and cost tracking

**Tracked Metrics:**
- Total cuts (lifetime)
- Total length (meters)
- Total time (seconds)
- Project cuts (resettable)
- Project length
- Project time
- Calculated cost (based on hourly rate)

**API:**
```cpp
void init(SystemSettings* s);
void update();                        // Call in main loop (time tracking)
void registerCut(float mm);           // Record a cut
void resetProject();                  // Clear project stats
```

---

### 6. AngleSensor (Optional Hardware)

**File:** `AngleSensor.cpp`, `AngleSensor.h`

**Responsibility:** AS5600 magnetic encoder interface

**Key Features:**
- I2C communication (address 0x36)
- 12-bit absolute angle reading (0-360°)
- Automatic fallback if sensor not detected

**API:**
```cpp
bool init();                  // Detect sensor, return true if found
float getAngleDegrees();      // Current angle (0.0 - 360.0)
uint16_t getRawAngle();       // Raw 12-bit value (0-4095)
```

---

## 🔄 State Machines

### Top-Level System State

```cpp
enum SystemState {
    STATE_IDLE,    // Normal operation (measurement display)
    STATE_MENU,    // Menu navigation active
    // STATE_MEASURING, STATE_CALIBRATION, STATE_ERROR (unused)
};
```

**State Transitions:**
```
STATE_IDLE
  │
  ├─► EVENT_LONG_PRESS ──► STATE_MENU
  │
  ├─► EVENT_CLICK ──► Register cut, zero encoder
  │
  ├─► Double-click ──► Toggle straight/angle mode
  │
  └─► EVENT_SUPER_LONG_PRESS ──► Show hidden diagnostics (stays in IDLE)

STATE_MENU
  │
  ├─► MenuSys.update() returns false ──► STATE_IDLE
  │
  └─► Navigate, edit, save settings
```

---

### Auto-Zero State Machine

**Activated When:**
- `settings.autoZeroEnabled == true`
- `settings.cutMode == 0` (straight cuts only)
- NOT in hidden diagnostics mode

**States:**
```cpp
enum AutoZeroState {
    AZ_DISABLED,   // Feature off or conditions not met
    AZ_MEASURING,  // Monitoring for stillness
    AZ_ARMED       // Ready to auto-zero on movement
};
```

**Logic:**
```
AZ_MEASURING
  │
  ├─► Position stable for 5 seconds ──► AZ_ARMED
  │
  └─► Position changes ──► Stay in AZ_MEASURING

AZ_ARMED
  │
  ├─► Position delta > threshold ──► Register cut, zero, goto AZ_MEASURING
  │
  └─► Position stable ──► Stay in AZ_ARMED
```

---

## ⏱️ Timing & Interrupts

### Interrupt Priority

| Source | Frequency | Priority | Purpose |
|--------|-----------|----------|---------|
| **TIM4 Encoder** | Hardware | Highest | Quadrature capture (no software ISR) |
| **TIM3 1kHz** | 1000 Hz | High | UserInput debouncing |
| **Watchdog** | 2s timeout | Critical | System reset on lockup |

### Main Loop Timing

**Target:** <10ms per iteration (no blocking calls)

**Loop Structure:**
```cpp
void loop() {
    IWatchdog.reload();                   // ~10μs
    
    InputEvent event = userInput.getEvent(); // ~50μs
    
    switch (currentState) {               // ~1-5ms
        case STATE_IDLE: ...
        case STATE_MENU: ...
    }
    
    encoderSys.update();                  // ~100μs
    displaySys.update();                  // ~500μs - 2ms (I2C)
    statsSys.update();                    // ~50μs
    
    // Total: ~2-8ms typical
}
```

---

## 💾 Memory Management

### RAM Usage

**Static Allocation Only** (no `malloc`/`new` in main loop)

| Component | Estimated Size |
|-----------|---------------|
| SystemSettings struct | ~100 bytes |
| LiquidCrystal_I2C | ~200 bytes |
| LCDBigNumbers | ~150 bytes |
| String caching (DisplaySys) | ~500 bytes |
| Menu system strings | ~300 bytes |
| Stack (ISR + main) | ~2KB |
| **Total** | **~3.5KB / 128KB** |

**Plenty of headroom for persistence implementation.**

---

### Flash Usage

| Component | Size |
|-----------|------|
| Core code | ~60KB |
| Arduino framework | ~30KB |
| Libraries (LCD, etc.) | ~20KB |
| Stock library data | ~2KB |
| **Total** | **~112KB / 512KB** |

---

## 📊 Data Flow

### Measurement Path (Critical)

```
Hardware Encoder
  │ (A/B quadrature)
  ▼
TIM4 Hardware Counter
  │ (no CPU)
  ▼
EncoderSys.update()
  │ reads TIM4->CNT
  ▼
Calculate: count × (wheelDia × π / 4096) + offset
  │
  ▼
Main Loop: getDistanceMM()
  │
  ▼
DisplaySys.showIdle()
  │
  ▼
20x4 LCD Display
```

**Zero CPU overhead until `getDistanceMM()` is called.**

---

### Menu Navigation Path

```
KY-040 Encoder
  │ (CLK/DT/SW)
  ▼
TIM3 ISR (1kHz)
  │ debounce
  ▼
UserInput.getEvent()
  │ EVENT_CW/CCW/CLICK/LONG_PRESS
  ▼
toSemanticEvent()
  │ (translate to EVENT_NEXT/PREV)
  ▼
MenuSys.update(event)
  │ state machine
  ▼
Update settings, DisplaySys.showMenu()
  │
  ▼
Return false → Exit to STATE_IDLE
```

---

## 🔐 Safety Mechanisms

### 1. Watchdog Timer

```cpp
IWatchdog.begin(2000000);  // 2-second timeout

void loop() {
    IWatchdog.reload();   // Must call every <2s
    // ...
}
```

**Failure Mode:** System locks up → Watchdog resets MCU → Measurement lost but system recovers

---

### 2. I2C Error Handling

**Current:** Not implemented (relies on Wire library robustness)

**Future:** Add bus health checks
