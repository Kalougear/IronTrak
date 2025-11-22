# IronTrak - Wiring Diagram & Passive Components (STM32F4 Edition)

## Complete Connection Schematic

```
╔══════════════════════════════════════════════════════════════════════════════╗
║                          IRONTRAK WIRING DIAGRAM                             ║
║                      (STM32F411 "Black Pill")                                ║
╚══════════════════════════════════════════════════════════════════════════════╝

                                  USB POWER BANK / 5V PSU
                                       │
                                       ▼
    ┌────────────────────────────────────────────────────────────────┐
    │                      STM32F4 BLACK PILL                        │
    │                                                                │
    │  PB6 ◄───────────┐                    ┌────────────► PB9 (SDA) │
    │ (TIM4_CH1)       │                    │                        │
    │                  │                    │                        │
    │  PB7 ◄───────────┤                    ├────────────► PB8 (SCL) │
    │ (TIM4_CH2)       │                    │                        │
    │                  │                    │                        │
    │  PB12 ◄──────────┤                    │                        │
    │  (Menu CLK)      │                    │                        │
    │                  │                    │                        │
    │  PB13 ◄──────────┤                    │                        │
    │  (Menu DT)       │                    │                        │
    │                  │                    │                        │
    │  PB14 ◄──────────┤                    │                        │
    │  (Menu SW)       │                    │                        │
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
    │  [G] A ──────────┐    │   │        │  CLK ────►PB12│     │ SDA ──► PB9│
    │  [W] B ──────────┤    │   │        │  DT  ────►PB13│     │ SCL ──► PB8│
    │                  │    │   │        │  SW  ────►PB14│     │            │
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
        └──────────┴────► PB6   │
                                │
                   5V           │
                    │           │
                 ┌──┴──┐        │
                 │ 4.7k│        │
                 │ Ω   │        │
                 └──┬──┘        │
                    │           │
                    └───────────┴────► PB7

```

---

## ⚠️ CRITICAL: 5V Tolerance

The STM32F4 runs on **3.3V logic**. However, specific pins are **5V Tolerant (FT)**.
We have carefully selected pins that can safely accept 5V signals:

- **PB6, PB7, PB8, PB9** are all 5V Tolerant.
- **PB12, PB13, PB14** are all 5V Tolerant.

**DO NOT** move connections to other pins (like PA0-PA7) without checking the datasheet, as you might fry the chip!

---

## Detailed Wire Connections

### 1. Industrial Encoder → STM32

```
ENCODER SIDE          WIRE COLOR    STM32 SIDE
┌──────────┐
│  VCC     │────[Red]──────────────► 5V
│          │
│  GND     │────[Black]────────────► GND
│          │                          │
│  A       │────[Green]────┬──────────┼──► PB6
│          │               │          │
│          │            ┌──┴──┐       │
│          │            │4.7kΩ│       │
│          │            └──┬──┘       │
│          │               │          │
│          │             [5V]◄────────┘
│          │
│  B       │────[White]────┬──────────┼──► PB7
│          │               │          │
│          │            ┌──┴──┐       │
│          │            │4.7kΩ│       │
│          │            └──┬──┘       │
│          │               │          │
│          │             [5V]◄────────┘
```

### 2. KY-040 Menu Encoder → STM32

```
KY-040 MODULE         WIRE COLOR    STM32 SIDE
┌──────────┐
│  +       │────[Red]──────────────► 5V
│          │
│  GND     │────[Black]────────────► GND
│          │
│  CLK     │────[Yellow]───────────► PB12
│          │
│  DT      │────[Orange]───────────► PB13
│          │
│  SW      │────[Blue]─────────────► PB14
└──────────┘
```

### 3. 20×4 LCD I2C → STM32

```
LCD I2C BACKPACK      WIRE COLOR    STM32 SIDE
┌──────────┐
│  VCC     │────[Red]──────────────► 5V
│          │
│  GND     │────[Black]────────────► GND
│          │
│  SDA     │────[Green]────────────► PB9
│          │
│  SCL     │────[Yellow]───────────► PB8
└──────────┘
```

---

## Power Distribution

```
USB POWER BANK / 5V PSU
      │
      ▼
  STM32 BLACK PILL
      │
      ├─[5V Pin]──┬──► Industrial Encoder VCC
      │           ├──► KY-040 VCC (+)
      │           ├──► LCD VCC
      │           └──► Pull-up Resistors
      │
      └─[GND]─────┬──► Industrial Encoder GND
                  ├──► KY-040 GND
                  └──► LCD GND
```

**Note:** The STM32 Black Pill has an onboard regulator that takes 5V from the USB or 5V pin and converts it to 3.3V for the chip. **Do not feed 5V into the 3.3V pin!**
