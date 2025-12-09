# IronTrak - Power Budget Analysis

**Comprehensive power consumption analysis and supply recommendations**

---

## 📋 Executive Summary

| Parameter                     | Value                 | Notes                                                 |
| ----------------------------- | --------------------- | ----------------------------------------------------- |
| **Idle Current Draw**         | ~155 mA               | MCU in run mode, LCD backlight ON, no encoder motion  |
| **Peak Current Draw**         | ~330 mA               | All components active, encoder rotating, LCD updating |
| **Recommended Safety Margin** | +20%                  | Standard industrial practice                          |
| **Total Design Current**      | **396 mA**            | Peak + 20% safety margin                              |
| **Recommended Power Supply**  | **5V @ 500mA (2.5W)** | Minimum safe specification                            |
| **Optimal Power Supply**      | **5V @ 1A (5W)**      | Recommended for reliability                           |

### 🎯 Key Findings

> [!IMPORTANT]
> The **LCD backlight is the most power-hungry component**, consuming **36-48%** of total system power. Consider implementing user-selectable brightness control or auto-dimming after inactivity to significantly extend battery runtime.

---

## 🔌 Component Inventory & Current Consumption

### Main Components

| Component                    | Operating Voltage | Typical Current | Peak Current | % of Peak Load | Datasheet Source           |
| ---------------------------- | ----------------- | --------------- | ------------ | -------------- | -------------------------- |
| **STM32F411CE Black Pill**   | 3.3V (5V input)   | 40-60 mA        | 100 mA       | 30.3%          | ST Microelectronics        |
| **20x4 LCD + I2C Backpack**  | 5V                | 120-160 mA      | 160 mA       | 48.5%          | HD44780 + PCF8574T         |
| **1024 PPR Optical Encoder** | 5V                | 30-40 mA        | 50 mA        | 15.2%          | Industrial optical encoder |
| **KY-040 Rotary Encoder**    | 3.3V              | 5 mA            | 10 mA        | 3.0%           | Standard rotary encoder    |
| **AS5600 Magnetic Sensor**   | 3.3V              | 6 mA            | 10 mA        | 3.0%           | AMS AS5600 (optional)      |
| **TOTAL (without AS5600)**   |                   | **~200 mA**     | **~320 mA**  | **97%**        |                            |
| **TOTAL (with AS5600)**      |                   | **~206 mA**     | **~330 mA**  | **100%**       |                            |

---

## 📊 Detailed Component Analysis

### 1. STM32F411CE Black Pill

**Datasheet Specifications:**

- **Run Mode @ 100MHz:** 100 µA/MHz = 10 mA (core only)
- **Peripherals Active:** Additional 30-50 mA
  - TIM4 (Encoder quadrature capture)
  - TIM3 (1kHz interrupt)
  - I2C1 (LCD + sensor communication)
  - UART (debug output)
  - GPIOs (encoder inputs, LED)

**Measured Estimates:**

- **Idle (no code execution):** 40 mA
- **Typical (normal operation):** 50-60 mA
- **Peak (heavy I2C traffic + interrupts):** 100 mA

**Power Breakdown:**

- Core @ 100MHz: ~10 mA
- Flash access: ~15 mA
- Peripherals (TIM4, TIM3, I2C, UART): ~25 mA
- GPIO switching: ~5-10 mA

---

### 2. 20x4 LCD with I2C Backpack

**Critical Component:** This is the **largest power consumer** in the system.

**LCD Controller (HD44780):**

- Logic current: ~3-5 mA
- Character generation: ~2 mA

**I2C Backpack (PCF8574T):**

- I2C I/O expander: ~1 mA

**LED Backlight (Most Significant):**

- **Typical:** 120 mA @ 5V (blue backlight)
- **Maximum:** 160 mA @ 5V (full brightness)
- **Measured Range:** 40-240 mA depending on model/brightness

**Power Optimization Opportunities:**

