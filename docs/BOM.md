# IronTrak - Bill of Materials (BOM)

## ✅ Complete Parts List for Hardware Build

---

## 1. Electronics Components

### Microcontroller
- **STM32F411 "Black Pill"**
  - Quantity: 1
  - Specs: 100MHz, 512KB Flash, 128KB RAM
  - **CRITICAL:** Get the "WeAct Studio" version if possible (better quality)
  - Connector: USB-C
  - ~$5-6 on AliExpress/Amazon
  - **Why?** Hardware Encoder Interface & 5V Tolerant Pins

### Programmer (Optional but Recommended)
- **ST-Link V2** (USB Dongle)
  - Quantity: 1
  - Purpose: Easy programming and debugging
  - ~$3-5
  - *Alternative:* Use USB-C cable and DFU mode (hold BOOT0 while plugging in)

### Main Encoder (Critical!)
- **Industrial Optical Rotary Encoder**
  - Quantity: 1
  - Specs: **600 PPR** (Pulses Per Revolution)
  - Output: NPN Open Collector
  - Voltage: 5-24V (will use 5V from Arduino)
  - Signals: A, B, Z (Z optional, not used)
  - Mounting: 6mm or 8mm shaft
  - Search terms: "600PPR Optical Rotary Encoder NPN"
  - ~$15-25 on AliExpress
  - **Example:** E6B2-CWZ6C 600P/R

### Menu Input Encoder
- **KY-040 Rotary Encoder Module**
  - Quantity: 1
  - Includes: Built-in button
  - Pins: CLK, DT, SW, +, GND
  - ~$1-2 on AliExpress/Amazon
  - Very common Arduino module

### Display
- **20×4 LCD with I2C Backpack**
  - Quantity: 1
  - Color: Blue backlight recommended (easy to read)
  - Interface: I2C (only uses 2 pins: SDA, A4 / SCL, A5)
  - Address: Usually 0x27 or 0x3F
  - ~$6-8 on AliExpress/Amazon
  - Search: "20x4 LCD I2C Blue"
  - ⚠️ Make sure it HAS the I2C adapter soldered on the back!

### Power Supply
- **USB Power Bank**
  - Quantity: 1
  - Capacity: Any (5000-10000mAh recommended)
  - Output: 5V USB
  - Powers entire system via Arduino USB port
  - You probably already have one!
  - Alternative: 5V wall adapter with USB cable

---

## 2. Mechanical Components

### Encoder Wheel Setup
- **Wheel/Pulley**
  - Quantity: 1
  - Diameter: ~50mm (will calibrate in software)
  - Material: Aluminum, 3D printed (ABS/PETG), or rubber-coated
  - Bore: Match your encoder shaft (6mm or 8mm)
  - **Option 1:** Buy aluminum timing pulley (~$3-5)
  - **Option 2:** 3D print custom wheel (STL needed)

- **Bearings (608ZZ)**
  - Quantity: 2
  - Type: Skateboard bearings (8mm ID, 22mm OD)
  - ~$2 for 10pcs on AliExpress
  - Makes wheel spin smoothly

- **Axle/Shaft**
  - Quantity: 1
  - Type: M8 threaded rod or smooth 8mm rod
  - Length: 50-80mm (depends on your design)
  - Material: Steel
  - ~$1-2

- **Spider Coupling (Flexible Coupler)**
  - Quantity: 1
  - Size: 6mm to 8mm (or whatever matches encoder shaft to wheel axle)
  - Purpose: Connects encoder to wheel, absorbs misalignment
  - ~$2-3 on AliExpress
  - Search: "6mm 8mm flexible coupling"

### Enclosure
- **3D Printed Housing**
  - Material: PET-G or ABS (oil-resistant)
  - Quantity: 1 set (base + lid)
  - Purpose: Protects electronics from metal dust/oil
  - Design: Custom (STL files to be created)
  - Mounting: M3 or M4 screws for Arduino/display
  - Cable glands for encoder wires

### Mounting Hardware
- **Screws & Nuts** (M3, M4, M8)
  - M3 × 10mm screws: 10pcs (for Arduino/display mounting)
  - M4 × 16mm screws: 4pcs (enclosure assembly)
  - M8 nuts: 2pcs (axle locking)
  - Assorted washers
  - ~$2-3 for assortment kit

- **Bandsaw Mounting Bracket**
  - Material: Aluminum angle or 3D printed
  - Design: Custom to your bandsaw model
  - Purpose: Mounts encoder assembly to saw table
  - Fasteners: Bolts compatible with your saw

---

## 3. Wiring & Connectors

### Wire
- **Dupont Jumper Wires** (Female-Female)
  - Quantity: 20-30 assorted lengths
  - Purpose: Prototyping connections
  - ~$2 for 40pcs kit

- **22 AWG Stranded Wire** (Optional, for permanent install)
  - Colors: Red, Black, Yellow, Green, Blue
  - Length: 2-3 meters total
  - Purpose: Cleaner wiring than jumpers
  - ~$5-8 for assortment

### Connectors (Optional but Recommended)
- **JST-XH Connectors** (2.54mm pitch)
  - 3-pin: 2 sets (for encoders)
  - 4-pin: 1 set (for I2C display)
  - Purpose: Removable connections for service
  - ~$5 for assorted kit with crimper

### Heat Shrink Tubing
- Assorted sizes (2mm, 3mm, 5mm)
- Purpose: Protect solder joints
- ~$3 for kit

---

## 4. Tools Required

### Soldering (If making permanent connections)
- Soldering iron (30-60W)
- Solder (lead-free or 60/40)
- Flux (optional but helps)

### Hand Tools
- Screwdrivers (Phillips, flathead)
- Allen keys (hex wrenches) for set screws
- Wire strippers/cutters
- Pliers
- Drill (if mounting to metal bandsaw)

