# IronTrak PCB Design Guide for KiCad

**Complete schematic and component specification for PCB layout**

---

## 📐 Black Pill Pinout (Pins to Break Out)

### Left Header (2×20 Female Socket)

```
┌──────────────────────────────────┐
│  STM32F411 Black Pill LEFT SIDE  │
├─────┬────────────────────────────┤
│ Pin │ Function (IronTrak Usage)  │
├─────┼────────────────────────────┤
│ B12 │ Menu Encoder CLK           │ ← BREAKOUT to screw terminal
│ B13 │ Menu Encoder DT            │ ← BREAKOUT to screw terminal
│ B14 │ Menu Encoder SW            │ ← BREAKOUT to screw terminal
│ B15 │ (Unused - available)       │
│ A8  │ (Unused - available)       │
│ A9  │ (Unused - available)       │
│ A10 │ (Unused - available)       │
│ A15 │ (Unused - available)       │
│ B3  │ (Unused - available)       │
│ B4  │ (Unused - available)       │
│ B5  │ (Unused - available)       │
│ B6  │ Main Encoder A (TIM4_CH1)  │ ← BREAKOUT to screw terminal
│ B7  │ Main Encoder B (TIM4_CH2)  │ ← BREAKOUT to screw terminal
│ B8  │ I2C SCL                    │ ← BREAKOUT to I2C connector
│ B9  │ I2C SDA                    │ ← BREAKOUT to I2C connector
│ 5V  │ 5V Power Input             │ ← CONNECT to 5V power rail
│ GND │ Ground                     │ ← CONNECT to GND rail
│ 3V3 │ 3.3V Output (150mA max)    │ ← CONNECT to 3.3V power rail
│ VB  │ VBAT (not used)            │
│ C13 │ Onboard LED (don't use)    │
└─────┴────────────────────────────┘
```

### Right Header (2×20 Female Socket)

```
┌──────────────────────────────────┐
│  STM32F411 Black Pill RIGHT SIDE │
├─────┬────────────────────────────┤
│ Pin │ Function (IronTrak Usage)  │
├─────┼────────────────────────────┤
│ B10 │ (Unused - available)       │
│ B2  │ (Unused - available)       │
│ B1  │ (Unused - available)       │
│ B0  │ (Unused - available)       │
│ A7  │ SPI MOSI (Flash - used)    │ ⚠️ DO NOT USE (external flash)
│ A6  │ SPI MISO (Flash - used)    │ ⚠️ DO NOT USE (external flash)
│ A5  │ SPI SCK (Flash - used)     │ ⚠️ DO NOT USE (external flash)
│ A4  │ SPI CS (Flash - used)      │ ⚠️ DO NOT USE (external flash)
│ A3  │ (Unused - available)       │
│ A2  │ (Unused - available)       │
│ A1  │ (Unused - available)       │
│ A0  │ (Unused - available)       │
│ R   │ NRST (Reset button)        │
│ C15 │ (Unused - available)       │
│ C14 │ (Unused - available)       │
│ C13 │ (Unused - LED, don't use)  │
│ VB  │ VBAT (not used)            │
│ 3V3 │ 3.3V Output                │ ← CONNECT to 3.3V rail
│ GND │ Ground                     │ ← CONNECT to GND rail
│ GND │ Ground                     │ ← CONNECT to GND rail
└─────┴────────────────────────────┘
```

---

## 🔌 PCB Connector Pinouts

### Connector J1: Main Encoder (3-pin Screw Terminal)

```
Pin 1: Encoder A  ──► From R1 junction (PB6 + 4.7kΩ pull-up)
Pin 2: Encoder B  ──► From R2 junction (PB7 + 4.7kΩ pull-up)
Pin 3: GND        ──► GND rail
```

**External Wiring (to encoder):**

- Encoder Brown → 5V (separate power, not from PCB)
- Encoder Blue → GND (Pin 3)
- Encoder Black → Pin 1 (A channel)
- Encoder White → Pin 2 (B channel)

---

### Connector J2: I2C Bus (4-pin JST-XH or Header)

```
Pin 1: VCC (5V)   ──► 5V rail
Pin 2: GND        ──► GND rail
Pin 3: SDA        ──► From R3 junction (PB9 + optional 4.7kΩ pull-up)
Pin 4: SCL        ──► From R4 junction (PB8 + optional 4.7kΩ pull-up)
```

