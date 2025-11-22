# IronTrak - Wiring Diagram & Passive Components

## Complete Connection Schematic

```
╔══════════════════════════════════════════════════════════════════════════════╗
║                          IRONTRAK WIRING DIAGRAM                             ║
╚══════════════════════════════════════════════════════════════════════════════╝

                                  USB POWER BANK
                                       │
                                  [USB Cable]
                                       │
                                       ▼
    ┌────────────────────────────────────────────────────────────────┐
    │                      ARDUINO NANO                              │
    │                    (ATmega328P)                                │
    │                                                                │
    │  D2 ◄────────────┐                    ┌────────────► A4 (SDA) │
    │                  │                    │                        │
    │  D3 ◄────────────┤                    ├────────────► A5 (SCL) │
    │                  │                    │                        │
    │  D4 ◄────────────┤                    │                        │
    │                  │                    │                        │
    │  D5 ◄────────────┤                    │                        │
    │                  │                    │                        │
    │  D6 ◄────────────┤                    │                        │
    │                  │                    │                        │
    │  5V ──────────┬──┼────────────────────┼──────────┬─────────────┤
    │               │  │                    │          │             │
    │  GND ─────────┼──┼────────────────────┼──────────┼─────────────┤
    │               │  │                    │          │             │
    └───────────────┼──┼────────────────────┼──────────┼─────────────┘
                    │  │                    │          │
                    │  │                    │          │
        ┌───────────┘  │        ┌───────────┘          └───────────┐
        │              │        │                                  │
        │              │        │                                  │
        │              │        │                                  │
    ┌───▼──────────────▼────┐   │        ┌───────────────┐     ┌──▼─────────┐
    │  INDUSTRIAL ENCODER   │   │        │   KY-040      │     │  20×4 LCD  │
    │    (600 PPR NPN)      │   │        │   ENCODER     │     │   I2C      │
    │                       │   │        │               │     │            │
    │  [R] VCC ──────► 5V   │   │        │  [+] ────► 5V │     │ VCC ──► 5V │
    │  [Bk] GND ──────► GND │   │        │  [G] ────► GND│     │ GND ──► GND│
    │  [G] A ──────────┐    │   │        │  CLK ────► D4 │     │ SDA ──► A4 │
    │  [W] B ──────────┤    │   │        │  DT  ────► D5 │     │ SCL ──► A5 │
    │                  │    │   │        │  SW  ────► D6 │     │            │
    └──────────────────┼────┘   │        └───────────────┘     └────────────┘
                       │        │
                       │        │
        ┌──────────────┘        │
        │                       │
        │    PULL-UP RESISTORS  │
        │    (4.7kΩ each)       │
        │                       │
        │         5V            │
        │          │            │
        │       ┌──┴──┐         │
        │       │ 4.7k│         │
        │       │ Ω   │         │
        │       └──┬──┘         │
        │          │            │
        └──────────┴────► D2    │
                                │
                   5V           │
                    │           │
                 ┌──┴──┐        │
                 │ 4.7k│        │
                 │ Ω   │        │
                 └──┬──┘        │
                    │           │
                    └───────────┴────► D3

```

---

## Passive Components Required

### Pull-Up Resistors (CRITICAL!)

The **Industrial Encoder (600PPR NPN Open Collector)** requires pull-up resistors on the output signals.

| Component | Quantity | Value | Purpose | Location |
|-----------|----------|-------|---------|----------|
| Resistor | 2 | **4.7kΩ** | Pull-up for Encoder A signal | Between D2 and 5V |
| Resistor | 2 | **4.7kΩ** | Pull-up for Encoder B signal | Between D3 and 5V |

**Why?** NPN open collector outputs need external pull-ups to create a HIGH signal when the transistor is OFF.

**Alternatives:**
- 4.7kΩ (recommended)
- 10kΩ (also works, slightly slower switching)
- 2.2kΩ (works, uses more current)

**DO NOT USE:**
- Values < 1kΩ (too much current)
- Values > 47kΩ (unreliable, slow)

---

## Optional Passive Components

### Decoupling Capacitors (Recommended for Reliability)