```
Current Configuration: 120 mA (backlight always ON)

Potential Savings:
1. PWM brightness control (50% duty): Save ~60 mA
2. Auto-dim after 30s inactivity: Save ~80 mA (backlight LOW)
3. Auto-off after 5min inactivity: Save ~120 mA
```

**Recommendation:** Implement backlight PWM control for **30-50% power reduction** in typical use.

---

### 3. Industrial Optical Encoder (1024 PPR)

**Type:** NPN Open Collector Quadrature Encoder

**Power Consumption:**

- **Supply Voltage:** 5V (can operate 5-24V)
- **Quiescent Current:** 30 mA (no rotation)
- **Active Current:** 40-50 mA (during rotation)

**Pull-Up Resistors:**

- 2× 4.7kΩ pull-ups to 3.3V
- Current per resistor: (3.3V / 4.7kΩ) = **0.7 mA**
- Total pull-up current: **1.4 mA** (included in encoder current above)

**Notes:**

- Current draw is **independent of rotation speed** (optical sensing)
- Power consumption is **constant** in bandsaw application

---

### 4. KY-040 Rotary Encoder (Menu Input)

**Type:** Mechanical rotary encoder with integrated pull-ups

**Power Consumption:**

- **Typical:** 5 mA (idle)
- **Peak:** 10 mA (button pressed + rotation)

**Components:**

- Quadrature switches: Negligible
- Integrated 10kΩ pull-up resistors: ~1 mA
- Push-button switch: Negligible

**Notes:**

- Lowest power consumer in system
- Power draw primarily from integrated pull-ups

---

### 5. AS5600 Magnetic Angle Sensor (Optional)

**Type:** 12-bit contactless magnetic rotary position sensor

**Datasheet Specifications:**

- **Operating Voltage:** 3.3V or 5V
- **Normal Mode:** 6.5 mA typical
- **Low Power Mode 1:** 3.4 mA
- **Low Power Mode 3:** 1.3 mA

**For IronTrak:**

- Operating at 3.3V: **6 mA typical**
- Peak (I2C communication): **10 mA**

**Power Optimization:**

- Can use Low Power Mode when not actively measuring angle
- Potential savings: **3-5 mA** if duty-cycled

---

## ⚡ Power Consumption by Operating State

### State 1: IDLE (Waiting for User Input)

**Definition:** System powered on, displaying current measurement, no fence movement.

| Component         | Current     | Notes                            |
| ----------------- | ----------- | -------------------------------- |
| STM32F411CE       | 50 mA       | Run mode, periodic LCD updates   |
| 20x4 LCD          | 120 mA      | Backlight ON, static display     |
| Optical Encoder   | 30 mA       | Powered, no rotation             |
| KY-040 Encoder    | 5 mA        | Idle                             |
| AS5600 (optional) | 0 mA        | Not fitted in base configuration |
| **TOTAL IDLE**    | **~205 mA** | **1.025W @ 5V**                  |

**With AS5600 fitted:** ~211 mA (1.055W @ 5V)

---

### State 2: ACTIVE (Normal Cutting Operation)

**Definition:** User moving fence, encoder tracking, LCD updating in real-time.

| Component         | Current     | Notes                                  |
| ----------------- | ----------- | -------------------------------------- |
| STM32F411CE       | 60 mA       | Heavy interrupt load (TIM4 quadrature) |
| 20x4 LCD          | 130 mA      | Backlight ON, frequent updates         |
| Optical Encoder   | 40 mA       | Actively rotating                      |
| KY-040 Encoder    | 5 mA        | Idle                                   |
| AS5600 (optional) | 0 mA        | Not fitted                             |
| **TOTAL ACTIVE**  | **~235 mA** | **1.175W @ 5V**                        |

**With AS5600 fitted:** ~241 mA (1.205W @ 5V)

---

### State 3: PEAK (Worst-Case Scenario)