**External Wiring (to LCD):**

- LCD VCC → Pin 1
- LCD GND → Pin 2
- LCD SDA → Pin 3
- LCD SCL → Pin 4

---

### Connector J3: Menu Encoder (5-pin Header)

```
Pin 1: 3.3V       ──► 3.3V rail
Pin 2: GND        ──► GND rail
Pin 3: SW         ──► PB14 (direct connection)
Pin 4: DT         ──► PB13 (direct connection)
Pin 5: CLK        ──► PB12 (direct connection)
```

**External Wiring (to KY-040):**

- KY-040 + → Pin 1
- KY-040 GND → Pin 2
- KY-040 SW → Pin 3
- KY-040 DT → Pin 4
- KY-040 CLK → Pin 5

---

### Connector J4: Power Input (2-pin Screw Terminal)

```
Pin 1: +5V Input  ──► 5V rail (with reverse protection diode)
Pin 2: GND        ──► GND rail
```

**External Wiring:**

- USB Power Bank + → Pin 1
- USB Power Bank - → Pin 2

---

## 🧰 Complete Bill of Materials (BOM)

### Passive Components (Resistors)

| Ref    | Value     | Package             | Quantity | Purpose                      | Placement    |
| ------ | --------- | ------------------- | -------- | ---------------------------- | ------------ |
| **R1** | 4.7kΩ ±5% | 0805 SMD or 1/4W TH | 1        | Encoder A pull-up            | Near PB6 pin |
| **R2** | 4.7kΩ ±5% | 0805 SMD or 1/4W TH | 1        | Encoder B pull-up            | Near PB7 pin |
| **R3** | 4.7kΩ ±5% | 0805 SMD or 1/4W TH | 1        | I2C SDA pull-up (optional\*) | Near PB9 pin |
| **R4** | 4.7kΩ ±5% | 0805 SMD or 1/4W TH | 1        | I2C SCL pull-up (optional\*) | Near PB8 pin |
| **R5** | 1kΩ       | 0805 SMD or 1/4W TH | 1        | Power LED current limit      | Near LED1    |
| **R6** | 1kΩ       | 0805 SMD or 1/4W TH | 1        | Status LED current limit     | Near LED2    |

**Total Resistors:** 6 (or 4 if skipping R3/R4)

**\*Note:** R3/R4 optional if LCD backpack has onboard pull-ups. Add solder jumpers JP1/JP2 to enable/disable.

---

### Passive Components (Capacitors)

| Ref    | Value | Type          | Voltage | Package          | Quantity | Purpose                   | Placement                |
| ------ | ----- | ------------- | ------- | ---------------- | -------- | ------------------------- | ------------------------ |
| **C1** | 10μF  | Electrolytic  | 16V     | Radial 5mm       | 1        | 5V bulk decoupling        | Near J4 power input      |
| **C2** | 0.1μF | Ceramic (X7R) | 16V     | 0805 SMD or disc | 1        | 5V high-freq decoupling   | Parallel with C1         |
| **C3** | 10μF  | Electrolytic  | 16V     | Radial 5mm       | 1        | 3.3V bulk decoupling      | Near Black Pill 3.3V pin |
| **C4** | 0.1μF | Ceramic (X7R) | 16V     | 0805 SMD or disc | 1        | 3.3V high-freq decoupling | Parallel with C3         |
| **C5** | 0.1μF | Ceramic (X7R) | 16V     | 0805 SMD or disc | 1        | STM32 VDD decoupling      | Near Black Pill center   |
| **C6** | 0.1μF | Ceramic (X7R) | 16V     | 0805 SMD or disc | 1        | Encoder power decoupling  | Near J1 connector        |

**Total Capacitors:** 6

---

### Active Components

| Ref      | Component             | Package            | Quantity | Purpose                     |
| -------- | --------------------- | ------------------ | -------- | --------------------------- |
| **U1**   | STM32F411 Black Pill  | 2×20 pin module    | 1        | Microcontroller             |
| **D1**   | 1N5819 Schottky Diode | DO-41 or SMB       | 1        | Reverse polarity protection |
| **LED1** | Red LED 3mm           | 3mm TH or 0805 SMD | 1        | Power indicator (5V)        |
| **LED2** | Green LED 3mm         | 3mm TH or 0805 SMD | 1        | Status indicator (optional) |

**Total Active:** 4 components

---