| Component | Quantity | Value | Purpose | Location |
|-----------|----------|-------|---------|----------|
| Ceramic Cap | 1 | 100nF (0.1µF) | Filter encoder power | Across VCC/GND at encoder |
| Ceramic Cap | 1 | 100nF (0.1µF) | Filter LCD power | Across VCC/GND at LCD |
| Electrolytic | 1 | 100µF | Bulk power filtering | Across 5V/GND on Arduino |

**Purpose:** Reduce electrical noise, prevent glitches from encoder switching.

**Not Required but Improves:**
- Signal stability
- Noise immunity
- Long-term reliability

---

## Detailed Wire Connections

### 1. Industrial Encoder → Arduino

```
ENCODER SIDE          WIRE COLOR    ARDUINO SIDE
┌──────────┐
│  VCC     │────[Red]──────────────► 5V
│          │
│  GND     │────[Black]────────────► GND
│          │                          │
│  A       │────[Green]────┬──────────┼──► D2
│          │               │          │
│          │            ┌──┴──┐       │
│          │            │4.7kΩ│       │
│          │            └──┬──┘       │
│          │               │          │
│          │             [5V]◄────────┘
│          │
│  B       │────[White]────┬──────────┼──► D3
│          │               │          │
│          │            ┌──┴──┐       │
│          │            │4.7kΩ│       │
│          │            └──┬──┘       │
│          │               │          │
│          │             [5V]◄────────┘
│          │
│  Z       │────[Not connected]
└──────────┘
```

**⚠️ CRITICAL:** The 4.7kΩ resistors are **MANDATORY** for NPN encoders!

---

### 2. KY-040 Menu Encoder → Arduino

```
KY-040 MODULE         WIRE COLOR    ARDUINO SIDE
┌──────────┐
│  +       │────[Red]──────────────► 5V
│          │
│  GND     │────[Black]────────────► GND
│          │
│  CLK     │────[Yellow]───────────► D4
│          │
│  DT      │────[Orange]───────────► D5
│          │
│  SW      │────[Blue]─────────────► D6
└──────────┘
```

**Note:** KY-040 has **built-in pull-ups** on the module PCB. No external resistors needed!

---

### 3. 20×4 LCD I2C → Arduino

```
LCD I2C BACKPACK      WIRE COLOR    ARDUINO SIDE
┌──────────┐
│  VCC     │────[Red]──────────────► 5V
│          │
│  GND     │────[Black]────────────► GND
│          │
│  SDA     │────[Green]────────────► A4
│          │
│  SCL     │────[Yellow]───────────► A5
└──────────┘
```

**Note:** I2C backpack has **built-in pull-ups** (usually 4.7kΩ on SDA/SCL). No external resistors needed!

---

### 4. Power Distribution

```
USB POWER BANK
      │
      │ [USB Cable (Mini-B)]
      │
      ▼
  ARDUINO NANO
      │
      ├─[5V]──┬──► Industrial Encoder VCC
      │       ├──► KY-040 VCC (+)
      │       ├──► LCD VCC
      │       └──► Pull-up Resistors (top rail)
      │
      └─[GND]─┬──► Industrial Encoder GND
              ├──► KY-040 GND
              └──► LCD GND
```

**Current Draw (Estimate):**
- Arduino: ~50mA
- Industrial Encoder: ~100mA
- LCD Backlight: ~80mA
- KY-040: ~5mA
- **Total: ~235mA** (well within USB power bank capacity)

---

## Build Options

### Option A: Breadboard Prototype (Quick Test)

**You Need:**
- Breadboard
- Dupont jumper wires
- 2× 4.7kΩ resistors (1/4W through-hole)
- (Optional) 3× 100nF ceramic caps

**Advantages:**
- Fast assembly
- Easy to debug
- Can swap components

**Disadvantages:**
- Not mechanically stable
- Vibration-prone
- Jumper wires can disconnect

---

### Option B: Perfboard/Stripboard (Permanent)

**You Need:**
- Perfboard or stripboard
- 22 AWG wire
- 2× 4.7kΩ resistors (through-hole or SMD)
- (Optional) Screw terminals
- Soldering iron

**Advantages:**
- Mechanically stable
- Compact
- Reliable connections

**Disadvantages:**
- Permanent (harder to modify)
- Requires soldering skill

---

### Option C: Custom PCB (Professional)

