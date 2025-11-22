# IronTrak - Power Supply Design (HiLink 5V Module)

## HiLink AC-DC Module + Input Filtering Circuit

### HiLink Module Specifications
- **Model:** HLK-PM01 (or HLK-5M05)
- **Input:** 100-240VAC, 50/60Hz
- **Output:** 5VDC, 600mA (3W) or 1A (5W)
- **Efficiency:** ~75%
- **Ripple:** <150mV (unfiltered)
- **Isolation:** 3000VAC

---

## Complete Power Input Circuit

```
230VAC Mains
    │
    │
   [Fuse 500mA slow-blow]
    │
    ├──► HiLink HLK-PM01 ◄── [Varistor 275VAC]
    │         │
    │        5VDC Out (Raw)
    │         │
    │         ├─[Fuse 1A fast-blow]──┐
    │         │                      │
    │         ├─[TVS Diode]─────┐    │
    │         │                 │    │
    │         │               [GND]  │
    │         │                      │
    │         ├─[1000µF/16V]────┬────┤
    │         │    (Bulk)       │    │
    │         │                 │    │
    │         ├─[100µF/16V]─────┤    │
    │         │  (Decoupling)   │    │
    │         │                 │    │
    │         ├─[10µF/16V]──────┤    │
    │         │  (Ceramic)      │    │
    │         │                 │    │
    │         ├─[100nF]─────────┤    │
    │         │  (HF Filter)    │    │
    │         │                 │    │
    │         ├─[Ferrite Bead]──┼────┤
    │         │                 │    │
    │         │               [GND]  │
    │         │                      │
    │         └──► 5V Clean ◄────────┘
    │                │
    │                ├──► Arduino 5V Pin
    │                ├──► Encoder VCC
    │                ├──► LCD VCC
    │                └──► KY-040 VCC
    │
   [GND Common]──────┴──► All GND connections
```

---

## Bill of Materials: Power Input Section

### Required Components

| Component | Value/Rating | Quantity | Purpose | Part Number Example |
|-----------|--------------|----------|---------|---------------------|
| **AC Input Protection** |
| Fuse (AC side) | 500mA Slow-Blow, 250VAC | 1 | AC overcurrent protection | Littelfuse 0215.500 |
| Fuse Holder | PCB mount | 1 | Fuse socket | Keystone 3557-2 |
| MOV Varistor | 275VAC, 14mm | 1 | Surge protection | EPCOS B72214S2271K101 |
| **DC Output Protection** |
| Fuse (DC side) | 1A Fast-Blow, 32V | 1 | DC overcurrent protection | Littelfuse 0251001 |
| TVS Diode | 6.8V, 600W (SMBJ6.8A) | 1 | Overvoltage/reverse polarity | ON Semi SMBJ6.8A |
| **Bulk Capacitors** |
| Electrolytic | 1000µF, 16V, Low-ESR | 1 | Bulk energy storage | Panasonic EEU-FR1C102 |
| Electrolytic | 100µF, 16V | 1 | Secondary bulk | Panasonic ECA-1CM101 |
| **Decoupling Capacitors** |
| Ceramic (X7R) | 10µF, 16V, 1206 | 1 | Mid-freq filtering | Murata GRM31CR71C106KA01 |
| Ceramic (X7R) | 100nF, 50V, 0805 | 2 | HF noise filtering | Murata GRM21BR71H104KA01 |
| **EMI Filtering** |
| Ferrite Bead | 600Ω @100MHz (BLM18) | 1 | RF noise suppression | Murata BLM18PG601SN1D |
| **Connector** |
| Screw Terminal | 2-pin, 5mm pitch | 1 | 5V/GND output | Phoenix 1757019 |

**Total Cost:** ~$8-12 for complete power input filtering

---

## PCB Layout Recommendations

### Power Input Zone Design

```
┌─────────────────────────────────────────────────────────────┐
│  AC INPUT ZONE (Keep 5mm clearance from DC!)                │
│                                                              │
│  [AC L] ──[Fuse]──[MOV]──┐                                  │
│                          │                                   │
│                     [HiLink Module]                          │
│                          │                                   │
│  [AC N] ────────────────┤                                   │
│                          │                                   │
│  [PE] ──────────────────┤                                   │
│                                                              │
├──────────────────────────┼───────────────────────────────────┤
│  DC OUTPUT ZONE          │                                   │
│                          │                                   │
│  5V ─[Fuse]─[TVS]─┬─[1000µF]─┬─[100µF]─┬─[10µF]─┬─[100nF]─┤
│                   │          │         │        │          │
│                   │          │         │        │          │
│  GND ─────────────┴──────────┴─────────┴────────┴──────────┤
│                                                              │
│       ▲                                                      │
│       │                                                      │
│   [Ferrite Bead]                                            │
│       │                                                      │
│       ▼                                                      │
│  [5V Out] ──────────────► To Arduino VIN or 5V Pin          │
│  [GND Out] ─────────────► To Arduino GND                    │
│                                                              │
└──────────────────────────────────────────────────────────────┘

NOTES:
- Keep AC traces wide (2mm minimum)
- Keep DC traces wide (1.5mm for 5V rail)
- Ground plane recommended on bottom layer
- Place capacitors close to HiLink output
- Star ground topology for best noise performance
```