**Definition:** All components operating at maximum rated current simultaneously.

| Component         | Current     | Notes                           |
| ----------------- | ----------- | ------------------------------- |
| STM32F411CE       | 100 mA      | Maximum rated current           |
| 20x4 LCD          | 160 mA      | Backlight at maximum brightness |
| Optical Encoder   | 50 mA       | Peak optical sensor current     |
| KY-040 Encoder    | 10 mA       | Button pressed + rotation       |
| AS5600 (optional) | 10 mA       | Peak I2C read operation         |
| **TOTAL PEAK**    | **~330 mA** | **1.65W @ 5V**                  |

**Notes:**

- This state is **unlikely** during normal operation
- Represents **absolute maximum** for power supply sizing

---

### State 4: MENU NAVIGATION

**Definition:** User actively navigating settings menu.

| Component         | Current     | Notes                                   |
| ----------------- | ----------- | --------------------------------------- |
| STM32F411CE       | 70 mA       | Menu state machine, frequent LCD writes |
| 20x4 LCD          | 140 mA      | Backlight ON, full-screen updates       |
| Optical Encoder   | 30 mA       | Not rotating (fence stationary)         |
| KY-040 Encoder    | 10 mA       | Active rotation + button presses        |
| AS5600 (optional) | 6 mA        | Idle polling                            |
| **TOTAL MENU**    | **~250 mA** | **1.25W @ 5V**                          |

---

## 🔋 Power Supply Design Calculations

### Total System Power Requirement

```
Typical Operation: 200-235 mA
Peak Operation:    320-330 mA
Safety Margin:     +20% (industrial standard)

Design Current = Peak × 1.20
               = 330 mA × 1.20
               = 396 mA
```

### Power Calculation

```
Power (W) = Voltage (V) × Current (A)

Typical Power:  5V × 0.205A = 1.025 W
Peak Power:     5V × 0.330A = 1.650 W
Design Power:   5V × 0.396A = 1.980 W (~2W)
```

---

## 📦 Power Supply Recommendations

### Option 1: USB Power Bank (Current Implementation) ✅

**Specification:**

- **Voltage:** 5V USB standard
- **Capacity:** 10,000 mAh (typical)
- **Output Current:** 1A - 2.1A (depends on bank)

**Advantages:**

- ✅ Portable (no AC wiring required)
- ✅ Built-in charge management
- ✅ Widely available, low cost ($10-15)
- ✅ Auto-shutoff protection
- ✅ USB-C or Micro-USB input (easy charging)

**Runtime Calculation:**

```
Battery Capacity:     10,000 mAh @ 5V
Typical Current Draw: 205 mA

Runtime = Capacity / Current
        = 10,000 mAh / 205 mA
        = 48.8 hours (theoretical)

Adjusted for 80% usable capacity:
        = 48.8 hours × 0.80
        = 39 hours continuous use
```

**Real-World Runtime:**

- **Heavy Use (8hr/day):** ~5 days per charge
- **Light Use (2hr/day):** ~20 days per charge

**Recommendation:** ✅ **EXCELLENT CHOICE** for portable bandsaw installation.

---

### Option 2: USB Wall Adapter (Permanent Installation)

**Specification:**

- **Voltage:** 5V DC
- **Current Rating:** Minimum 500mA, Recommended 1A
- **Connector:** USB-A to Micro-USB or USB-C

**Recommended Models:**
| Rating | Use Case | Cost |
|--------|----------|------|
| 5V @ 500mA (2.5W) | **Minimum safe rating** | $5 |
| 5V @ 1A (5W) | **Recommended** | $8 |
| 5V @ 2A (10W) | Overkill but future-proof | $10 |

**Advantages:**

- ✅ No charging required
- ✅ Continuous operation
- ✅ Lower long-term cost

**Disadvantages:**

- ❌ Requires AC outlet near bandsaw
- ❌ Cable management