### 3D Printer (If printing enclosure)
- Printer bed size: >150×150mm
- Nozzle: 0.4mm standard
- LayerHeight: 0.2mm recommended

### Programming
- Computer with USB port
- PlatformIO (VS Code extension) - FREE
- USB cable (Type A to Mini-B for Nano)

---

## 5. Complete Shopping List

### Electronics (~$30-40)
| Item | Qty | Price | Source |
|------|-----|-------|--------|
| Arduino Nano | 1 | $4 | AliExpress |
| 600PPR Encoder | 1 | $20 | AliExpress |
| KY-040 Encoder | 1 | $2 | AliExpress |
| 20×4 LCD I2C | 1 | $7 | AliExpress |
| USB Power Bank | 1 | $0 (have) or $10 | Existing |

### Mechanical (~$10-15)
| Item | Qty | Price | Source |
|------|-----|-------|--------|
| 608ZZ Bearings | 2 | $1 | AliExpress |
| M8 Rod | 1 | $1 | Hardware store |
| Spider Coupling | 1 | $3 | AliExpress |
| Aluminum Pulley | 1 | $5 | AliExpress |
| Screws/Nuts Kit | 1 | $3 | Hardware store |

### Wiring (~$5-10)
| Item | Qty | Price | Source |
|------|-----|-------|--------|
| Dupont Wires | 40 | $2 | AliExpress |
| 22AWG Wire | 5m | $5 | AliExpress |
| JST Connectors | Kit | $5 | AliExpress |

### 3D Printing (~$2-5)
| Item | Qty | Price | Source |
|------|-----|-------|--------|
| PETG Filament | 50g | $2 | Your printer |

**TOTAL COST: ~$50-70** (less if you already have tools/power bank/wire)

---

## 6. Recommended Purchase Priority

### Phase 1: Core System (Test on Breadboard)
1. Arduino Nano
2. 600PPR Encoder
3. KY-040 Encoder
4. 20×4 LCD I2C
5. Dupont jumper wires
6. USB Power Bank (or use computer USB)

**Cost: ~$35**
**Goal:** Get software working, verify all electronics

### Phase 2: Mechanical Assembly
1. Bearings
2. Shaft/Rod
3. Spider coupling
4. Wheel/Pulley
5. Mounting hardware

**Cost: ~$15**
**Goal:** Build encoder wheel assembly

### Phase 3: Integration
1. Wire/connectors for clean install
2. 3D printed enclosure
3. Mounting bracket

**Cost: ~$10**
**Goal:** Final assembly on bandsaw

---

## 7. Wire Connections Reference

### Industrial Encoder → Arduino
- **Red** (VCC) → Arduino **5V**
- **Black** (GND) → Arduino **GND**
- **Green** (A) → Arduino **D2** (Interrupt pin)
- **White** (B) → Arduino **D3** (Interrupt pin)
- (Z index - not used, leave disconnected)

### KY-040 Menu Encoder → Arduino
- **+** → Arduino **5V**
- **GND** → Arduino **GND**
- **CLK** → Arduino **D4**
- **DT** → Arduino **D5**
- **SW** → Arduino **D6**

### 20×4 LCD I2C → Arduino
- **VCC** → Arduino **5V**
- **GND** → Arduino **GND**
- **SDA** → Arduino **A4**
- **SCL** → Arduino **A5**

### Power
- **USB Power Bank** → Arduino **USB port** (Mini-B connector)

---

## 8. Optional Upgrades

### Better Display
- **OLED 128×64** (1.3") - $8-10
  - Pros: Higher contrast, modern look
  - Cons: Smaller text, harder to read at distance
  - Only if you want "premium" feel

### Better Encoder
- **1000 PPR or 2000 PPR** - $30-50
  - Pros: Higher resolution
  - Cons: Overkill for bandsaw, more expensive
  - 600PPR is plenty!

### Weatherproofing
- **IP65 Enclosure** - $15-20
  - Pros: Fully sealed from dust/oil
  - Cons: Harder to service
  - Only if outdoor or super dirty environment

### Second Encoder (Future Feature)
- **Rotary encoder on blade tilt** - $20
  - Reads actual blade angle
  - Calculates push distance automatically
  - Requires firmware update (not yet implemented)

---

## 9. Print-Friendly Checklist

```
☐ Arduino Nano (ATmega328P)
☐ 600PPR Optical Encoder (NPN, 5V)
☐ KY-040 Rotary Encoder Module
☐ 20×4 LCD with I2C Backpack
☐ USB Power Bank or 5V Adapter
☐ 608ZZ Bearings (×2)
☐ M8 Rod (50-80mm)
☐ 6-8mm Spider Coupling
☐ Aluminum Pulley or 3D Printed Wheel
☐ Dupont Jumper Wires (40pcs)
☐ M3/M4 Screws & Nuts Assortment
☐ (Optional) JST Connectors
☐ (Optional) 3D Printer Filament (PETG/ABS)
```

---

## 10. Where to Buy

### **AliExpress** (Cheapest, 2-4 week shipping)
- Electronics: Arduino, Encoders, LCD
- Mechanical: Bearings, couplings, pulleys
- Wire/connectors

### **Amazon** (Faster, 1-2 day shipping)
- Same items, 2-3× price
- Good if you're in a hurry

### **Local Hardware Store**
- M8 rod, screws, nuts
- Wire, heat shrink
- Usually cheaper than online for basic hardware

### **Your 3D Printer** (If you have one)
- Enclosure parts
- Custom wheel
- Mounting brackets

---

**Questions? Ready to order? Let me know if you need help identifying specific part numbers or finding suppliers!**
