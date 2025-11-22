# IronTrak - Simplified Power Circuit (Minimum Components)

## Option 1: Bare Minimum (Works But Not Ideal)

### Circuit Without TVS Diode or Ferrite Bead

```
230VAC IN
    │
  [Fuse 500mA]
    │
    ├──[MOV Varistor]──┐
    │                  │
    │    HiLink        │
    │    Module        │
    │                  │
    └──────┬───────────┘
           │
           ├──[Fuse 1A]──────┐
           │                 │
           ├──[1000µF/16V]───┤
           │   (CRITICAL!)   │
           │                 │
           ├──[100µF/16V]────┤
           │   (Important)   │
           │                 │
           ├──[100nF]────────┤
           │   (Important)   │
           │                 │
           └──► 5V Out ◄─────┘
                  │
                [GND]
```

**What You MUST Have:**
- ✅ AC fuse (500mA)
- ✅ MOV varistor (275VAC) 
- ✅ DC fuse (1A)
- ✅ 1000µF electrolytic capacitor
- ✅ 100µF electrolytic capacitor
- ✅ 100nF ceramic capacitor

**What You Can Skip:**
- ⚠️ TVS diode (optional but recommended)
- ⚠️ Ferrite bead (optional)
- ⚠️ 10µF ceramic cap (nice to have)

**Cost:** ~$5 instead of $7.50

---

## What Happens Without Each Component?

### Without TVS Diode
**Risk:** Overvoltage or reverse polarity can damage Arduino
**Mitigation:**
- Be VERY careful when connecting power
- Double-check polarity before plugging in
- HiLink has built-in protection (not as good as TVS though)

**Workaround:** Add a simple 5.1V Zener diode (common, cheap)
```
5V ──┬──► Output
     │
   [5.1V Zener]
     │
    GND
```
- Cost: $0.10
- Part: 1N4733A (5.1V, 1W)
- Protection: Clamps voltage to ~5.7V (less precise than TVS but works)

---

### Without Ferrite Bead
**Risk:** More RF noise on 5V rail
**Impact:**
- Encoder might count erratically
- LCD might flicker
- Arduino might reset rarely

**Mitigation:**
- Add more 100nF ceramic caps (3-4 instead of 1)
- Place them strategically:
  - One at HiLink output
  - One at Arduino 5V pin
  - One at encoder VCC
  - One at LCD VCC

**Workaround:** Use a small inductor instead
- 10µH inductor (through-hole)
- Cost: $0.20
- Works almost as well as ferrite bead

---

## Absolute Bare Minimum (Emergency Build)

If you're **really** in a pinch and just need it working NOW:

```
HiLink 5V ──┬──[1000µF]──┬──► Arduino 5V
            │            │
            └──[100nF]───┴──► GND
```

**Only 2 capacitors:**
- 1× 1000µF electrolytic (bulk)
- 1× 100nF ceramic (HF filter)

**Will it work?** YES, but:
- More noise
- Less protection
- Arduino might reset if encoder draws sudden current
- Not recommended for permanent installation

**Use this ONLY for:**
- Initial testing
- Proof of concept
- "I need it working today!"

**Then upgrade to proper circuit later.**

---

## Capacitor Selection Guide

### What You REALLY Need

**1000µF Electrolytic - CRITICAL**
- Stores energy for current spikes
- **Don't skip this one!**
- Minimum voltage: 10V (16V better)
- Low-ESR type preferred but not required
- Cost: $0.50-1.00

**100µF Electrolytic - Important**
- Secondary filtering
- Reduces ripple significantly
- Cost: $0.30