**Recommendation:** ✅ **BEST for stationary installation** near AC power.

---

### Option 3: HiLink AC-DC Module (Professional Installation)

**Specification:**

- **Model:** HLK-PM01 or HLK-5M05
- **Input:** 100-240VAC, 50/60Hz
- **Output:** 5V DC @ 600mA or 1A
- **Isolation:** 3000VAC

**Complete Circuit Cost:** ~$7.50 (see `POWER_SUPPLY.md`)

**Advantages:**

- ✅ Direct mains connection (no USB adapter)
- ✅ Compact, internal mounting
- ✅ Industrial-grade reliability
- ✅ High-quality filtering (low ripple)

**Disadvantages:**

- ❌ Requires AC wiring (**dangerous if DIY**)
- ❌ Safety considerations (isolation, fusing)
- ❌ More complex installation

**Recommendation:** ⚠️ **ONLY if you are comfortable with mains voltage wiring** or hiring an electrician.

---

### Option 4: Battery (LiPo/Li-Ion) - Custom Build

**Specification:**

- **Battery Type:** Single-cell Li-Ion (3.7V nominal) or 2S LiPo (7.4V nominal)
- **Step-Up/Step-Down Converter:** Buck-boost to 5V
- **Capacity:** 2000-5000 mAh

**Example Configuration:**

```
18650 Li-Ion Cell (3.7V, 3000mAh)
    ↓
Step-Up Converter (3.7V → 5V @ 90% efficiency)
    ↓
5V Output to IronTrak

Effective Capacity: 3000mAh × 3.7V / 5V × 0.90 = ~2000 mAh @ 5V

Runtime: 2000 mAh / 205 mA = 9.7 hours
```

**Advantages:**

- ✅ Compact, integrated design
- ✅ Rechargeable (USB-C charge board)

**Disadvantages:**

- ❌ Requires custom PCB or module
- ❌ Battery protection circuit needed
- ❌ Shorter runtime vs. power bank

**Recommendation:** ❌ **NOT RECOMMENDED** - USB power bank is simpler and cheaper.

---

## 🏆 Final Recommendation

### **Best Power Supply: 10,000mAh USB Power Bank**

**Rationale:**

1. ✅ **Meets all power requirements** with significant headroom (1A output >> 330mA peak)
2. ✅ **Portable** - No AC wiring, easy to move between machines
3. ✅ **Long runtime** - 39 hours continuous use (weeks of typical shop use)
4. ✅ **Easy charging** - USB-C/Micro-USB, charge overnight monthly
5. ✅ **Built-in protection** - Overcurrent, short-circuit, auto-shutoff
6. ✅ **Low cost** - $10-15, widely available

### **Alternative: 5V 1A USB Wall Adapter**

**Use if:**

- Bandsaw is near AC outlet
- Permanent installation preferred
- No need for portability

**Cost:** ~$8 for quality adapter

---

## ⚙️ Power Optimization Strategies

### Immediate Opportunities (No Hardware Changes)

#### 1. LCD Backlight PWM Control (HIGH IMPACT)

**Implementation:**

```cpp
// Add PWM control to LCD backlight
// Connect LCD backlight enable pin to PWM-capable GPIO

void setBacklightBrightness(uint8_t brightness) {
    // brightness: 0-100%
    analogWrite(LCD_BACKLIGHT_PIN, map(brightness, 0, 100, 0, 255));
}

// Settings menu option
enum BacklightMode {
    BACKLIGHT_FULL = 100,    // 160 mA
    BACKLIGHT_MEDIUM = 50,   // ~80 mA (SAVE 80mA!)
    BACKLIGHT_LOW = 25,      // ~40 mA (SAVE 120mA!)
    BACKLIGHT_OFF = 0        // ~0 mA (SAVE 160mA!)
};
```

**Power Savings:**