### Connectors & Sockets

| Ref            | Component            | Type         | Quantity | Purpose               |
| -------------- | -------------------- | ------------ | -------- | --------------------- |
| **CON1, CON2** | 2×20 Female Header   | 2.54mm pitch | 2        | Black Pill socket     |
| **J1**         | 3-pin Screw Terminal | 5mm pitch    | 1        | Main encoder          |
| **J2**         | 4-pin JST-XH         | 2.5mm pitch  | 1        | I2C bus (LCD)         |
| **J3**         | 5-pin Header         | 2.54mm pitch | 1        | Menu encoder (KY-040) |
| **J4**         | 2-pin Screw Terminal | 5mm pitch    | 1        | Power input           |

**Total Connectors:** 7

---

### Optional Components (Configurability)

| Ref     | Component     | Type  | Purpose                             |
| ------- | ------------- | ----- | ----------------------------------- |
| **JP1** | Solder Jumper | 2-pad | Enable/disable R3 (I2C SDA pull-up) |
| **JP2** | Solder Jumper | 2-pad | Enable/disable R4 (I2C SCL pull-up) |
| **JP3** | Solder Jumper | 3-pad | Select 3.3V or 5V for I2C VCC rail  |

---

## 📋 Complete KiCad Schematic (Text Representation)

```
Power Section:
┌─────────────────────────────────────────────────┐
│  J4 (Power Input)                               │
│  ╔═══╗                                          │
│  ║ + ║──┬─► [D1: 1N5819] ──┬─► [C1: 10μF] ──┬── 5V Rail
│  ║ - ║  │                   │   [C2: 0.1μF]  │
│  ╚═══╝  │                   └────────┬─────────┘
│         │                            │
│         └────────────────────────────┴── GND Rail
└─────────────────────────────────────────────────┘

3.3V Generation (from Black Pill LDO):
┌─────────────────────────────────────────────────┐
│  Black Pill 3.3V Output                         │
│         │                                        │
│         ├─► [C3: 10μF] ──┬── 3.3V Rail          │
│         │   [C4: 0.1μF]  │                       │
│         └────────┬───────┘                       │
│                  │                                │
│                  └── GND Rail                     │
└─────────────────────────────────────────────────┘

Encoder A/B Pull-Ups:
┌─────────────────────────────────────────────────┐
│       3.3V Rail                                 │
│          │                                       │
│          ├─► [R1: 4.7kΩ] ──┬─► PB6 (Black Pill) │
│          │                  └─► J1 Pin1 (Enc A) │
│          │                                       │
│          └─► [R2: 4.7kΩ] ──┬─► PB7 (Black Pill) │
│                             └─► J1 Pin2 (Enc B) │
└─────────────────────────────────────────────────┘

I2C Pull-Ups (Optional):
┌─────────────────────────────────────────────────┐
│       3.3V Rail                                 │
│          │                                       │
│     [JP1]├─► [R3: 4.7kΩ] ──┬─► PB9 (SDA)       │
│          │                  └─► J2 Pin3         │
│          │                                       │
│     [JP2]└─► [R4: 4.7kΩ] ──┬─► PB8 (SCL)       │
│                             └─► J2 Pin4         │
└─────────────────────────────────────────────────┘

Menu Encoder (Direct Connection):
┌─────────────────────────────────────────────────┐
│  PB12 ───────────► J3 Pin5 (CLK)                │
│  PB13 ───────────► J3 Pin4 (DT)                 │
│  PB14 ───────────► J3 Pin3 (SW)                 │
│  3.3V ───────────► J3 Pin1 (+)                  │
│  GND  ───────────► J3 Pin2 (GND)                │
└─────────────────────────────────────────────────┘

Power Indicators:
┌─────────────────────────────────────────────────┐
│  5V Rail ──► [R5: 1kΩ] ──► [LED1: Red] ──► GND │
│  3.3V ────► [R6: 1kΩ] ──► [LED2: Green] ─► GND │
└─────────────────────────────────────────────────┘
```

---

## 🎨 PCB Layout Recommendations

### Board Dimensions

- **Size:** 80mm × 60mm (compact, fits standard enclosures)
- **Layers:** 2-layer (Top + Bottom, Ground plane on bottom)
- **Thickness:** 1.6mm standard FR4

### Component Placement

