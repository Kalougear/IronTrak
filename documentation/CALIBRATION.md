# IronTrak Calibration Guide

**Setup, calibration procedures, and accuracy optimization**

---

## 📋 Table of Contents

- [Overview](#overview)
- [Initial Setup](#initial-setup)
- [Auto Calibration Wizard](#auto-calibration-wizard)
- [Manual Wheel Diameter Calibration](#manual-wheel-diameter-calibration)
- [Direction Reversal](#direction-reversal)
- [Angle Sensor Calibration](#angle-sensor-calibration)
- [Accuracy Verification](#accuracy-verification)
- [Troubleshooting](#troubleshooting)

---

V

## 🎯 Overview

### What Needs Calibration?

The IronTrak encoder measures **fence position** by counting wheel rotations. Accuracy depends on knowing the **exact wheel diameter**. Small errors compound over distance.

### Calibration Accuracy

| Wheel Diameter Error | Position Error @ 1 meter |
| -------------------- | ------------------------ |
| ±0.1mm               | ±2mm                     |
| ±0.5mm               | ±10mm                    |
| ±1.0mm               | ±20mm                    |

**Goal:** Calibrate within ±0.1mm for sub-mm accuracy across typical cutting ranges.

---

## 🔧 Initial Setup

### 1. Physical Installation

**Before calibrating:**

1. Mount encoder securely to bandsaw fence
2. Ensure wheel makes **solid contact** with fence rail (no slipping)
3. Check for **smooth rotation** (no binding or wobble)
4. Verify encoder wheel diameter is approximately 50mm (default assumption)

### 2. Power On Test

```
1. Connect USB power
2. Observe 3 LED blinks (boot sequence)
3. LCD should display:
   ┌────────────────────┐
   │   0.0 CM           │
   │                    │
   │ 0° -- F:--         │
   │                    │
   └────────────────────┘
```

If display is blank, see [Troubleshooting](#troubleshooting).

### 3. Initial Movement Test

```
1. Manually move fence ~100mm
2. Observe display updating in real-time
3. If direction is backwards, see [Direction Reversal](#direction-reversal)
4. If value is wildly wrong (~2x or 0.5x), wheel diameter needs calibration
```

---

## 🧙 Auto Calibration Wizard

**Recommended Method** - Guides you through precise calibration.

### Step-by-Step Procedure

#### 1. Enter Calibration Menu

```
1. Long-press menu button (500ms)
2. Rotate encoder to "Calibration"
3. Click to enter
4. Select "Auto Calibration Wizard"
5. Click to start
```

#### 2. Prepare Reference Measurement

**You Need:**

- Tape measure or precision ruler (metric preferred)
- Flat reference surface or fence scale
- Ability to move fence exactly 100mm

**Display Shows:**

```
┌────────────────────┐
│  Calibration Mode  │
│  Move fence 100mm  │
│  Click when ready  │
│                    │
└────────────────────┘
```

#### 3. Perform Measurement

```
1. Note starting fence position (e.g., 50mm on fence scale)
2. Move fence EXACTLY 100mm (ending at 150mm)
3. Click button
```

**Critical:** The more accurate your 100mm reference, the better the calibration.

#### 4. System Calculates

**Display Shows:**

```
┌────────────────────┐
│  Calculating...    │
│  Pulses: XXXX      │
│  New Dia: XX.X mm  │
│                    │
└────────────────────┘
```

**Formula:**

```
wheelDiameter = (100mm × oldDiameter × π) / (pulses × π)
              = (100mm × oldDiameter) / pulses
```

#### 5. Confirm & Save

```
1. Review calculated diameter (should be 48-52mm typical)
2. Rotate encoder to adjust if needed (fine-tune ±0.1mm)
3. Click to save
```

**Display Returns to Idle:** Calibration complete!

---

## 🔧 Manual Wheel Diameter Calibration

**Use When:**

- Auto wizard fails
- You have a measured wheel diameter from calipers
- Fine-tuning after initial calibration

### Procedure

```
1. Long-press menu button
2. Navigate to: Calibration → Manual Wheel Diameter
3. Click to enter edit mode (value highlighted)
4. Rotate encoder to adjust (±0.1mm per click)
5. Click to save
```

### Measuring Wheel Diameter with Calipers

```
1. Remove encoder wheel from shaft
2. Measure diameter with calipers across center
3. Take 3 measurements at different points
4. Average the readings
5. Enter value in manual calibration menu
```

**Example:**

```
Measurement 1: 50.2mm
Measurement 2: 50.3mm
Measurement 3: 50.1mm
Average: 50.2mm ← Enter this value
```

---

## ↔️ Direction Reversal

**Symptom:** Fence moves right, display counts down (or vice versa).

### Fix

```
1. Long-press menu button
2. Navigate to: Calibration → Reverse Direction
3. Click to toggle (ON/OFF)
4. Long-press to exit menu
5. Verify direction is correct
```

**Alternate Method (Hardware):**
Swap encoder A/B wires (PB6 ↔ PB7).

---

## 📐 Angle Sensor Calibration

**Only applicable if AS5600 magnetic sensor is installed.**

### Physical Alignment

```
1. Set bandsaw to exact 0° (use precision square)
2. Observe displayed angle (should read 0-5°)
3. If grossly wrong (90° shown at 0°), check magnet orientation
4. Magnet must be diametrically magnetized, not axially
```

### Software Calibration (Future Feature)

**Currently:** AS5600 outputs absolute angle, no offset calibration available.

**Planned:**

- Zero angle offset setting
- 45° reference point calibration
- Linear interpolation for improved accuracy

---

## ✅ Accuracy Verification

### Test Procedure

```
1. Zero encoder (single click)
2. Move fence to 100mm (using precision scale)
3. Note IronTrak reading
4. Move to 200mm
5. Note reading
6. Move to 500mm
7. Note reading
```

### Expected Results

| Actual Position | IronTrak Reading | Error | Status       |
| --------------- | ---------------- | ----- | ------------ |
| 100mm           | 100.0 ± 0.5mm    | <0.5% | ✅ Excellent |
| 200mm           | 200.0 ± 1.0mm    | <0.5% | ✅ Excellent |
| 500mm           | 500.0 ± 2.5mm    | <0.5% | ✅ Excellent |

**If errors exceed 1%:** Re-run auto calibration wizard.

### Long-Range Test

```
1. Zero encoder
2. Move fence 1000mm (1 meter)
3. Check IronTrak reading
```

**Acceptable:** ±5mm error at 1m (0.5% accuracy)  
**Excellent:** ±2mm error at 1m (0.2% accuracy)

---

## 🔍 Troubleshooting

### Calibration Wizard Returns Same Diameter

**Cause:** Encoder not rotating during reference measurement.

**Fix:**

1. Check wheel contact with fence (slipping?)
2. Verify encoder wiring (A/B channels connected)
3. Monitor Serial1 debug output for pulse count

---

### Reading Drifts Over Time

**Symptom:** Zero at start, reads +5mm after 10 cuts.

**Causes:**

1. **Wheel slip:** Encoder wheel slipping on fence rail
   - Fix: Add rubber coating or increase contact pressure
2. **Thermal expansion:** Fence rail heating up
   - Normal in shop environment, re-zero periodically
3. **Encoder failure:** Missed pulses due to vibration
   - Check wiring, inspect encoder for damage

---

### Jumpy Readings (Skipping Values)

**Symptom:** Display jumps 147mm → 149mm → 152mm

**Causes:**

1. **Poor encoder contact**
   - Wheel binding or intermittent contact
2. **Electrical noise**
   - Check pull-up resistor values (4.7kΩ-10kΩ)
   - Verify encoder power supply stability
3. **I2C interference**
   - Separate encoder wiring from I2C lines

---

### Auto-Calibration Gives Unrealistic Diameter

**Example:** Wizard calculates 35mm or 70mm (expected ~50mm)

**Causes:**

1. **Incorrect reference measurement**
   - Moved 50mm instead of 100mm (result: 2x error)
   - Moved 200mm instead of 100mm (result: 0.5x error)
2. **Wheel slipped during calibration**
   - Repeat with firmer contact
3. **Encoder pulse loss**
   - Check wiring during movement

**Fix:** Repeat calibration with verified 100mm reference.

---

## 📊 Fine-Tuning for Maximum Accuracy

### Step 1: Coarse Calibration

Use auto wizard to get within ±2% accuracy.

### Step 2: Verification Cut

```
1. Zero encoder
2. Cut test piece at exactly 500mm (measure with calipers)
3. Note IronTrak reading when cut is made
```

### Step 3: Calculate Correction Factor

```
Error = (IronTrak_reading - Actual_length) / Actual_length

New_diameter = Old_diameter × (1 + Error)
```

**Example:**

```
Actual: 500.0mm (calipers)
IronTrak: 502.5mm (display)
Old diameter: 50.0mm

Error = (502.5 - 500.0) / 500.0 = 0.005 (0.5% high)
New diameter = 50.0 × (1 - 0.005) = 50.0 × 0.995 = 49.75mm
```

Enter `49.75mm` in manual calibration.

### Step 4: Re-Verify

Repeat test cut. Error should now be <0.1%.

---

## 🎓 Best Practices

### Always Calibrate When:

1. **First installation**
2. **After moving the encoder mounting**
3. **If wheel is replaced**
4. **If accuracy degrades** (>1% error)

### Periodic Verification:

- **Weekly:** Zero-check against fence scale
- **Monthly:** 100mm verification test
- **Quarterly:** Full accuracy verification to 1m

### Environmental Considerations:

- **Temperature:** Metal expands ~0.01mm/m per 10°C
  - Re-zero when shop temperature changes significantly
- **Vibration:** Tight mounting bolts, check periodically
- **Dust:** Keep encoder optics clean (compressed air monthly)

---

## 📞 Support

If calibration issues persist:

1. Check Serial1 debug output (115200 baud, PA9/PA10)
2. Document symptoms with photos/video
3. Report via GitHub Issues

---

**Last Updated:** 2025-11-25  
**Firmware Version:** v1.0.0
