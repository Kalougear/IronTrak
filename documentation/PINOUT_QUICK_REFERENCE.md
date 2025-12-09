# IronTrak Pinout Quick Reference

**Operational Hardware Connections Only**  
_Excludes programming pins (USB, UART, SWD)_

---

## 📡 SIGNAL CONNECTIONS

### Main Encoder (1024 PPR Industrial Optical)

| STM32 Pin | Function             | Wire Color | Connects To        | Pull-Up Required   |
| --------- | -------------------- | ---------- | ------------------ | ------------------ |
| **PB6**   | Encoder A (TIM4_CH1) | Yellow     | Encoder Black wire | Yes, 4.7kΩ to 3.3V |
| **PB7**   | Encoder B (TIM4_CH2) | Green      | Encoder White wire | Yes, 4.7kΩ to 3.3V |

**Note:** Encoder A/B wires must be **twisted pair** (3-5 twists per inch) to reduce EMI.

---

### I2C Bus (LCD Display + Optional AS5600)

| STM32 Pin | Function | Wire Color | Connects To          | Pull-Up Required         |
| --------- | -------- | ---------- | -------------------- | ------------------------ |
| **PB9**   | I2C SDA  | Blue       | LCD SDA + AS5600 SDA | Optional\* 4.7kΩ to 3.3V |
| **PB8**   | I2C SCL  | White      | LCD SCL + AS5600 SCL | Optional\* 4.7kΩ to 3.3V |

**\*Pull-Up Note:** Most LCD I2C backpacks have onboard 10kΩ pull-ups. Test with multimeter before adding external pull-ups.

---

### Menu Input (KY-040 Rotary Encoder)

| STM32 Pin | Function | Wire Color | Connects To | Notes                    |
| --------- | -------- | ---------- | ----------- | ------------------------ |
| **PB12**  | Menu CLK | Orange     | KY-040 CLK  | Quadrature A             |
| **PB13**  | Menu DT  | Orange     | KY-040 DT   | Quadrature B             |
| **PB14**  | Menu SW  | Orange     | KY-040 SW   | Push button (active LOW) |

---

## 🔌 POWER CONNECTIONS

### STM32F411 Black Pill Power

| STM32 Pin | Voltage     | Source         | Connects To             | Notes             |
| --------- | ----------- | -------------- | ----------------------- | ----------------- |
| **5V**    | 5V input    | USB Power Bank | 5V rail (red wire)      | Main power input  |
| **GND**   | Ground      | Power Bank GND | GND rail (black wire)   | Multiple GND pins |
| **3.3V**  | 3.3V output | Black Pill LDO | 3.3V rail (orange wire) | Max 150mA output  |

---

### 20×4 LCD I2C Power

| LCD Pin | Voltage | Connects To | Wire Color |
| ------- | ------- | ----------- | ---------- |
| **VCC** | 5V      | 5V rail     | Red        |
| **GND** | Ground  | GND rail    | Black      |

---

### Industrial Encoder Power

| Encoder Wire | Voltage   | Connects To             | Notes                   |
| ------------ | --------- | ----------------------- | ----------------------- |
| **Brown**    | 5V or 12V | 5V rail (or 12V supply) | Check encoder datasheet |
| **Blue**     | Ground    | GND rail                | Common ground           |

**IMPORTANT:** Verify encoder voltage rating before connecting (5-24V range typical).

---

### KY-040 Menu Encoder Power

| KY-040 Pin | Voltage | Connects To | Wire Color |
| ---------- | ------- | ----------- | ---------- |
| **+**      | 3.3V    | 3.3V rail   | Orange     |
| **GND**    | Ground  | GND rail    | Black      |

**CRITICAL:** KY-040 must use 3.3V (not 5V) to avoid damaging STM32 inputs.

---

### Optional: AS5600 Angle Sensor Power

| AS5600 Pin | Voltage | Connects To | Wire Color |
| ---------- | ------- | ----------- | ---------- |
| **VCC**    | 3.3V    | 3.3V rail   | Orange     |
| **GND**    | Ground  | GND rail    | Black      |

---

## 📊 Complete Pin Summary

**Signal Pins Used:** 7 total

- Encoder: PB6, PB7
- I2C: PB8, PB9
- Menu: PB12, PB13, PB14

**Power Pins Used:** 3 total

- 5V input (USB power)
- GND (ground reference)
- 3.3V output (regulated from Black Pill)

**Pins Available:** 33 unused GPIO pins for future expansion

---

## 🔧 Component Passive Requirements

**Pull-Up Resistors:**

- R1: 4.7kΩ between 3.3V and PB6 (Encoder A)
- R2: 4.7kΩ between 3.3V and PB7 (Encoder B)
- R3, R4: 4.7kΩ on PB8/PB9 (optional, check LCD first)

**Decoupling Capacitors:**

- C1: 10μF electrolytic on 5V rail
- C2: 0.1μF ceramic near STM32 VDD pin
- C3: 10μF electrolytic on 3.3V rail (optional but recommended)

---

**Last Updated:** 2025-11-28  
**Hardware Version:** v1.0  
**Compatible Firmware:** v1.0.0+

---

> [!TIP]
> Print this page and laminate for shop floor reference during assembly and troubleshooting.