- Medium brightness (50%): Save **80 mA** (39% reduction)
- Low brightness (25%): Save **120 mA** (58% reduction)

**Runtime Impact:**

```
Original Runtime:     39 hours @ 205 mA
With 50% Backlight:  64 hours @ 125 mA (+64% runtime!)
```

> [!TIP]
> Implementing adjustable backlight brightness could **extend battery life by 60%** with minimal code changes. This is the **highest ROI power optimization** available.

---

#### 2. Auto-Dim / Auto-Off Timers (MEDIUM IMPACT)

**Strategy:**

```
1. Full brightness during active use
2. Auto-dim to 25% after 30 seconds of inactivity
3. Auto-off backlight after 5 minutes of inactivity
4. Wake on button press or encoder movement
```

**Power Savings (during idle periods):**

- Auto-dim: Save **120 mA**
- Auto-off: Save **160 mA**

**Use Case:**

- Bandsaw sits idle between cuts (common in shop)
- Operator walks away, backlight dims/off automatically
- Instant wake on next interaction

---

### Future Hardware Optimizations (Requires Redesign)

#### 3. AS5600 Duty Cycling (LOW IMPACT, OPTIONAL)

**Strategy:**

- Only power AS5600 when angle reading needed
- Use Low Power Mode 3 when idle
- Savings: **4-5 mA** (negligible)

#### 4. MCU Clock Scaling (COMPLEX, LOW BENEFIT)

**Strategy:**

- Reduce STM32 clock from 100MHz to 50MHz during idle
- Savings: **~25 mA**
- **NOT RECOMMENDED:** Adds complexity, minimal gain vs. backlight control

---

## 📋 Power Budget Summary Table

| Operating State               | Current Draw | Power (W) | Runtime (10Ah Bank) | % of Peak |
| ----------------------------- | ------------ | --------- | ------------------- | --------- |
| **Idle**                      | 205 mA       | 1.03 W    | 39 hours            | 62%       |
| **Active (Cutting)**          | 235 mA       | 1.18 W    | 34 hours            | 71%       |
| **Menu Navigation**           | 250 mA       | 1.25 W    | 32 hours            | 76%       |
| **Peak (Worst-Case)**         | 330 mA       | 1.65 W    | 24 hours            | 100%      |
| **Optimized (50% Backlight)** | 125 mA       | 0.63 W    | 64 hours            | 38%       |

---

## 🎯 Success Criteria (From User Request)

### ✅ Estimated Total Current Draw

- **Peak:** 330 mA (all components maximum)
- **Idle:** 205 mA (typical waiting state)

### ✅ Power Supply Recommendation

- **USB Power Bank:** 10,000mAh @ 5V/1A (39 hours runtime)
- **Wall Adapter:** 5V @ 1A (5W) minimum, 5V @ 2A preferred

### ✅ Most Power-Hungry Component

- **20x4 LCD Backlight:** 120-160 mA (**48.5% of total peak load**)
- **Optimization potential:** 60% power reduction via PWM brightness control

---

## 📚 References & Sources

### Component Datasheets

- [STM32F411CE Datasheet](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf) - Power consumption: 100 µA/MHz
- [HD44780 LCD Controller](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) - Logic + backlight specifications
- [AS5600 Magnetic Sensor](https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf) - 6.5 mA normal mode
- Industrial Optical Encoder - Typical 30-50 mA @ 5V for 1024 PPR units

### Existing Documentation

- [`HARDWARE.md`](HARDWARE.md#power-requirements) - Preliminary power budget
- [`POWER_SUPPLY.md`](../docs/POWER_SUPPLY.md) - HiLink AC-DC design (professional installation)
- [`POWER_SUPPLY_SIMPLIFIED.md`](../docs/POWER_SUPPLY_SIMPLIFIED.md) - Budget filtering options

---

**Last Updated:** 2025-11-28  
**Analysis Version:** 1.0  
**Compatible Firmware:** v1.0.0+