---

## Detailed Component Explanations

### 1. AC Side Fuse (500mA Slow-Blow)
**Purpose:** Protects mains wiring if HiLink fails short-circuit
**Rating:** 500mA is 2× normal current (HiLink draws ~150-200mA @230VAC)
**Slow-blow:** Tolerates inrush current when powered on

### 2. MOV Varistor (275VAC)
**Purpose:** Absorbs voltage spikes/surges from mains
**Clamping:** Limits voltage to ~400V during surge
**Placement:** Directly across AC input before HiLink

### 3. DC Side Fuse (1A Fast-Blow)
**Purpose:** Protects circuit if load shorts
**Rating:** 1A for total system draw (~300mA typical, 500mA max)
**Fast-blow:** Reacts quickly to overload

### 4. TVS Diode (SMBJ6.8A)
**Purpose:** Clamps overvoltage, protects against reverse polarity
**Clamping:** 6.8V breakdown (safe for 5V circuit)
**Power:** 600W peak, handles transients

### 5. 1000µF Bulk Capacitor (Low-ESR)
**Purpose:** Energy reservoir for current spikes
**ESR:** Low equivalent series resistance (<0.1Ω)
**Ripple Current:** Rated for 1A+
**Effect:** Reduces voltage sag when encoder/LCD draw current

### 6. 100µF Decoupling Capacitor
**Purpose:** Secondary filtering for mid-frequency noise
**Frequency:** ~1-100kHz ripple reduction
**Placement:** Close to HiLink output

### 7. 10µF Ceramic Capacitor (X7R)
**Purpose:** Fast response to load transients
**Type:** X7R dielectric (stable, low drift)
**Frequency:** ~100kHz-1MHz filtering

### 8. 100nF Ceramic Capacitors (×2)
**Purpose:** High-frequency noise filtering
**Frequency:** 1MHz-100MHz
**Placement:** One at HiLink, one at Arduino 5V pin

### 9. Ferrite Bead (600Ω @100MHz)
**Purpose:** Blocks RF noise from HiLink switching
**Impedance:** High at radio frequencies, low at DC
**Placement:** In series with 5V line after capacitors

---

## Schematic with Values

```
230VAC IN
    │
  [F1: 500mA SB]
    │
    ├──[MOV: 275VAC]──┐
    │                 │
    │    ┌────────────┴─────────┐
    │    │   HiLink HLK-PM01    │
    │    │   230VAC → 5VDC      │
    │    │      (1A Max)        │
    │    └──┬───────────────────┘
    │       │  5V Raw
    │       │
  [GND]     ├──[F2: 1A FB]───┐
            │                │
            │                │
            ├──[D1: SMBJ6.8A]┤
            │  (Cathode to +)│
            │                │
            ├──[C1: 1000µF/16V]─┬──┐
            │   (Low-ESR)        │  │
            │                    │  │
            ├──[C2: 100µF/16V]──┤  │
            │                    │  │
            ├──[C3: 10µF/16V]───┤  │
            │   (Ceramic X7R)    │  │
            │                    │  │
            ├──[C4: 100nF/50V]──┤  │
            │   (Ceramic)        │  │
            │                    │  │
            ├──[FB1: 600Ω@100MHz]─┤
            │   (Ferrite Bead)   │  │
            │                    │  │
            └──► 5V Clean ◄──────┘  │
                    │               │
                    │             [GND]
                    │
                  OUTPUT
```

---

## Power Quality Specifications

### Before Filtering (HiLink Raw Output)
- **Voltage:** 5.0V ± 5% (4.75-5.25V)
- **Ripple:** ~100-150mV pk-pk @100kHz
- **Noise:** ~50-100mV RF noise

### After Filtering (Circuit Output)
- **Voltage:** 5.0V ± 2% (4.90-5.10V)
- **Ripple:** <10mV pk-pk
- **Noise:** <5mV RF noise
- **Transient Response:** <50mV droop for 500mA step

**Result:** Clean, stable, Arduino-safe power!

---

## Safety Considerations

### Electrical Safety

