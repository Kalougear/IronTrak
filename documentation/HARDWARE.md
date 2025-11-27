# IronTrak Hardware Documentation

**Complete hardware specifications, pinout, and wiring guide**

---

## 📋 Table of Contents

- [System Overview](#system-overview)
- [Component Specifications](#component-specifications)
- [Pin Assignments](#pin-assignments)
- [Wiring Diagrams](#wiring-diagrams)
- [Power Requirements](#power-requirements)
- [Mechanical Assembly](#mechanical-assembly)
- [Troubleshooting](#troubleshooting)

---

## 🔌 System Overview

### Block Diagram

```
┌─────────────────┐
│  USB Power Bank │  5V
│     (5V/2A)     │
└────────┬────────┘
         │
         ├──────────────────────┐
         │                      │
         ▼                      ▼
┌─────────────────┐     ┌──────────────┐
│  STM32F411CE    │◄───►│ 20x4 LCD I2C │
│   Black Pill    │     │   (0x27)     │
│                 │     └──────────────┘
│  TIM4: Encoder  │◄────┐
│  TIM3: 1kHz ISR │     │
│  I2C1: Display  │     │  ┌──────────────────┐
│  I2C1: Sensor   │     └──│ 1024PPR Optical  │
│  GPIO: Input    │◄───────│ Quadrature       │
└─────────────────┘        │ Encoder          │
         │                 └──────────────────┘
         │
         ▼
┌─────────────────┐        ┌──────────────────┐
│   KY-040 Rot.   │◄──────►│   AS5600 Mag.    │
│   Encoder       │        │   Angle Sensor   │
│   (Menu Input)  │        │   (Optional)     │
└─────────────────┘        └──────────────────┘
```

---

## 🛠️ Component Specifications

### 1. Microcontroller: STM32F411CE Black Pill

**Specifications:**
- **CPU:** ARM Cortex-M4F @ 100MHz
- **Flash:** 512KB
- **RAM:** 128KB
- **Peripherals:** TIM1-5, I2C1-3, USART1-2, USB OTG
- **Operating Voltage:** 3.3V (5V tolerant I/O)
- **Dimensions:** 53mm × 23mm

**Programming:**
- **ST-Link V3:** SWD interface (SWDIO/SWCLK/GND/3.3V)
- **DFU Mode:** USB programming (BOOT0 jumper to 1)

**Resources:**
- [STM32F411 Datasheet](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)
- [Black Pill Pinout](https://stm32-base.org/boards/STM32F411CEU6-Black-Pill)

---

### 2. Industrial Optical Encoder

**Specifications:**
- **Model:** 1024 PPR Optical Quadrature Encoder
- **Output Type:** NPN Open Collector / PNP (voltage select)
- **Supply Voltage:** 5-24V DC
- **Output Signals:** A, B (quadrature), Z (index - not used)
- **Resolution:** 4096 pulses/rev (after quadrature decoding)
- **Shaft Diameter:** 6mm
- **Operating Temp:** -10°C to 70°C

**Quadrature Decoding:**
```
1024 PPR × 4 (edges) = 4096 counts/revolution
Wheel circumference: π × 50mm = 157.08mm
Resolution: 157.08mm / 4096 = 0.038mm per pulse
```

**Wiring (NPN Mode):**
```
Brown  → +12V (or +5V)
Blue   → GND
Black  → Channel A (to PB6 via pull-up)
White  → Channel B (to PB7 via pull-up)
```

**Pull-Up Resistors Required:** 4.7kΩ - 10kΩ to 3.3V (on STM32 side)

---

### 3. Display: 20x4 LCD with I2C Backpack

**Specifications:**
- **Type:** HD44780-compatible character LCD
- **Size:** 20 columns × 4 rows
- **I2C Address:** 0x27 (default, may be 0x3F on some modules)
- **Backlight:** Integrated blue LED
- **Supply Voltage:** 5V
- **Logic Levels:** 5V (direct STM32 connection OK - STM32 is 5V tolerant)

**I2C Backpack:**
- **Chip:** PCF8574T I2C I/O expander
- **Speed:** Standard mode (100kHz) or Fast mode (400kHz)
- **Address Select:** Solder jumpers A0-A2 (default all open = 0x27)

**Custom Characters:**
- **Capacity:** 8 custom 5×8 pixel characters
- **Usage:** Icons for stock types, angles, statistics (see DisplaySys.cpp)

---

### 4. Menu Input: KY-040 Rotary Encoder Module

**Specifications:**
- **Type:** Mechanical rotary encoder with push-button
- **Detents:** 20 positions per rotation
- **Output:** Quadrature (CLK/DT) + Switch (SW)
- **Supply Voltage:** 3.3V - 5V
- **Pull-ups:** Integrated 10kΩ resistors on module

**Debouncing:**
- Software debounce via 1kHz timer interrupt (TIM3)
- 16-bit shift register for stable state detection
- Long-press detection: 500ms (menu entry), 10s (hidden diagnostics)

---

### 5. Optional: AS5600 Magnetic Angle Sensor

**Specifications:**
- **Type:** 12-bit contactless magnetic rotary position sensor
- **Interface:** I2C (address 0x36)
- **Range:** 0-360° absolute
- **Resolution:** 0.087° (4096 positions)
- **Supply Voltage:** 3.3V or 5V
- **Magnet:** Diametrically magnetized (6mm × 3mm recommended)

**Mounting:**
- Magnet attached to bandsaw angle adjustment mechanism
- Sensor PCB mounted stationary, 0.5-3mm air gap
- Automatic angle detection for 0-90° cut modes

**I2C Sharing:**
- Shares I2C bus with LCD (different addresses)
- Enable/disable via `USE_ANGLE_SENSOR` define in Config.h

---

## 📍 Pin Assignments

### STM32F411CE Pinout Table

| Function | STM32 Pin | Physical Pin | Direction | Notes |
|----------|-----------|--------------|-----------|-------|
| **Encoder A** | PB6 | 42 | Input | TIM4_CH1 (hardware capture) |
| **Encoder B** | PB7 | 43 | TIM4_CH2 (hardware capture) |
| **Menu CLK** | PB12 | 25 | Input | KY-040 quadrature A |
| **Menu DT** | PB13 | 26 | Input | KY-040 quadrature B |
| **Menu SW** | PB14 | 27 | Input | KY-040 push-button (active LOW) |
| **I2C SDA** | PB9 | 46 | Bidirectional | I2C1 SDA (LCD + AS5600) |
| **I2C SCL** | PB8 | 45 | Output | I2C1 SCL (LCD + AS5600) |
| **LED** | PC13 | 2 | Output | Onboard LED (active LOW) |
| **USB D-** | PA11 | 32 | USB | USB OTG (programming/debug) |
| **USB D+** | PA12 | 33 | USB | USB OTG (programming/debug) |
| **UART TX** | PA9 | 30 | Output | Serial1 debug output |
| **UART RX** | PA10 | 31 | Input | Serial1 debug input |
| **SWDIO** | PA13 | 34 | Bidirectional | ST-Link programming |
| **SWCLK** | PA14 | 37 | Input | ST-Link programming |

### Power Pins

| Pin | Name | Voltage | Usage |
|-----|------|---------|-------|
| 1 | VB | 3.3V | Battery backup (not used) |
| 8, 44 | GND | 0V | Ground (multiple pins) |
| 23 | 3.3V | 3.3V | Regulated output (max 150mA) |
| 24 | VDD | 3.3V | MCU supply |
| 48 | 5V | 5V | USB power input |

---

## 🔗 Wiring Diagrams

### Full System Wiring

```
┌──────────────────────────────────────────────────┐
│              USB Power Bank (5V/2A)              │
└──────────────┬───────────────────────────────────┘
               │
               ├─────────────────┐
               │                 │
          ┌────▼────┐       ┌────▼────┐
          │  Black  │       │  20x4   │
          │  Pill   │       │  LCD    │
          │         │       │  I2C    │
          │ PB9 ────┼──────►│ SDA     │
          │ PB8 ────┼──────►│ SCL     │
          │ GND ────┼──────►│ GND     │
          │ 5V  ────┼──────►│ VCC     │
          └─────────┘       └─────────┘
               │
               │  ┌─────────────────────────────┐
               │  │  KY-040 Rotary Encoder      │
               ├─►│  CLK ──► PB12               │
               ├─►│  DT  ──► PB13               │
               ├─►│  SW  ──► PB14               │
               ├─►│  +   ──► 3.3V               │
               └─►│  GND ──► GND                │
                  └─────────────────────────────┘

┌──────────────────────────────────────────────────┐
│  1024PPR Optical Encoder (with pull-ups)        │
│                                                  │
│  Brown (12V) ──────► +12V/5V Supply             │
│  Blue (GND)  ──────► GND                        │
│  Black (A)   ──┬───► 4.7kΩ ──► 3.3V            │
│                └───────────────► PB6 (TIM4_CH1) │
│  White (B)   ──┬───► 4.7kΩ ──► 3.3V            │
│                └───────────────► PB7 (TIM4_CH2) │
└──────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────┐
│  AS5600 Magnetic Angle Sensor (Optional)        │
│                                                  │
│  VCC ──────────► 3.3V                           │
│  GND ──────────► GND                            │
│  SDA ──────────► PB9 (shared with LCD)          │
│  SCL ──────────► PB8 (shared with LCD)          │
│                                                  │
│  DIR (not connected)                            │
│  PGO (not connected)                            │
└──────────────────────────────────────────────────┘
```

### Encoder Pull-Up Resistor Circuit

```
        3.3V
         │
         ├───[4.7kΩ]────┬──► PB6 (TIM4_CH1)
         │              │
         │          ┌───┴───┐
         │          │ Black │  Encoder
         │          │   A   │  (NPN Open
         │          └───┬───┘   Collector)
         │              │
         ├───[4.7kΩ]────┼──► PB7 (TIM4_CH2)
         │              │
         │          ┌───┴───┐
         │          │ White │
         │          │   B   │
         │          └───┬───┘
         │              │
        GND ←───────────┘
```

**Why Pull-Ups?**
- NPN open-collector outputs pull LOW when active
- Pull-up resistors provide HIGH state when inactive
- 4.7kΩ balances noise immunity and switching speed

---

## ⚡ Power Requirements

### Power Budget

| Component | Voltage | Current (Typical) | Current (Max) |
|-----------|---------|-------------------|---------------|
| STM32F411CE | 3.3V | 50mA | 100mA |
| 20x4 LCD (backlight ON) | 5V | 120mA | 150mA |
| KY-040 Encoder | 3.3V | 5mA | 10mA |
| AS5600 Sensor | 3.3V | 6mA | 10mA |
| Optical Encoder | 12V/5V | 30mA | 50mA |
| **Total (5V)** | | **~200mA** | **~320mA** |

### Power Supply Recommendations

**Option 1: USB Power Bank (Recommended)**
- **Spec:** 5V, 1A minimum (2A recommended)
- **Connector:** USB-A to Micro-USB cable
- **Advantages:** Portable, cheap, readily available, built-in charge management
- **Runtime:** 10,000mAh bank = ~40 hours continuous use

**Option 2: Wall Adapter**
- **Spec:** 5V/1A USB wall adapter
- **Use Case:** Permanently installed on bandsaw near AC outlet

**Encoder Power:**
- If using 12V encoder: Separate 12V supply or DC-DC buck converter from main power
- If using 5V-tolerant encoder: Direct connection to USB 5V rail

---

## 🔧 Mechanical Assembly

### Encoder Mounting

**Drive System:**
```
Fence Rail → Spider Coupling → M8 Axle → 608ZZ Bearings → Encoder Shaft
```

**Components:**
1. **Encoder Wheel:** 50mm diameter (aluminum or 3D-printed)
   - Attached to encoder shaft
   - Friction/contact with fence rail or drive belt

2. **Spider Coupling:** 6mm-to-8mm flexible coupling
   - Compensates for misalignment
   - Reduces vibration transmission

3. **Bearings:** 608ZZ (8mm ID, 22mm OD)
   - Supports M8 axle
   - Smooth, low-friction rotation

### Enclosure

**3D-Printed Parts (Design TBD):**
- Main housing for STM32 + LCD
- Encoder mounting bracket
- Cable management clips
- Access panel for USB charging

**Material:** PETG or ABS (better temperature tolerance than PLA)

---

## 🛠️ Troubleshooting

### Encoder Not Counting

**Symptoms:** Position doesn't change when fence moves

**Checks:**
1. Verify encoder wiring (A/B channels not swapped)
2. Check pull-up resistors (4.7kΩ to 3.3V)
3. Measure encoder output with multimeter (should toggle 0V ↔ VCC)
4. Verify TIM4 initialization in code (`EncoderSys::init()`)
5. Check encoder power supply voltage

---

### LCD Not Displaying

**Symptoms:** Blank screen or partial characters

**Checks:**
1. Verify I2C address (run I2C scanner: 0x27 or 0x3F)
2. Check SDA/SCL wiring (not swapped)
3. Confirm 5V power to LCD
4. Adjust contrast potentiometer on I2C backpack
5. Test I2C communication (monitor Serial1 debug output)

---

### Menu Encoder Erratic

**Symptoms:** Random rotations or missed clicks

**Checks:**
1. Verify KY-040 wiring (CLK/DT not swapped)
2. Check debounce logic (1kHz TIM3 interrupt running)
3. Inspect mechanical encoder for wear/dirt
4. Replace KY-040 module (common failure mode)

---

### Angle Sensor Not Detected

**Symptoms:** "Angle Sensor NOT FOUND" on boot

**Checks:**
1. Verify AS5600 wiring (SDA/SCL correct)
2. Confirm I2C address (0x36 fixed address)
3. Check magnet position (0.5-3mm air gap, centered)
4. Test with I2C scanner
5. Verify `USE_ANGLE_SENSOR` enabled in Config.h

---

### System Resets Randomly

**Symptoms:** Unexpected reboots during operation

**Checks:**
1. **Power supply:** Insufficient current (upgrade to 2A)
2. **Watchdog timeout:** `IWatchdog.reload()` not called frequently (check main loop)
3. **Brown-out:** Voltage drops below 4.5V (poor USB cable, weak power bank)
4. **I2C lockup:** Bus stuck (power cycle all I2C devices)

---

## 📚 Additional Resources

- **[STM32F411 Datasheet](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)**
- **[Black Pill Schematic](https://github.com/WeActTC/MiniSTM32F4x1)**
- **[HD44780 LCD Datasheet](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf)**
- **[AS5600 Datasheet](https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf)**

---

**Last Updated:** 2025-11-25  
**Hardware Version:** v1.0  
**Compatible Firmware:** v1.0.0+