**100nF Ceramic - Important**
- High-frequency noise filter
- Prevents Arduino resets
- Get several (they're cheap!)
- Cost: $0.10 each

**10µF Ceramic - Nice to Have**
- Mid-frequency filtering
- Can skip if budget-constrained
- Cost: $0.30

---

## Simple Shopping List (Budget Version)

### Minimum Required ($5 total)

| Component | Value | Qty | Cost | Can Buy At |
|-----------|-------|-----|------|------------|
| AC Fuse | 500mA slow-blow | 1 | $0.50 | Hardware store |
| MOV Varistor | 275VAC | 1 | $0.30 | Electronics shop |
| DC Fuse | 1A fast-blow | 1 | $0.30 | Hardware store |
| Capacitor | 1000µF 16V | 1 | $0.80 | Electronics shop |
| Capacitor | 100µF 16V | 1 | $0.30 | Electronics shop |
| Capacitor | 100nF 50V | 3 | $0.30 | Electronics shop |
| Screw terminals | 2-pos | 2 | $0.50 | Electronics shop |
| **TOTAL** | | | **$3.00** | |

### Recommended Additions (+$2)

| Component | Alternative | Cost |
|-----------|-------------|------|
| TVS Diode | **OR** 5.1V Zener (1N4733A) | $0.50 / $0.10 |
| Ferrite Bead | **OR** 10µH Inductor | $0.20 / $0.20 |
| 10µF Ceramic | | $0.30 |

---

## Alternative Protection: Zener Diode

If you can't find a TVS diode, use this simple circuit:

```
5V ──┬──[D1: 1N5819 Schottky]──► To Arduino
     │   (Reverse polarity protection)
     │
     ├──[D2: 1N4733A Zener]
     │   (5.1V, 1W)
     │   Cathode to +5V
     │
    GND
```

**Parts:**
- **D1:** 1N5819 Schottky diode (prevents reverse polarity)
  - Cost: $0.10
  - Note: Drops 0.3V, so you get 4.7V output (Arduino is OK with this)
  
- **D2:** 1N4733A Zener diode (overvoltage protection)
  - Cost: $0.10
  - Clamps voltage to 5.7V max

**Total cost:** $0.20 (much cheaper than TVS!)

---

## Alternative EMI Filter: Simple Inductor

Instead of ferrite bead, use this:

```
HiLink 5V ──┬──[100nF]──┬──[10µH Inductor]──┬──► Clean 5V
            │           │                   │
          [GND]       [GND]            [100nF]
                                           │
                                         [GND]
```

**Parts:**
- 10µH inductor (radial, through-hole)
- Cost: $0.20
- Works well for switching noise

---

## Step-by-Step: What to Do

### Option A: Full Protection (Recommended)
1. Get all components from power supply doc
2. Build complete circuit
3. Best reliability and safety
4. **Total: $7.50**

### Option B: Budget Build (Good Enough)
1. Skip TVS → Use 5.1V Zener ($0.10)
2. Skip Ferrite → Use 10µH inductor ($0.20)
3. **Total: ~$5**

### Option C: Bare Minimum (Testing Only)
1. Just use: 1000µF + 100µF + 100nF caps
2. **Total: ~$2**
3. ⚠️ Upgrade to Option B for permanent install!

---

## What I Recommend

**For your machine shop bandsaw:**

**Phase 1 (Testing - Today):**
```
Minimum circuit:
- AC fuse
- MOV varistor  
- 1000µF cap
- 100nF cap (×2)
Cost: $2-3
```
**Get it working, test the software.**

**Phase 2 (Production - Next Week):**
```
Add:
- 100µF cap
- 5.1V Zener diode
- 10µH inductor
- DC fuse
Additional cost: $2
```
**Now it's reliable and protected.**

**This way you can test NOW without waiting for all parts!**

---

## Quick Reference: Capacitor Placement

```
HiLink Output
     │
     ├──[1000µF]──┐  ◄── Place close to HiLink
     │            │
     ├──[100µF]───┤  ◄── Within 1cm of 1000µF
     │            │
     ├──[100nF]───┤  ◄── Right at HiLink pins
     │            │
     └──► 5V ◄────┘
          │
          ├──[100nF]──► At Arduino 5V pin
          │
          ├──[100nF]──► At Encoder VCC
          │
          └──[100nF]──► At LCD VCC
```

**Key:** Spread the 100nF caps around, not all in one place!

---

## Testing Without TVS/Ferrite

**Extra checks you should do:**

1. **Measure voltage carefully:**
   - Should be 4.9-5.2V under no load
   - Should NOT go above 5.5V even with no load

2. **Load test:**
   - Connect a 10Ω resistor across output (500mA load)
   - Voltage should stay above 4.7V
   - If it drops too much, add more bulk capacitance

3. **Ripple check:**
   - If you have oscilloscope: Should be <50mV pk-pk
   - If Arduino resets randomly: Add more 100nF caps

4. **Thermal check:**
   - HiLink should be warm but not burning hot
   - If too hot: Reduce load or improve airflow

---

**Bottom Line:** Start with the minimum, test it, add protection later if you have issues. The encoder pullup resistors (4.7kΩ) are MORE critical than the power filtering!**