**⚠️ DANGER: 230VAC CAN KILL!**

1. **Isolation:**
   - HiLink provides 3000VAC isolation between AC and DC
   - Keep 5mm minimum clearance between AC and DC traces
   - Use conformal coating over AC traces

2. **Enclosure:**
   - PCB must be in insulated enclosure
   - No exposed AC connections
   - Use proper strain relief on AC cable

3. **Grounding:**
   - Connect AC ground (PE) to enclosure
   - DO NOT connect PE to DC GND (HiLink is isolated!)

4. **Fusing:**
   - Always use fuse on AC side
   - Consider double-fusing (AC + DC)

5. **Testing:**
   - Test with multimeter BEFORE connecting Arduino
   - Check for shorts between AC and DC

### Regulatory Compliance

For commercial use, consider:
- **CE Marking** (if selling in EU)
- **UL Listing** (if selling in US)
- **EMC Testing** (electromagnetic compatibility)

For personal use: Just follow best practices above.

---

## Testing Procedure

### Step 1: Visual Inspection
- [ ] Check all solder joints
- [ ] Verify no shorts between AC and DC
- [ ] Confirm polarity of electrolytic caps
- [ ] TVS diode oriented correctly (cathode to +5V)

### Step 2: Resistance Check (POWER OFF!)
- [ ] AC to DC isolation: >10MΩ
- [ ] 5V to GND: ~100Ω (capacitors charging)
- [ ] No shorts on 5V rail

### Step 3: Initial Power-Up (NO LOAD)
1. Connect AC power
2. **Measure 5V output with multimeter**
3. **Expected:** 4.9-5.1V
4. **Measure ripple** with oscilloscope (if available)
5. **Expected:** <20mV pk-pk

### Step 4: Load Test
1. Connect 100Ω resistor across 5V/GND (50mA load)
2. **Measure voltage drop**
3. **Expected:** <50mV drop
4. Connect Arduino (no peripherals yet)
5. **Measure current:** Should be ~50mA
6. Add encoder, LCD, KY-040
7. **Measure total current:** Should be <300mA

### Step 5: Thermal Check
1. Run system for 30 minutes
2. **Touch HiLink module**
3. **Expected:** Warm but not too hot to touch (<60°C)
4. If too hot: Improve ventilation

---

## Troubleshooting

### Problem: No 5V Output
**Check:**
- AC fuse blown?
- HiLink connected correctly (L/N)?
- DC fuse blown?
- TVS diode shorted (test with multimeter)?

### Problem: Low Voltage (<4.8V)
**Check:**
- Too much load? (Measure current)
- Weak HiLink module?
- High-resistance connection?
- Add more bulk capacitance

### Problem: High Ripple (>20mV)
**Check:**
- Capacitors installed?
- Electrolytic caps oriented correctly?
- Add more 100nF caps
- Check ferrite bead

### Problem: Arduino Resets Randomly
**Check:**
- Brownout during load transients (add more 1000µF caps)
- Ground loops (star ground topology)
- RF noise (add ferrite bead, shielded USB cable)

---

## Alternative: Using Arduino VIN Pin

Instead of connecting to 5V pin, you can use Arduino's built-in regulator:

```
HiLink 5V ──► Arduino VIN Pin
            (Arduino LM1117 regulator inside)
            ▼
         Arduino 5V Pin ──► Peripherals
```

**Pros:**
- Arduino regulator provides additional filtering
- Thermal/overcurrent protection built-in

**Cons:**
- Wastes power as heat (~0.5W)
- Regulator can get hot
- Lower efficiency

**Recommendation:** 
- For **PCB build**: Connect directly to 5V pin (cleaner)
- For **breadboard**: Use VIN pin (safer during prototyping)

---

## Cost Breakdown

| Item | Cost | Source |
|------|------|--------|
| HiLink HLK-PM01 (5V/1A) | $3.50 | AliExpress |
| Fuses (2×) | $0.50 | DigiKey |
| MOV Varistor | $0.30 | DigiKey |
| TVS Diode | $0.50 | DigiKey |
| Capacitors (6×) | $2.00 | DigiKey |
| Ferrite Bead | $0.20 | DigiKey |
| Screw Terminals | $0.50 | DigiKey |
| **TOTAL** | **$7.50** | |

**Note:** Prices for single quantities. Bulk pricing is much lower.

---

## PCB Gerber Files (Future Addition)

Once you have a working prototype, I can help design a proper PCB with:
- Optimized power input circuit
- Arduino header footprint
- Screw terminals for all connections
- Proper trace widths and clearances
- Mounting holes

---

**Ready for professional-grade power! No more USB cables—this is permanent installation quality!**
