# IronTrak Assembly Standards

**Comprehensive technical assembly documentation for Perfboard prototyping and PCB production builds**

---

## 📋 Table of Contents

- [Overview & Quality Tiers](#overview--quality-tiers)
- [Tools & Equipment Requirements](#tools--equipment-requirements)
- [Materials & Consumables](#materials--consumables)
- [Perfboard Assembly Standards](#perfboard-assembly-standards)
- [PCB Assembly Standards](#pcb-assembly-standards)
- [Comparative Matrix](#comparative-matrix-medium-vs-high-standards)
- [Component-Specific Guidelines](#component-specific-guidelines)
- [Material Handling Compliance](#material-handling-compliance)
- [Workflow Checklists](#workflow-checklists)

---

## 🎯 Overview & Quality Tiers

### Philosophy

IronTrak is designed for **shop floor deployment** in harsh industrial environments with vibration, metal dust, oil contamination, and temperature variation. Assembly quality directly impacts measurement reliability—a mission-critical requirement per GEMINI.md Section 3.2.

This document defines **two quality tiers** aligned with IPC standards for electronic assembly:

### Quality Tier Definitions

#### Medium Standards (IPC Class 2 Equivalent)

**Application:** Functional prototyping, development testing, low-production builds

**Priority:** Functionality and serviceability  
**Environment:** Benchtop testing, controlled indoor environments  
**Rework:** Permitted (up to 2 cycles)  
**Inspection:** Visual inspection with limited magnification  
**Cleaning:** Optional (flux residue acceptable if non-conductive)

**Use Cases:**

- Initial breadboard-to-Perfboard builds
- Software development test fixtures
- Pre-production validation units
- Educational/demonstration units

---

#### High Standards (IPC Class 3 Equivalent)

**Application:** Production builds, shop floor deployment, high-reliability systems

**Priority:** Long-term reliability and environmental resistance  
**Environment:** Industrial shop floor (vibration, oil, metal dust, -10°C to 70°C)  
**Rework:** Severely restricted (zero rework on critical components)  
**Inspection:** 10x magnification mandatory, electrical testing required  
**Cleaning:** Mandatory (zero flux residue tolerance)  
**Protection:** Conformal coating required

**Use Cases:**

- All units deployed to bandsaw/metalworking environments
- Final production assemblies
- Safety-critical measurement systems
- Long-term deployed units (>1 year continuous operation)

---

### When to Use Each Tier

**Use Medium Standards if:**

- ✅ Unit will remain on benchtop or in clean workshop
- ✅ Frequent firmware updates/modifications expected
- ✅ Quick turnaround needed for testing
- ✅ Budget constraints exist

**Use High Standards if:**

- ✅ Unit will be exposed to oil, metal dust, or vibration
- ✅ Long-term reliability is critical (>6 months continuous operation)
- ✅ Measurement accuracy cannot be compromised
- ✅ Limited access for maintenance/repair

---

## 🔧 Tools & Equipment Requirements

### Soldering Equipment

#### Medium Standards

| Tool                | Specification                   | Notes                                          |
| ------------------- | ------------------------------- | ---------------------------------------------- |
| Soldering Iron      | 30-60W, adjustable temperature  | Minimum 320-380°C range                        |
| Tip Geometry        | Chisel (2-3mm) or conical (1mm) | Chisel for through-hole, conical for fine work |
| Temperature Control | ±10°C accuracy                  | Digital display preferred                      |
| Tip Cleaning        | Brass wool + wet sponge         | Clean before each joint                        |
| Soldering Stand     | ESD-safe holder                 | Heat-resistant base                            |

#### High Standards

| Tool                | Specification                                | Notes                                     |
| ------------------- | -------------------------------------------- | ----------------------------------------- |
| Soldering Station   | 60-90W, PID temperature control              | ±5°C accuracy mandatory                   |
| Tip Geometry        | Multiple tips (chisel, knife, conical 0.5mm) | Match tip to component size               |
| Hot Air Rework      | 100-450°C, variable airflow                  | For SMT rework and heat shrink            |
| Preheating Platform | 100-150°C uniform heating                    | Reduces thermal shock on BGAs/MCUs        |
| Fume Extraction     | Active carbon filter                         | Protects operator, prevents contamination |

---

### Inspection Equipment

#### Medium Standards

- **Magnification:** 3x hands-free loupe or reading glasses
- **Lighting:** Bright white LED work light (minimum 500 lumens)
- **Multimeter:** Basic DMM (continuity, resistance, voltage)
  - Accuracy: ±1% for resistance measurements
  - Test leads: Probes with sharp tips for component testing

#### High Standards

- **Magnification:** 10x stereo microscope or illuminated magnifier
- **Lighting:** Adjustable LED ring light (minimum 1000 lumens, 5000K color temp)
- **Multimeter:** Precision DMM with data logging
  - Accuracy: ±0.1% for critical measurements
  - Insulation resistance: >100MΩ range
- **Optional:** USB microscope (50-1000x) for solder joint documentation
- **Optional:** Thermal camera for power trace verification (detect hot spots)

---

### ESD Protection Requirements

#### Medium Standards (Basic ESD Awareness)

- **Wrist Strap:** 1MΩ resistor, connected to earth ground
- **Work Surface:** ESD mat (grounded) preferred but not mandatory
- **Component Handling:** Avoid touching IC pins, store in conductive foam

#### High Standards (Class 1 ESD Sensitivity Protocol)

- **Wrist Strap:** 1MΩ resistor, continuous ground monitoring
- **Work Surface:** Grounded ESD mat (blue/black) with 10^6-10^9Ω resistance
- **Ionizer:** Benchtop ionizer for neutralizing charged insulators
- **Footwear:** ESD-safe shoes or heel straps
- **Clothing:** Static-dissipative smock (no synthetic fabrics)
- **Packaging:** Store assembled units in metallized ESD bags with humidity indicators
- **Verification:** Daily wrist strap and mat resistance testing

> [!CAUTION] > **STM32F4 ESD Sensitivity:** The STM32F411CE is **Class 1 ESD sensitive** (human body model <2kV). Failure to follow High Standards ESD protocols can cause latent defects that manifest as intermittent failures in the field.

---

## 🧪 Materials & Consumables

### Solder Selection

#### Recommended Alloys

**Lead-Free (RoHS Compliant):**

- **SAC305** (Sn96.5/Ag3.0/Cu0.5)
  - **Melting Point:** 217-220°C
  - **Working Temp:** 340-370°C
  - **Pros:** Industry standard, good mechanical strength, RoHS compliant
  - **Cons:** Higher melting point, requires higher iron temperature
  - **Use:** Production builds, export units, environmental compliance required

**Leaded (Legacy/Prototype):**

- **Sn63/Pb37** (63% Tin, 37% Lead)
  - **Melting Point:** 183°C (eutectic)
  - **Working Temp:** 320-350°C
  - **Pros:** Lower temperature, faster wetting, easier for hand soldering
  - **Cons:** Not RoHS compliant, health hazard (use fume extraction)
  - **Use:** Prototyping, rework, legacy component compatibility

#### Wire Gauge Recommendations

- **Through-Hole (0.8-1.0mm):** General purpose, most passive components
- **Fine Pitch (0.5mm):** SMT passives, tight spacing (e.g., TSSOP pin repair)
- **Heavy Power (1.5mm):** Power connectors, high-current traces

---

### Flux Selection

#### Medium Standards

- **Type:** No-Clean Rosin Flux (ROL0 or ROL1)
- **Form Factor:** Flux pen or syringe for spot application
- **Activity Level:** Low to medium (mildly activated)
- **Residue:** Non-conductive, cosmetic only (cleaning optional)
- **Example:** Kester 951 or MG Chemicals 8341

#### High Standards

- **Type:** Water-Soluble Organic Flux (ORL0)
- **Form Factor:** Syringe for precision application
- **Activity Level:** Medium (more aggressive wetting, faster action)
- **Residue:** **MUST be cleaned** (hygroscopic, can corrode traces over time)
- **Cleaning:** Deionized water rinse + IPA wipe, or ultrasonic bath
- **Example:** Kester 2331-ZX or Amtech RMA-223

> [!WARNING] > **Flux Residue in High-Vibration Environments:** Uncleaned flux residue can trap metal dust and absorb oil in shop floor conditions, creating conductive paths that cause intermittent shorts. High Standards **require** complete flux removal via IPA (≥99%) or ultrasonic cleaning.

---

### Cleaning Agents (High Standards Only)

| Agent                       | Concentration             | Application                          | Notes                                     |
| --------------------------- | ------------------------- | ------------------------------------ | ----------------------------------------- |
| **Isopropyl Alcohol (IPA)** | ≥99%                      | Post-solder flux removal             | Evaporates quickly, no residue            |
| **Deionized Water**         | 18.2 MΩ·cm                | Ultrasonic bath (water-soluble flux) | Must dry completely (bake at 60°C, 30min) |
| **Surfactant Solution**     | 5% saponifier in DI water | Heavy flux contamination             | Rinse with DI water, then IPA             |
| **Compressed Air (Dry)**    | Oil-free, desiccant-dried | Final drying after wet cleaning      | Prevents moisture entrapment              |

**Ultrasonic Cleaning Protocol (Production Builds >10 Units):**

1. Pre-rinse with IPA to remove bulk flux
2. Ultrasonic bath in DI water + surfactant (5 min, 40kHz, 40°C)
3. DI water rinse (2 min)
4. IPA rinse (1 min)
5. Compressed air drying
6. Bake at 60°C for 30 min (removes moisture from under components)

---

## 🛠️ Perfboard Assembly Standards

### Medium Standards (Prototyping)

#### Point-to-Point Wiring Techniques

**Component Lead Preparation:**

1. **Bending:** Use needle-nose pliers, bend leads at 90° close to component body
2. **Spacing:** Match lead spacing to Perfboard hole pitch (0.1" / 2.54mm standard)
3. **Trimming:** Trim leads **after** soldering, leave 1-2mm stub above solder joint
4. **Orientation:** Orient resistors/capacitors horizontally for parallel traces

**Soldering Profile:**

- **Iron Temperature:** 320-350°C (lead-free SAC305) or 300-330°C (Sn63/Pb37)
- **Dwell Time:** 2-3 seconds per joint
- **Solder Application:** Touch solder wire to joint (not iron tip), allow wetting
- **Joint Appearance:** Concave fillet, complete wetting around lead and pad

**Acceptable Solder Joint Geometry:**

```
  Component Lead
       │
   ╱───┴───╲   ← Concave fillet (good wetting)
  │         │
  └─────────┘   ← Perfboard pad
```

**Defective Joints (Reject):**

- **Cold Joint:** Dull, grainy appearance (insufficient heat)
- **Bridging:** Solder connecting adjacent pads/traces
- **Insufficient Solder:** Lead visible through joint, weak mechanical bond
- **Overheated:** Discolored pads, lifted traces, charred insulation

---

#### Wire Routing & Strain Relief

**Wire Gauge Selection:**

- **Signal Lines:** 22-24 AWG stranded (encoder A/B, I2C SDA/SCL, menu inputs)
- **Power Lines:** 18-20 AWG stranded (5V/GND from USB to MCU)
- **Encoder Quadrature:** 24 AWG twisted pair (reduces EMI crosstalk)

**Color Coding Convention:**
| Wire Color | Function |
|------------|----------|
| **Red** | +5V Power |
| **Black** | Ground (GND) |
| **Yellow** | Encoder Channel A |
| **Green** | Encoder Channel B |
| **Blue** | I2C SDA |
| **White** | I2C SCL |
| **Orange** | Menu encoder signals |

**Mechanical Anchoring (Vibration Resistance):**

- **Cable Ties:** Secure wire bundles to Perfboard edges (use nylon ties, not metal)
- **Hot Glue:** Apply to connectors and high-stress solder joints (optional, reduces rework ease)
- **Strain Relief:** Loop wires before entering connectors, prevents tension on solder joints
- **Routing:** Avoid sharp bends (minimum 3mm radius), prevents wire fatigue

---

#### Inspection Criteria (Medium)

**Visual Inspection Checklist:**

- [ ] All solder joints are shiny and concave (no dull/grainy appearance)
- [ ] No solder bridges between adjacent pads
- [ ] Component leads are fully wetted (no gaps between solder and lead)
- [ ] Wire insulation is intact (no melted or damaged insulation near joints)
- [ ] Polarity-sensitive components oriented correctly (check silkscreen/schematic)

**Electrical Testing:**

- [ ] Continuity test: All expected connections verified with multimeter beep
- [ ] Resistance check: Pull-up resistors measure 4.5-5.0kΩ (±5% tolerance)
- [ ] Power rails: 5V measures 4.75-5.25V, GND is 0V
- [ ] No shorts: >1MΩ resistance between 5V and GND with power off

**Acceptable Defects (Medium Standards):**

- ✅ Minor flux residue (if non-conductive, non-corrosive)
- ✅ Visible wire insulation within 2mm of solder joint (if mechanically secured)
- ✅ Small solder blobs on component body (cosmetic only, no bridging)

**Pull Test Requirements:**

- **Force:** 200g (approximately 2N) applied to component leads
- **Pass Criteria:** Component does not shift, solder joint remains intact
- **Test Method:** Gently tug lead with tweezers, visual inspection for movement

---

### High Standards (Pre-Production Prototyping)

#### Enhanced Soldering Techniques

**Controlled Thermal Profile:**

1. **Preheat:** Place Perfboard on preheating platform at 150°C for 30 seconds (reduces thermal shock)
2. **Joint Soldering:** Iron tip temperature 360-370°C (SAC305), dwell time 1-2 seconds
3. **Cooling:** Allow natural cooling (no forced air), prevents stress fractures

**Solder Fillet Height Requirements:**

- **Minimum:** 50% of pad diameter
- **Ideal:** Solder climbs lead to component body edge (maximum wetting)
- **Concave Profile:** Smooth transition from pad to lead (no sharp angles)

**Flux Residue Cleaning (Mandatory):**

1. IPA wipe (≥99% concentration) with lint-free cloth
2. Inspection under 10x magnification (no visible residue)
3. Optional: Ultrasonic bath (5 min, IPA solution)

---

#### Advanced Wiring Standards

**Twisted Pair Requirements (Quadrature Encoder):**

- **Cables:** Encoder A/B channels must use twisted pair (3-5 twists per inch)
- **Rationale:** Reduces electromagnetic interference (EMI) and crosstalk
- **Termination:** Untwist only final 10mm before soldering to pads

**Ferrite Bead Placement (EMI Suppression):**

- **Component:** Ferrite bead (300Ω @ 100MHz, e.g., Würth 742 792 093)
- **Location:** On encoder power line (5V), close to encoder connector
- **Purpose:** Suppresses high-frequency noise from encoder switching

**Conformal Coating Application:**

- **Material:** Acrylic (MG Chemicals 422B) or Urethane (Humiseal 1A33)
- **Thickness:** 25-50μm (2-3 coats with brush or aerosol spray)
- **Masking:** Mask connectors, switches, test points before coating
- **Curing:** 24 hours at room temperature, or 2 hours at 60°C
- **Purpose:** Protects against oil, metal dust, and moisture ingress

---

#### Strict Inspection Criteria (High)

**10x Magnification Inspection:**

- [ ] Every solder joint inspected under 10x stereo microscope
- [ ] Fillet geometry verified (90° contact angle minimum, smooth concave profile)
- [ ] Zero voids visible in solder joint (indicates proper wetting)
- [ ] No flux residue visible (cleaned surface only)

**Zero Flux Residue Tolerance:**

- **Test:** UV light inspection (flux residue fluoresces under 365nm UV)
- **Pass Criteria:** No fluorescence visible on or around solder joints

**Insulation Resistance Testing:**

- **Equipment:** Megohmmeter or precision multimeter (>10MΩ range)
- **Test Voltage:** 50V DC between adjacent traces
- **Pass Criteria:** >10MΩ resistance (ideally >100MΩ)
- **Purpose:** Detects contamination, flux residue, or moisture ingress

**Pull Test Requirements (High Standards):**

- **Force:** 500g minimum (approximately 5N) for critical signals
  - Encoder A/B inputs: 500g
  - I2C SDA/SCL: 500g
  - Power/GND: 800g
- **Test Method:** Calibrated force gauge or spring scale
- **Pass Criteria:** Zero movement, no solder joint cracking

---

## 📐 Perfboard Layout Diagram

### Complete Bill of Materials (Passive Components)

| Component  | Value             | Quantity | Purpose                             | Notes                           |
| ---------- | ----------------- | -------- | ----------------------------------- | ------------------------------- |
| **R1, R2** | 4.7kΩ ±5%         | 2        | Encoder A/B pull-ups                | 1/4W or 1/8W carbon/metal film  |
| **R3, R4** | 4.7kΩ ±5%         | 2        | I2C SDA/SCL pull-ups                | _Optional - check LCD backpack_ |
| **C1**     | 10μF electrolytic | 1        | 5V power rail bulk decoupling       | 16V+ rating                     |
| **C2**     | 0.1μF ceramic     | 1        | STM32 VDD high-frequency decoupling | X7R/X5R dielectric              |
| **C3**     | 10μF electrolytic | 1        | 3.3V rail bulk decoupling           | 16V+ rating                     |
| **C4**     | 0.1μF ceramic     | 1        | Encoder power decoupling            | Optional but recommended        |

**Total Passive Components:** 4-8 (minimum 6 for basic build)

---

### Perfboard Layout (Top View)

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         IronTrak Perfboard Layout                        │
│                          (60 holes × 40 holes)                           │
└─────────────────────────────────────────────────────────────────────────┘

        COLUMN:  5   10   15   20   25   30   35   40   45   50   55
                 │    │    │    │    │    │    │    │    │    │    │
        ROW  5 ──┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼──
                 │                   ┌─────────────────────┐
             10 ─┤                   │   20×4 LCD I2C      │
                 │                   │   0x27 Address      │
             15 ─┤                   │  [VCC GND SDA SCL]  │
                 │                   └──────────┬──────────┘
             20 ─┤                              │
                 │         ┌───────────────────────────────┐
             25 ─┤         │  STM32F411 Black Pill         │
                 │         │  ┌─────────────────────┐      │
             30 ─┤         │  │ 5V  GND  3.3V       │      │
                 │         │  │ PB9 PB8  PB7  PB6   │      │
             35 ─┤         │  │ PB14 PB13 PB12      │      │
                 │         │  └─────────────────────┘      │
             40 ─┤         └───────────────────────────────┘
                 │              │   │    │    │
                 │         5V  GND  3.3V │
                 │          │   │    │   └────[R1: 4.7kΩ]───┐
                 │          │   │    └────[R2: 4.7kΩ]───┐   │
                 │          │   │                        │   │
        ROW 45 ──┤      [C1] [GND]   [R3] [R4]          │   │
                 │      10μF         4.7kΩ 4.7kΩ        │   │
                 │       │            │   │             │   │
             50 ─┤       │         PB9  PB8            PB6 PB7
                 │       │         (SDA)(SCL)          (A) (B)
                 │       │            │   │             │   │
             55 ─┤       │            │   │             │   │
                 │       │            └───┴─────────────┼───┼───► LCD I2C
                 │       │                              │   │
             60 ─┤       └──────────────────────────────┼───┼───► Encoder
                 │                                      │   │
        ─────────┴──────────────────────────────────────┴───┴─────────────

Legend:
  [C1] = Capacitor 10μF electrolytic (power bulk decoupling)
  [R1-R4] = Resistors 4.7kΩ (pull-ups for encoder and I2C)
  ──── = Wire connections (use color-coded 22-24 AWG stranded)
  PBx  = STM32 pin labels
```

---

### Connector Pinout Reference Card

**Print and laminate this for quick reference during assembly:**

```
╔════════════════════════════════════════

════════════════════════╗
║                 IronTrak Perfboard Pinout Reference            ║
╠════════════════════════════════════════════════════════════════╣
║  STM32F411 Black Pill (Critical Pins Only)                     ║
║  ┌─────────────────────────────────────────────────────┐      ║
║  │  LEFT HEADER         │  RIGHT HEADER                │      ║
║  ├──────────────────────┼──────────────────────────────┤      ║
║  │  5V  (Power In)      │  3.3V (Power Out)            │      ║
║  │  GND (Ground)        │  PB6  (Encoder A, TIM4_CH1)  │      ║
║  │  PB12 (Menu CLK)     │  PB7  (Encoder B, TIM4_CH2)  │      ║
║  │  PB13 (Menu DT)      │  PB8  (I2C SCL)              │      ║
║  │  PB14 (Menu SW)      │  PB9  (I2C SDA)              │      ║
║  └──────────────────────┴──────────────────────────────┘      ║
║                                                                 ║
║  20×4 LCD I2C (4-pin connector, address 0x27)                  ║
║  ┌────────────────────────────────────────────────────┐       ║
║  │  VCC ──► 5V rail      (Red wire)                   │       ║
║  │  GND ──► GND rail     (Black wire)                 │       ║
║  │  SDA ──► PB9          (Blue wire, via R3 optional) │       ║
║  │  SCL ──► PB8          (White wire, via R4 optional)│       ║
║  └────────────────────────────────────────────────────┘       ║
║                                                                 ║
║  Industrial Encoder (1024 PPR, NPN open collector)             ║
║  ┌────────────────────────────────────────────────────┐       ║
║  │  Brown  ──► 5V/12V    (Encoder power, check spec)  │       ║
║  │  Blue   ──► GND       (Ground)                     │       ║
║  │  Black  ──► PB6 + R1  (Channel A, yellow twisted)  │       ║
║  │  White  ──► PB7 + R2  (Channel B, green twisted)   │       ║
║  │  Orange ──► (Z index, not used)                    │       ║
║  └────────────────────────────────────────────────────┘       ║
║                                                                 ║
║  KY-040 Menu Encoder (5-pin)                                   ║
║  ┌────────────────────────────────────────────────────┐       ║
║  │  +   ──► 3.3V rail    (Orange)                     │       ║
║  │  GND ──► GND rail     (Black)                      │       ║
║  │  SW  ──► PB14         (Button, orange)             │       ║
║  │  DT  ──► PB13         (Quadrature B, orange)       │       ║
║  │  CLK ──► PB12         (Quadrature A, orange)       │       ║
║  └────────────────────────────────────────────────────┘       ║
║                                                                 ║
║  Passive Components:                                            ║
║  • R1, R2: 4.7kΩ ±5% (Encoder A/B pull-ups to 3.3V)           ║
║  • R3, R4: 4.7kΩ ±5% (I2C pull-ups, optional)                 ║
║  • C1: 10μF 16V (5V bulk decoupling, electrolytic)            ║
║  • C2: 0.1μF (STM32 VDD ceramic decoupling)                   ║
╚════════════════════════════════════════════════════════════════╝
```

---

See [HARDWARE.md](HARDWARE.md#wiring-diagrams) for complete wiring details and [BOM.md](../docs/BOM.md) for component sourcing.

---

## 🖥️ PCB Assembly Standards

### Medium Standards (Functional Testing)

#### Surface-Mount Passive Component Soldering (0805/0603)

**Hand Soldering Process:**

1. **Flux Application:** Apply small amount of flux pen to pads
2. **Tin One Pad:** Flow small amount of solder onto one pad
3. **Component Placement:** Hold component with tweezers, reflow solder on tinned pad
4. **Solder Second Pad:** Remove tweezers, solder opposite end
5. **Reflow First Pad:** Ensure both ends have equal solder volume

**Temperature Profile:**

- **Iron Temperature:** 340-360°C (lead-free)
- **Dwell Time:** 2-3 seconds per pad
- **Tip Geometry:** Chisel or knife (1-2mm width)

**Acceptable Fillet Geometry:**

- **Contact Angle:** Minimum 75° between solder and component body
- **Pad Coverage:** Solder covers 80-100% of pad area
- **Component Alignment:** Centered on pads (±0.2mm offset acceptable)

**Common Defects (Reject):**

- **Tombstoning:** Component standing upright (one end reflowed before the other)
- **Insufficient Solder:** Component floating, no fillet visible
- **Solder Balls:** Small solder spheres near joint (excess flux or moisture)

**Rework Allowances:**

- **Permitted:** Up to 2 rework cycles (heating) per component
- **Inspection:** After 2nd rework, inspect for pad damage or lifted traces
- **Rejection:** If 3rd rework needed, replace component or scrapped PCB section

---

#### Through-Hole Component Soldering

**Thermal Relief Pad Engagement:**

- **Spoke Count:** Minimum 2 thermal relief spokes connecting pad to plane
- **Spoke Width:** 0.3-0.5mm (design parameter, not assembly)
- **Solder Flow:** Solder should wet spoke connections (visible in through-hole)

**Hole-Fill Percentage:**

- **Minimum:** 50% of via/hole filled with solder (Medium Standard)
- **Inspection:** View hole from both sides (solder visible on bottom)
- **Acceptable:** Partial fill if mechanical strength adequate (component not load-bearing)

---

#### Inspection Criteria (Medium PCB)

**Visual Inspection:**

- [ ] All SMT components centered on pads (no severe misalignment)
- [ ] Solder fillets present on both ends of passives
- [ ] No tombstoning or floating components
- [ ] Through-hole joints have solder on both top and bottom sides

**Functional Testing:**

- [ ] Power-On Test: 5V rail measures 4.75-5.25V
- [ ] I2C Communication: LCD displays boot screen (confirms I2C functional)
- [ ] Encoder Pulse Verification: Rotate encoder, observe pulse count increment
- [ ] Button Inputs: Press menu button, verify response

**Acceptable Defects (Medium PCB):**

- ✅ Minor pad discoloration (oxidation, no functional impact)
- ✅ Flux residue if non-conductive (cosmetic only)
- ✅ Small solder bridges on non-critical traces (if intentionally connected)

---

### High Standards (Production)

#### Precision SMT Soldering

**Temperature Control:**

- **Iron Temperature:** 340-350°C ±5°C (tight control mandatory)
- **Preheat Platform:** 100-120°C (PCB-level preheating reduces thermal gradient)
- **Ambient Temperature:** 20-25°C (prevents cold solder joints in cold workshop)

**Fillet Geometry Requirements:**

- **Contact Angle:** 90° minimum (ideal: 100-120° for maximum strength)
- **Wetting:** Complete wetting of pad and component termination
- **Void-Free:** Zero voids >25% of joint area (X-ray inspection if critical)
- **Heel Fillet:** Solder climbs component body edge (maximum wetting indication)

**Zero Rework on Critical Components:**

- **Encoder Input Resistors (R1-R4):** 4.7kΩ pull-ups on PB6/PB7
- **MCU Power Decoupling (C1-C4):** 0.1μF ceramics near STM32 VDD pins
- **I2C Pull-ups (R5-R6):** 4.7kΩ on SDA/SCL lines
- **Rationale:** Rework thermal cycles can create latent defects (lifted pads, cracked solder)

---

#### Thermal Relief & Pad Geometry Standards

> [!NOTE]
> These are **design** guidelines for PCB layout, included here for assembly context.

**Thermal Relief Specifications (Design):**

- **Spoke Count:** Minimum 2, ideally 4 for ground/power planes
- **Spoke Width:** 0.3mm minimum (balances thermal conductivity and solderability)
- **Spoke Angle:** 45° or 90° orientation (prevents accidental bridging during soldering)

**Pad-to-Hole Ratio (Through-Hole):**

- **Minimum:** 1.8:1 (e.g., 1.0mm hole → 1.8mm pad diameter)
- **Ideal:** 2.0:1 for high-reliability
- **Purpose:** Ensures adequate annular ring after drilling tolerances

**Annular Ring Requirements:**

- **Minimum:** 0.2mm after drilling tolerance (prevents pad breakout during rework)
- **Inspection:** Visual verification under 10x magnification

**Via Treatment (Power/Ground):**

- **Tenting:** Solder mask over via (prevents solder wicking during assembly)
- **Filling:** Conductive epoxy fill for thermal vias (improves heat dissipation)

---

#### Advanced Inspection Protocols

**AOI-Equivalent Manual Inspection:**

- **Magnification:** 10-20x stereo microscope
- **Lighting:** Coaxial ring light (eliminates shadows, reveals voids)
- **Criteria:**
  - [ ] 100% pad coverage on all SMT components
  - [ ] No component misalignment (maximum 10% off-center)
  - [ ] No solder bridges (including under components)
  - [ ] Consistent fillet geometry across all joints

**X-Ray Inspection (Optional, for Critical Assemblies):**

- **Purpose:** Inspect BGA solder balls, QFN thermal pads, buried thermal vias
- **Equipment:** 2D X-ray (sufficient for IronTrak, no BGAs currently)
- **Inspection Points:** STM32F411CE Black Pill module pre-soldered connections (if reworking headers)

**Cross-Sectional Analysis Sampling (Production >10 Units):**

- **Frequency:** 1 sample per 25 units
- **Method:** Cut PCB through critical solder joint, mount in epoxy, polish, microscope inspection
- **Analysis:** Measure actual joint thickness, void percentage, intermetallic layer formation
- **Acceptance:** <5% void area, intermetallic layer 0.5-2μm thickness

**Insulation Resistance (High Standards):**

- **Test Voltage:** 100V DC
- **Pass Criteria:** >100MΩ between all isolated traces
- **Critical Pairs:** Test between:
  - 5V rail and GND
  - Encoder A and Encoder B
  - I2C SDA and SCL

---

#### ESD Protection Protocols (Class 1 Sensitivity)

**Assembly Environment:**

- **Workstation:** Full ESD workstation (mat, wrist strap, ionizer)
- **Handling:** Touch only PCB edges, avoid touching component pins
- **Tools:** ESD-safe tweezers, cutters, soldering iron tips (grounded)

**Packaging:**

- **Individual Units:** Metallized ESD bag (shielding bag, Faraday cage)
- **Humidity Indicator:** Include desiccant pack and humidity card (ensure <40% RH)
- **Labeling:** ESD warning label on exterior

**Verification:**

- **Daily:** Test wrist strap continuity (1MΩ ±10%)
- **Weekly:** Verify mat resistance to ground (10^6-10^9Ω)
- **Monthly:** Ionizer balance test (±50V maximum offset)

---

#### Post-Assembly Cleaning (Mandatory)

**Ultrasonic Cleaning Cycle:**

1. **Pre-clean:** Rinse PCB with IPA (removes bulk flux)
2. **Ultrasonic Bath:**
   - Solution: DI water + 5% saponifier (or IPA for no-clean flux)
   - Frequency: 40kHz
   - Temperature: 40°C
   - Duration: 5 minutes
3. **Rinse:** DI water spray (2 min)
4. **IPA Rinse:** 99% IPA (1 min, removes water spots)
5. **Drying:** Compressed air + bake at 60°C for 30 min

**UV Light Inspection:**

- **Equipment:** 365nm UV flashlight
- **Method:** Inspect all solder joints and PCB surface in dark room
- **Pass Criteria:** No fluorescence (indicates zero flux residue)

**Conformal Coating Application:**

- **Material:** Acrylic (MG Chemicals 422B) or Urethane (Humiseal 1A33)
- **Application Method:** Brush or aerosol spray (2-3 coats)
- **Thickness:** 25-50μm (verify with wet film thickness gauge)
- **Masking:** Mask all connectors (USB, encoder, I2C header), switches, and test points
- **Curing:** 24 hours at 20-25°C, or 2 hours at 60°C (accelerated cure)
- **Verification:** Measure resistance of coated surface (>1GΩ indicates good coverage)

---

## 📊 Comparative Matrix: Medium vs High Standards

| Parameter                    | Medium Standards (IPC Class 2)  | High Standards (IPC Class 3)                |
| ---------------------------- | ------------------------------- | ------------------------------------------- |
| **Soldering Temperature**    | ±10°C tolerance                 | ±5°C tolerance                              |
| **Flux Cleaning**            | Optional (no-clean flux OK)     | Mandatory (zero residue)                    |
| **Inspection Magnification** | 3x loupe                        | 10-20x microscope                           |
| **Acceptable Defect Rate**   | <5% visual defects              | Zero defects on critical components         |
| **Pull Test Force (Signal)** | 200g (2N)                       | 500g (5N)                                   |
| **Pull Test Force (Power)**  | 500g                            | 800g                                        |
| **ESD Handling**             | Wrist strap + mat               | Full ESD workstation + ionizer              |
| **Rework Cycles Allowed**    | 2 cycles per component          | 0 cycles on critical components             |
| **Environmental Protection** | None (clean indoor environment) | Conformal coating mandatory                 |
| **Insulation Resistance**    | >1MΩ                            | >100MΩ                                      |
| **Component Storage**        | Room temp, no special packaging | ESD bags, desiccant, <40% RH                |
| **Cleaning Method**          | IPA wipe (optional)             | Ultrasonic + IPA + bake                     |
| **Documentation**            | Basic checklist                 | Full traceability (batch, date, operator)   |
| **Testing Depth**            | Power-on + functional           | Electrical + environmental stress screening |

---

## 🔌 Component-Specific Guidelines

### Pull-Up Resistors (Encoder NPN Outputs)

**Component:** 4.7kΩ ±5% (1/4W or 1/8W) carbon film or metal film resistors

**Circuit Context:**

```
       3.3V
         │
        [R1: 4.7kΩ] ────┬─── PB6 (STM32 TIM4_CH1)
                        │
                   ┌────┴────┐
                   │ Encoder │  NPN Open Collector
                   │  CH_A   │  (Pulls LOW when active)
                   └────┬────┘
                        │
                       GND
```

**Power Dissipation Verification:**

- **Voltage Drop:** 3.3V (when encoder output is LOW)
- **Current:** I = V/R = 3.3V / 4700Ω = 0.7mA
- **Power:** P = I²R = (0.0007A)² × 4700Ω = 0.0023W (**<0.1W, safe for 1/8W resistor**)

**Placement Recommendations:**

- **Perfboard:** Place resistors within 50mm of STM32 pins (minimize trace length)
- **PCB:** Place within 10mm of MCU pins (reduces noise susceptibility)
- **Orientation:** Orient resistor parallel to encoder signal traces (reduces loop area)

**Value Tolerance:**

- **Medium Standards:** ±5% tolerance (4465-4935Ω acceptable)
- **High Standards:** ±1% tolerance (4653-4747Ω) for tighter noise margins

**Soldering Notes:**

- **Heat Sensitivity:** Carbon film resistors can drift if overheated (>400°C)
- **Recommended Temp:** 340-360°C, 2-second dwell maximum
- **Verification:** Measure resistance after soldering (should remain within tolerance)

---

### I2C Bus Pull-Ups (LCD + AS5600)

**Component:** 4.7kΩ ±5% (Medium) or 2.2-4.7kΩ ±1% (High, calculated for bus capacitance)

**Circuit Context:**

```
       3.3V
         │
    ┌───[R_SDA]───┬─── PB9 (STM32 I2C1_SDA)
    │             ├─── LCD SDA (0x27)
    │             └─── AS5600 SDA (0x36, optional)
    │
    └───[R_SCL]───┬─── PB8 (STM32 I2C1_SCL)
                  ├─── LCD SCL
                  └─── AS5600 SCL
```

**Bus Capacitance Calculation (High Standards):**

- **LCD I2C Backpack:** ~30pF
- **AS5600 (if used):** ~10pF
- **PCB Traces (50mm):** ~5pF
- **Total:** C_bus ≈ 45-50pF

**Pull-Up Resistor Value Selection:**

- **I2C Fast Mode (400kHz):** Rise time t_r < 300ns
- **Formula:** R_pullup < t_r / (0.8473 × C_bus)
- **Calculation:** R < 300ns / (0.8473 × 50pF) = **7kΩ maximum**
- **Recommended:** 4.7kΩ (safe margin, standard value)

**Placement:**

- **Perfboard:** Near LCD I2C connector (not at MCU end, single pull-up point)
- **PCB:** Near first I2C device (LCD backpack), equidistant from SDA/SCL traces

**Note:** Many I2C backpacks include onboard pull-ups (typically 10kΩ). Verify with multimeter before adding external resistors (parallel resistors reduce total resistance).

---

### Power Decoupling Capacitors (STM32F411CE)

**Component:** 0.1μF ceramic (X7R or X5R dielectric) + 10μF electrolytic/ceramic bulk capacitors

**Circuit Context:**

```
      5V USB
       │
      [LDO: 3.3V Regulator on Black Pill]
       │
       ├─── [C_bulk: 10μF] ──┬─── 3.3V Rail
       │                      │
       └────────────────────┬─┴─── VDD_1 (STM32 Pin 24)
                           [C1: 0.1μF]
                             │
                            GND
(Repeat for VDD_2, VDD_3 if exposed on Black Pill headers)
```

**ESR (Equivalent Series Resistance) Requirements:**

- **0.1μF Ceramic:** <0.1Ω (low ESR for high-frequency noise suppression >1MHz)
- **10μF Bulk:** <0.5Ω (handles low-frequency transients <100kHz)

**Placement (PCB High Standards):**

- **Distance:** Within 5mm of STM32 VDD pins (minimizes inductance loop)
- **Via Placement:** GND via directly adjacent to capacitor pad (shortest return path)
- **Orientation:** Align capacitor to minimize current loop area (perpendicular to power trace)

**Soldering Notes:**

- **Ceramic Capacitors:** Mechanically fragile (avoid flexing PCB during/after soldering)
- **Electrolytic Polarity:** Verify polarity (stripe = negative terminal)
- **Temperature Sensitivity:** X7R capacitors lose capacitance at high temp (use X5R for -10°C to 70°C range)

---

### Connectors (JST-XH, Dupont Headers)

**Crimp Quality Inspection:**

- **Retention Force Test:** Pull wire with 500g force (should not extract from crimp barrel)
- **Visual Inspection:** Wire insulation should be gripped by insulation barrel, conductor by wire barrel
- **Cross-Section Check (Sampling):** Cut crimped wire, verify conductor strands compressed (not crushed)

**Strain Relief Requirements:**

- **Encoder Cables:** Loop wire 360° before entering connector housing (absorbs tension)
- **Hot Glue:** Apply to cable exit point from housing (prevents pullout)
- **Cable Ties:** Secure cable bundle 50mm from connector (redirect pull force)

**Contact Resistance Verification:**

- **Measurement:** 4-wire Kelvin measurement (eliminates test lead resistance)
- **Pass Criteria:** <10mΩ per contact (Medium), <5mΩ (High)
- **Method:** Insert precision probes into connector, measure with μΩ meter

**Soldering to PCB (Header Pins):**

- **Alignment:** Use connector as jig (insert into female housing, solder while coupled)
- **Solder Volume:** Fill through-hole 75-100%, avoid excess (blocks connector insertion)
- **Pin Height:** Verify 2-3mm pin protrusion above solder joint (allows rework)

---

## 🌡️ Material Handling Compliance

### Environmental Operating Conditions (Per GEMINI.md Section 2.2)

**IronTrak Deployment Environment:**

- **Vibration:** 5-10Hz industrial vibration (bandsaw operation)
- **Temperature Range:** -10°C to 70°C (encoder specification)
- **Contamination:** Oil mist, metal dust (steel/aluminum), cutting fluid spray

**Assembly Implications:**

#### Vibration Resistance (5-10Hz)

- **Solder Joint Fatigue:** Use SAC305 or leaded solder (higher fatigue life than pure tin)
- **Mechanical Support:** Hot glue or potting compound on high-mass components (LCD module, connectors)
- **Fasteners:** Loctite threadlocker on all mounting screws (prevents loosening)
- **Wire Routing:** Avoid cantilevered wire runs (secure at 50mm intervals)

#### Temperature Cycling (-10°C to 70°C)

- **Solder Alloy:** SAC305 remains ductile across temperature range
- **Component Rating:** Verify all passives rated for extended temperature (automotive grade preferred)
- **Capacitor Derating:** Use X7R/X5R ceramics (maintain capacitance at temperature extremes)
- **Thermal Expansion:** PCB mounted components: allow stress relief (no rigid connections to enclosure)

#### Contamination Protection (Oil, Metal Dust)

- **Conformal Coating:** Acrylic or urethane coating (25-50μm thickness)
- **Connector Sealing:** Use gasketed connectors or silicone sealant at cable entry points
- **Enclosure:** IP54 minimum (dust ingress limited, splashproof)
- **Ventilation:** Sealed enclosure (no ventilation holes that admit dust)

---

### Component Storage

**Moisture Sensitivity Level (MSL) Awareness:**

- **IronTrak Components:** Primarily through-hole and passive SMT (MSL 1, not moisture sensitive)
- **STM32 Black Pill:** Pre-assembled module (no MSL concerns for end-user)
- **Storage:** Room temperature, <60% RH (prevents oxidation of exposed pads/leads)

**ESD-Safe Storage (High Standards):**

- **Containers:** Conductive bins or metallized ESD bags
- **Labeling:** Clearly mark bins with component type and ESD sensitivity class
- **Access Control:** Store in ESD-protected area (personnel must wear wrist straps)

**Shelf-Life Tracking:**

- **Flux Pens:** 6-12 months (flux dries out, becomes less effective)
- **Solder Paste:** 6 months refrigerated (reflow assembly, not hand soldering)
- **IPA (Isopropyl Alcohol):** Indefinite (keep sealed to prevent water absorption)

---

## ✅ Workflow Checklists

### Perfboard Medium Standard Checklist

**Pre-Assembly Setup:**

- [ ] Workbench clean, well-lit (>500 lumens white LED)
- [ ] Soldering iron preheated to 320-350°C (verified with thermocouple)
- [ ] Tools prepared: pliers, wire strippers, flush cutters, tweezers
- [ ] ESD wrist strap connected to earth ground (1MΩ resistor verified)
- [ ] Schematic and wiring diagram available for reference

**Component Identification:**

- [ ] All components identified and matched to BOM
- [ ] Resistor values verified with multimeter (color code confirmation)
- [ ] Polarity-sensitive components marked (electrolytic caps, diodes, ICs)
- [ ] Encoder wiring identified (A/B channels, power, GND)

**Soldering Process:**

- [ ] Component leads bent and trimmed to correct length
- [ ] Components inserted into Perfboard, leads secured with tape (prevents shifting)
- [ ] Solder iron tip cleaned before each joint
- [ ] Solder applied to joint (not tip), wire heated for 1-2s before solder touch
- [ ] Visual inspection: joint is shiny, concave, complete wetting
- [ ] Excess lead trimmed (1-2mm stub remains above solder joint)

**Wire Routing:**

- [ ] Wire colors match schematic convention (red=5V, black=GND, etc.)
- [ ] Encoder A/B channels use twisted pair (24 AWG)
- [ ] Wire bundles secured with cable ties at 50mm intervals
- [ ] Strain relief applied to connector entries (wire loop + hot glue)

**Visual Inspection:**

- [ ] All solder joints shiny and concave (no cold joints)
- [ ] No solder bridges between adjacent pads/traces
- [ ] Wire insulation intact (no melted insulation within 5mm of joint)
- [ ] Components oriented correctly (polarity verified)

**Electrical Testing:**

- [ ] Power OFF during initial testing
- [ ] Continuity test: multimeter beep confirms all connections
- [ ] Resistance check: pull-up resistors measure 4.5-5.0kΩ
- [ ] Insulation test: >1MΩ between 5V and GND rails
- [ ] Power ON: 5V rail measures 4.75-5.25V

**Pull Test (200g):**

- [ ] Gently tug component leads with tweezers
- [ ] No movement or solder joint cracking observed

**Final QA Sign-Off:**

- [ ] Assembly date: **\*\***\_\_\_\_**\*\***
- [ ] Assembled by: **\*\***\_\_\_\_**\*\***
- [ ] Tested by: **\*\***\_\_\_\_**\*\***
- [ ] Status: ☐ PASS ☐ FAIL (notes: **\*\***\_**\*\***)

---

### Perfboard High Standard Checklist

**Pre-Assembly Setup:**

- [ ] Full ESD workstation (mat, wrist strap, ionizer verified)
- [ ] Soldering station with PID controller (±5°C accuracy)
- [ ] Preheating platform at 150°C (verified with IR thermometer)
- [ ] 10x stereo microscope positioned for inspection
- [ ] IPA (≥99%) and lint-free wipes prepared for cleaning

**Component Identification & Verification:**

- [ ] All components verified against BOM (part number, value, tolerance)
- [ ] Resistor tolerance verified: ±1% (color code or multimeter)
- [ ] Components stored in ESD-safe containers (remove only when soldering)

**Enhanced Soldering Process:**

- [ ] Perfboard preheated on platform (150°C, 30 seconds)
- [ ] Soldering iron: 360-370°C, tip cleaned with brass wool
- [ ] Solder applied with controlled dwell (1-2 seconds per joint)
- [ ] Fillet height verified: >50% of pad diameter (visual check)
- [ ] Natural cooling (no forced air, prevents thermal stress)

**Advanced Wiring Standards:**

- [ ] Encoder A/B channels: 24 AWG twisted pair (3-5 twists per inch)
- [ ] Ferrite bead installed on encoder 5V power line
- [ ] Wire routing: no sharp bends (<3mm radius minimum)
- [ ] Strain relief applied (360° wire loop before connector entry)

**Flux Residue Cleaning (Mandatory):**

- [ ] IPA wipe (≥99%, lint-free cloth) applied to all solder joints
- [ ] Inspection under 10x magnification: zero visible flux residue
- [ ] Optional: Ultrasonic bath (5 min, IPA solution)
- [ ] UV light inspection (365nm): no fluorescence detected

**Conformal Coating Application:**

- [ ] Mask all connectors, switches, test points with Kapton tape
- [ ] Apply acrylic or urethane coating (2-3 coats, brush or spray)
- [ ] Verify wet film thickness: 25-50μm (film thickness gauge)
- [ ] Cure: 24 hours at room temp OR 2 hours at 60°C

**Strict Inspection (10x Magnification):**

- [ ] Every solder joint inspected under microscope
- [ ] Fillet geometry: smooth concave profile, 90° contact angle
- [ ] Zero voids visible in solder joint
- [ ] No flux residue (post-cleaning verification)

**Insulation Resistance Test:**

- [ ] Megohmmeter or precision multimeter (>10MΩ range)
- [ ] Test voltage: 50V DC between adjacent traces
- [ ] Result: >10MΩ (record value: **\_\_\_** MΩ)

**Pull Test (500g Minimum):**

- [ ] Calibrated force gauge or spring scale used
- [ ] Encoder A/B inputs: 500g pull, zero movement
- [ ] I2C SDA/SCL: 500g pull, zero movement
- [ ] Power/GND: 800g pull, zero movement

**Electrical Testing:**

- [ ] Continuity, resistance, insulation tests per Medium checklist
- [ ] All tests PASS before conformal coating application

**Final QA Sign-Off:**

- [ ] Assembly date: **\*\***\_\_\_\_**\*\***
- [ ] Operator ID: **\*\***\_\_\_\_**\*\***
- [ ] Inspector: **\*\***\_\_\_\_**\*\***
- [ ] Insulation resistance: **\_\_\_** MΩ
- [ ] Conformal coating: ☐ Applied ☐ Cured
- [ ] Status: ☐ PASS ☐ FAIL (notes: **\*\***\_**\*\***)
- [ ] ESD bag serial number: **\*\***\_\_\_\_**\*\***

---

### PCB Medium Standard Checklist

**Pre-Assembly Setup:**

- [ ] PCB inspected for damage (no scratches, lifted pads, contamination)
- [ ] Soldering iron at 340-360°C (lead-free solder)
- [ ] Flux pen or syringe prepared (no-clean rosin flux)
- [ ] Tweezers, magnifier, and multimeter ready

**SMT Passive Component Soldering (0805/0603):**

- [ ] Apply flux to component pads
- [ ] Tin one pad with small solder amount
- [ ] Place component with tweezers, reflow tinned pad
- [ ] Solder opposite end, then reflow first end (equalize solder volume)
- [ ] Visual inspection: component centered, both fillets present, no tombstoning

**Through-Hole Component Soldering:**

- [ ] Component inserted, leads bent to secure in place
- [ ] Solder applied from bottom side of PCB (fills via)
- [ ] Visual inspection: solder visible on top and bottom, >50% hole fill

**Rework Tracking:**

- [ ] Record components reworked (maximum 2 cycles allowed)
- [ ] Inspect pads after 2nd rework (check for damage, lifted traces)
- [ ] If 3rd rework needed: reject component or PCB section

**Visual Inspection:**

- [ ] SMT components centered on pads (±0.2mm acceptable)
- [ ] Solder fillets present on both ends of resistors/capacitors
- [ ] No tombstoning (components standing upright)
- [ ] Through-hole joints have solder on both sides

**Functional Testing:**

- [ ] Power-On: 5V rail measures 4.75-5.25V
- [ ] I2C Communication: LCD displays boot screen
- [ ] Encoder Pulse Count: Rotate encoder, verify count increment
- [ ] Button Response: Press menu button, observe response

**Final QA Sign-Off:**

- [ ] Assembly date: **\*\***\_\_\_\_**\*\***
- [ ] Assembled by: **\*\***\_\_\_\_**\*\***
- [ ] Functional test: ☐ PASS ☐ FAIL
- [ ] Status: ☐ PASS ☐ FAIL (notes: **\*\***\_**\*\***)

---

### PCB High Standard Checklist (Production)

**Pre-Assembly Setup:**

- [ ] Full ESD workstation (mat, wrist strap, ionizer, daily verification logged)
- [ ] PCB inspected under 10x magnification (no defects, contamination)
- [ ] Preheating platform at 100-120°C
- [ ] Soldering station: 340-350°C ±5°C (PID controller)
- [ ] Ultrasonic cleaner prepared (DI water + surfactant solution)

**Precision SMT Soldering:**

- [ ] PCB preheated (30 seconds on platform)
- [ ] Flux applied sparingly (water-soluble organic flux)
- [ ] Component placement verified under 10x magnification (centered, aligned)
- [ ] Soldering: controlled dwell (1-2s), inspect fillet immediately (90° contact angle)
- [ ] No rework on critical components (encoder pull-ups, MCU decoupling caps)

**Through-Hole Soldering (High Reliability):**

- [ ] Thermal relief spokes wetted with solder (verify continuity)
- [ ] Hole fill: >75% (inspect from both sides)
- [ ] Annular ring intact (no breakout visible under magnification)

**AOI-Equivalent Manual Inspection:**

- [ ] 10-20x stereo microscope with ring light
- [ ] 100% pad coverage verified on all SMT components
- [ ] No component misalignment (max 10% off-center allowed)
- [ ] No solder bridges (including under components)

**Insulation Resistance Test:**

- [ ] Test voltage: 100V DC
- [ ] Critical pairs tested:
  - [ ] 5V to GND: **\_\_\_** MΩ (>100MΩ required)
  - [ ] Encoder A to Encoder B: **\_\_\_** MΩ
  - [ ] I2C SDA to SCL: **\_\_\_** MΩ

**Post-Assembly Cleaning (Mandatory):**

- [ ] Pre-clean: IPA rinse (removes bulk flux)
- [ ] Ultrasonic bath: 5 min, 40kHz, 40°C (DI water + surfactant)
- [ ] DI water rinse: 2 min (spray or immersion)
- [ ] IPA rinse: 1 min (99% IPA, removes water spots)
- [ ] Drying: compressed air + bake at 60°C for 30 min
- [ ] UV light inspection: no fluorescence (confirms zero flux residue)

**Conformal Coating Application:**

- [ ] Mask connectors, switches, test points (Kapton tape)
- [ ] Apply acrylic/urethane coating (brush or spray, 2-3 coats)
- [ ] Wet film thickness measured: 25-50μm
- [ ] Cure: 24 hours at room temp OR 2 hours at 60°C
- [ ] Resistance test of coated surface: >1GΩ

**Electrical Testing:**

- [ ] Power-on test: 5V rail ±2%
- [ ] I2C communication verified (LCD boot screen)
- [ ] Encoder quadrature verification (oscilloscope or logic analyzer)
- [ ] All GPIO inputs respond correctly (menu button, encoder)

**Environmental Stress Screening (Optional, Production >50 Units):**

- [ ] Temperature cycling: -10°C to 70°C, 5 cycles
- [ ] Vibration test: 5-10Hz, 30 minutes (simulates bandsaw operation)
- [ ] Power cycling: 100 cycles (on/off, 10s interval)
- [ ] Functional test after ESS: all tests PASS

**ESD Packaging:**

- [ ] Assembly placed in metallized ESD bag
- [ ] Desiccant pack included
- [ ] Humidity indicator card installed (<40% RH target)
- [ ] ESD warning label affixed
- [ ] Serial number logged: **\*\***\_\_\_\_**\*\***

**Final QA Sign-Off:**

- [ ] Assembly date: **\*\***\_\_\_\_**\*\***
- [ ] Operator ID: **\*\***\_\_\_\_**\*\***
- [ ] Inspector: **\*\***\_\_\_\_**\*\***
- [ ] Insulation resistance (5V-GND): **\_\_\_** MΩ
- [ ] Conformal coating applied: ☐ Yes ☐ No
- [ ] ESS performed: ☐ Yes ☐ No (results: \***\*\_\*\***)
- [ ] Status: ☐ PASS ☐ FAIL (notes: **\*\***\_**\*\***)
- [ ] Batch number: **\*\***\_\_\_\_**\*\***
- [ ] Traceability: PCB lot **\_\_\_**, BOM rev **\_\_\_**, firmware version **\_\_\_**

---

## 📚 Additional Resources

### Industry Standards Referenced

- **IPC-A-610:** Acceptability of Electronic Assemblies (Class 2 and Class 3)
- **IPC-7711/7721:** Rework, Modification, and Repair of Electronic Assemblies
- **IPC-CC-830:** Qualification and Performance of Electrical Insulating Compounds
- **J-STD-001:** Requirements for Soldered Electrical and Electronic Assemblies

### Recommended Reading

- [NASA Workmanship Standards (NASA-STD-8739)](https://nepp.nasa.gov/index.cfm/5555) - Gold standard for high-reliability soldering
- [IPC Solder Joint Evaluation Poster](https://www.ipc.org) - Visual reference for acceptable/defective joints
- [Kester Solder Training Videos](https://www.kester.com) - Hand soldering techniques

### Supplier Recommendations (Materials)

- **Solder:** Kester, MG Chemicals, Nihon Superior (SAC305 or Sn63/Pb37)
- **Flux:** Kester 951 (no-clean), MG Chemicals 8341, Amtech RMA-223 (water-soluble)
- **Conformal Coating:** MG Chemicals 422B (acrylic), Humiseal 1A33 (urethane)
- **IPA:** 99% isopropyl alcohol (electronics grade, no additives)
- **ESD Supplies:** Desco, 3M, Protektive Pak

---

**Document Version:** 1.0  
**Created:** 2025-11-28  
**Compliance:** GEMINI.md Section 2.2 (Hardware), Section 3.2 (Reliability Constraints)  
**Authority:** IronTrak Project - Assembly Quality Assurance

---

> [!TIP] > **Quick Reference Guide:** For fast lookup during assembly, print the **Workflow Checklists** section (pages 17-21) and laminate for use at the workbench. Keep this full document for training and quality audits.