**You Need:**
- PCB design software (KiCad, EasyEDA)
- PCB manufacturing service
- SMD components (optional)

**Advantages:**
- Most compact
- Most reliable
- Professional appearance

**Disadvantages:**
- Expensive ($20-50 for small run)
- Long lead time (2-4 weeks)
- Design complexity

**Recommended:** Start with **Option A** (breadboard) to test, then move to **Option B** (perfboard) for final build.

---

## Shopping List: Passive Components

### Minimum Required
- [ ] **2× 4.7kΩ resistors** (1/4W, through-hole)
  - Cost: $0.10 (or buy 100-pack for $2)
  - Mouser/DigiKey/Amazon

### Recommended Additions
- [ ] **3× 100nF ceramic capacitors** (50V rated)
  - Cost: $0.30 (or buy 100-pack for $3)
- [ ] **1× 100µF electrolytic capacitor** (16V or higher)
  - Cost: $0.20

### For Perfboard Build
- [ ] Perfboard (70×90mm or similar)
- [ ] 22 AWG solid core wire (red/black)
- [ ] Solder (60/40 or lead-free)

**Total Cost: $0.10 - $5.00** depending on options

---

## Common Wiring Mistakes to Avoid

### ❌ WRONG: No Pull-Up Resistors
```
Encoder A ──────────► D2  (Won't work! Signals float)
Encoder B ──────────► D3
```

### ✅ CORRECT: With Pull-Ups
```
        5V
         │
      ┌──┴──┐
      │4.7kΩ│
      └──┬──┘
         │
Encoder A┴──────────► D2  (Works!)
```

---

### ❌ WRONG: Mixing Power Rails
```
Encoder VCC ──► 3.3V  (Encoder needs 5V!)
Arduino VCC ──► 5V
```

### ✅ CORRECT: All 5V
```
Encoder VCC ──┬──► 5V
LCD VCC ──────┤
KY-040 VCC ───┘
```

---

### ❌ WRONG: Common Ground Missing
```
Encoder GND ──► GND (Encoder)
LCD GND ──────► GND (LCD)  (Separate grounds = glitches!)
```

### ✅ CORRECT: Star Ground
```
Encoder GND ──┬──► GND (Arduino)
LCD GND ──────┼──►
KY-040 GND ───┘
```

---

## Testing Procedure

### Step 1: Test Power Rails
1. Connect USB power
2. **Measure voltage** between Arduino 5V and GND
3. **Expected:** 4.8-5.2V
4. **If wrong:** Check power bank, USB cable

### Step 2: Test Pull-Ups
1. Disconnect encoder A wire from D2
2. **Measure voltage** on D2 pin
3. **Expected:** ~5V (pulled high by resistor)
4. **If wrong:** Check resistor connections

### Step 3: Test Encoder Signals
1. Connect encoder fully
2. Spin encoder wheel slowly
3. **Measure voltage** on D2/D3 while spinning
4. **Expected:** Voltage toggles between 0V and 5V
5. **If wrong:** Check encoder power, pull-ups

### Step 4: Test I2C Communication
1. Upload a simple I2C scanner sketch
2. **Expected:** LCD address detected (0x27 or 0x3F)
3. **If wrong:** Check SDA/SCL connections

---

## Quick Reference Card (Print This!)

```
┌─────────────────────────────────────────────────────┐
│         IRONTRAK QUICK CONNECTION GUIDE             │
├─────────────────────────────────────────────────────┤
│ INDUSTRIAL ENCODER:                                 │
│  Red ───► 5V    Green ───► D2 + 4.7kΩ to 5V       │
│  Black ─► GND   White ───► D3 + 4.7kΩ to 5V       │
│                                                     │
│ KY-040 ENCODER:                                     │
│  + ─────► 5V    CLK ──► D4                         │
│  GND ───► GND   DT ───► D5   SW ──► D6             │
│                                                     │
│ LCD I2C:                                            │
│  VCC ───► 5V    SDA ──► A4                         │
│  GND ───► GND   SCL ──► A5                         │
│                                                     │
│ POWER: USB Power Bank ──► Arduino USB Port         │
└─────────────────────────────────────────────────────┘
```

---

**Ready to wire it up? Start with the power connections, then add one module at a time and test!**