```
┌────────────────────────────────────────────────┐
│  TOP LAYER                                     │
│                                                 │
│  [J4 Power] [LED1] [LED2]  ← Left edge         │
│     ↓                                           │
│  [D1] [C1][C2]                                  │
│                                                 │
│              ┌──────────────────┐               │
│              │  STM32 Black Pill│               │
│              │    (2×20 Socket) │               │
│              │                  │               │
│              │   [C5] near VDD  │               │
│              └──────────────────┘               │
│                                                 │
│  [R1][R2]  [R3][R4]  [C3][C4]                  │
│    ↓         ↓                                  │
│  [J1]      [J2]      [J3]  ← Right edge        │
│  Encoder   I2C       Menu                      │
└────────────────────────────────────────────────┘
```

### Trace Width Guidelines

| Net               | Width           | Notes                             |
| ----------------- | --------------- | --------------------------------- |
| **5V Power**      | 0.5mm (20 mil)  | 500mA max current                 |
| **3.3V Power**    | 0.4mm (16 mil)  | 200mA max current                 |
| **GND**           | Polygon pour    | Solid copper plane                |
| **Signals**       | 0.25mm (10 mil) | Standard traces                   |
| **I2C (SDA/SCL)** | 0.3mm (12 mil)  | Slightly wider for noise immunity |

### Ground Plane Strategy

- **Bottom layer:** Solid GND plane (connected via vias)
- **Top layer:** GND fill around components (avoid signals)
- **Via stitching:** Place GND vias every 10mm along power traces

---

## ✅ KiCad Design Checklist

### Schematic Symbol Library

- [ ] Import STM32F411 Black Pill symbol (or create 2×20 header)
- [ ] Add resistor footprints (0805 SMD or axial TH)
- [ ] Add capacitor footprints (0805 SMD or radial TH)
- [ ] Add screw terminal symbols (Phoenix Contact or similar)
- [ ] Add JST-XH connector symbol

### Assign Footprints

- [ ] Black Pill socket: `PinSocket_2x20_P2.54mm_Vertical`
- [ ] Resistors: `R_0805_2012Metric` or `R_Axial_DIN0207_L6.3mm`
- [ ] Ceramic caps: `C_0805_2012Metric` or `C_Disc_D5.0mm`
- [ ] Electrolytic caps: `CP_Radial_D5.0mm_P2.00mm`
- [ ] Screw terminals: `TerminalBlock_Phoenix_PT-1,5-3-5.0-H`
- [ ] JST-XH: `JST_XH_B4B-XH-A_1x04_P2.50mm_Vertical`

### PCB Layout

- [ ] Set board outline (80×60mm rectangle)
- [ ] Place mounting holes (M3, 4 corners)
- [ ] Place Black Pill socket in center
- [ ] Place connectors along edges (J1, J2, J3 right, J4 left)
- [ ] Place passive components near associated pins
- [ ] Route power traces first (thick: 0.5mm)
- [ ] Route signal traces (thin: 0.25mm)
- [ ] Add ground plane (bottom layer)
- [ ] Add silkscreen labels (pin numbers, polarity marks)
- [ ] Add component reference designators

### Design Rule Check (DRC)

- [ ] Minimum trace width: 0.25mm
- [ ] Minimum clearance: 0.2mm
- [ ] Via size: 0.8mm drill, 1.5mm pad
- [ ] Pad-to-hole ratio: >1.8:1

---

## 📦 Manufacturing Files Export

### Gerber Files (for JLCPCB, PCBWay, OSH Park)

- `*-F_Cu.gbr` (Top copper layer)
- `*-B_Cu.gbr` (Bottom copper layer)
- `*-F_Mask.gbr` (Top solder mask)
- `*-B_Mask.gbr` (Bottom solder mask)
- `*-F_SilkS.gbr` (Top silkscreen)
- `*-B_SilkS.gbr` (Bottom silkscreen)
- `*-Edge_Cuts.gbr` (Board outline)
- `*.drl` (Drill file)

### BOM Export (CSV)

```csv
Ref,Value,Package,Quantity,Supplier
R1,4.7k,0805,1,Mouser/Digikey
R2,4.7k,0805,1,Mouser/Digikey
...
```

---

**This document is KiCad-ready!** All pinouts, components, and connections are specified for immediate PCB design. 🚀

---

**Last Updated:** 2025-11-28  
**PCB Version:** v1.0  
**KiCad Compatibility:** 6.0+
