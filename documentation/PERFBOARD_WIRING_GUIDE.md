# IronTrak Perfboard Assembly Checklist

**Check off each step as you complete it**

---

## 📦 Step 1: Gather Components

- [ ] STM32F411CE Black Pill (100MHz, USB-C)
- [ ] 20×4 LCD I2C Display (Address 0x27)
- [ ] KY-040 Rotary Encoder Module
- [ ] 1024 PPR Optical Encoder (Industrial quadrature)
- [ ] AS5600 Magnetic Sensor (optional)
- [ ] 4.7kΩ Resistors × 4
- [ ] 10µF Electrolytic Capacitors × 2
- [ ] 0.1µF Ceramic Capacitor × 1
- [ ] Perfboard (70×50mm minimum)
- [ ] Female pin headers for Black Pill
- [ ] Hookup wire (various colors)

---

## 🔌 Step 2: Prepare Power Rails

### 5V Rail

- [ ] Run 5V power rail across perfboard (red wire)
- [ ] Add 10µF electrolytic capacitor (5V to GND)

### 3.3V Rail

- [ ] Run 3.3V rail from Black Pill 3.3V pin (orange wire)
- [ ] Add 10µF electrolytic capacitor (3.3V to GND) - optional

### GND Rail

- [ ] Run GND rail across perfboard (black wire)
- [ ] Add 0.1µF ceramic capacitor near Black Pill VDD

---

## 🔧 Step 3: Install STM32 Black Pill

- [ ] Solder female pin headers to perfboard
- [ ] Connect Black Pill 5V pin to 5V rail
- [ ] Connect Black Pill GND pin to GND rail
- [ ] Verify 3.3V output available from Black Pill

---

## 🎛️ Step 4: Wire Industrial Encoder (1024 PPR)

### Pull-Up Resistors (REQUIRED!)

- [ ] Solder R1: 4.7kΩ between 3.3V and PB6
- [ ] Solder R2: 4.7kΩ between 3.3V and PB7

### Encoder Wires

| Wire Color   | Connect To          | Done |
| ------------ | ------------------- | ---- |
| Brown        | +5V or +12V rail    | [ ]  |
| Blue         | GND rail            | [ ]  |
| Black (Ch A) | PB6 (after pull-up) | [ ]  |
| White (Ch B) | PB7 (after pull-up) | [ ]  |

```
        3.3V
         │
     ┌───┴───┐
   [4.7kΩ] [4.7kΩ]
     │       │
     ├───────┼──► PB6 ◄── Encoder Black (A)
     │       │
     │       └──► PB7 ◄── Encoder White (B)
     │
    GND ◄─────────── Encoder Blue
```

> [!CAUTION] > **Pull-ups are MANDATORY for NPN open-collector encoders!**

---

## 📺 Step 5: Wire LCD Display (I2C)

| LCD Pin | Connect To | Done |
| ------- | ---------- | ---- |
| VCC     | 5V rail    | [ ]  |
| GND     | GND rail   | [ ]  |
| SDA     | PB9        | [ ]  |
| SCL     | PB8        | [ ]  |

### Optional I2C Pull-Ups

- [ ] Check if LCD has onboard pull-ups (most do)
- [ ] If needed: R3 4.7kΩ between 3.3V and PB8 (SCL)
- [ ] If needed: R4 4.7kΩ between 3.3V and PB9 (SDA)

**I2C Address: 0x27** (or 0x3F on some modules)

---

## 🎮 Step 6: Wire KY-040 Menu Encoder

| KY-040 Pin | Connect To    | Done |
| ---------- | ------------- | ---- |
| GND        | GND rail      | [ ]  |
| +          | **3.3V rail** | [ ]  |
| SW         | PB14          | [ ]  |
| DT         | PB13          | [ ]  |
| CLK        | PB12          | [ ]  |

> [!WARNING] > **CRITICAL: Power from 3.3V NOT 5V! 5V will damage STM32!**

---

## 🧭 Step 7: Wire AS5600 Angle Sensor (Optional)

| AS5600 Pin | Connect To            | Done |
| ---------- | --------------------- | ---- |
| VCC        | 3.3V rail             | [ ]  |
| GND        | GND rail              | [ ]  |
| SDA        | PB9 (shared with LCD) | [ ]  |
| SCL        | PB8 (shared with LCD) | [ ]  |
| DIR        | Not connected         | [ ]  |

**I2C Address: 0x36** (fixed)

---

## ✅ Step 8: Pre-Power Safety Checks

### Continuity Checks (Use Multimeter)

- [ ] No short between 5V and GND
- [ ] No short between 3.3V and GND
- [ ] No short between 5V and 3.3V

### Visual Inspection

- [ ] All solder joints clean (no bridges)
- [ ] All wires secure (no loose connections)
- [ ] Correct wire colors to correct pins
- [ ] Pull-up resistors installed on PB6 and PB7

### Voltage Verification

- [ ] KY-040 connected to 3.3V (NOT 5V)
- [ ] LCD connected to 5V
- [ ] AS5600 connected to 3.3V (if used)

---

## ⚡ Step 9: Power-On Tests

### Initial Power-Up

- [ ] Connect USB power bank
- [ ] Blue LED on Black Pill turns on
- [ ] No smoke or burning smell
- [ ] 5V rail measures 4.8-5.2V
- [ ] 3.3V rail measures 3.2-3.4V

### LCD Test

- [ ] LCD backlight turns on
- [ ] Characters display (may need contrast adjustment)
- [ ] If blank: adjust blue potentiometer on I2C backpack

### Encoder Test

- [ ] Moving fence changes position reading
- [ ] Position increases and decreases correctly
- [ ] No erratic jumps in reading

### Menu Encoder Test

- [ ] Rotating KY-040 navigates menu
- [ ] Pressing button selects items
- [ ] No erratic behavior

---

## 📊 Quick Reference Card

```
┌─────────────────────────────────────────────────┐
│            IRONTRAK PINOUT SUMMARY              │
├─────────────────────────────────────────────────┤
│ ENCODER (1024PPR)    │ MENU (KY-040)           │
│ Black (A) → PB6      │ CLK → PB12              │
│ White (B) → PB7      │ DT  → PB13              │
│ + 4.7kΩ pull-ups!    │ SW  → PB14              │
│                      │ +   → 3.3V ONLY!        │
├─────────────────────────────────────────────────┤
│ I2C BUS              │ POWER                   │
│ SDA → PB9            │ 5V  → Black Pill 5V     │
│ SCL → PB8            │ 3.3V → From LDO         │
│ LCD: 0x27            │ GND → Common ground     │
│ AS5600: 0x36         │                         │
└─────────────────────────────────────────────────┘
```

---

## 🔧 Troubleshooting Checklist

### Encoder Not Counting

- [ ] Verify pull-up resistors installed (4.7kΩ to 3.3V)
- [ ] Check A/B wires not swapped
- [ ] Verify encoder power supply (5V or 12V per spec)
- [ ] Test encoder output with multimeter

### LCD Blank/Garbled

- [ ] Adjust contrast potentiometer
- [ ] Verify I2C address (0x27 or 0x3F)
- [ ] Check SDA/SCL not swapped
- [ ] Confirm 5V power

### Menu Encoder Erratic

- [ ] Verify powered from 3.3V (NOT 5V)
- [ ] Check CLK/DT not swapped
- [ ] Inspect for dirty/worn contacts

### System Resets

- [ ] Check power supply current (need 500mA+)
- [ ] Verify no power rail shorts
- [ ] Check USB cable quality

---

**Created:** 2025-12-07  
**Hardware Version:** v1.0
