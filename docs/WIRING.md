# IronTrak - Wiring Diagram & Passive Components (STM32F4 Edition)

## Complete Connection Schematic

| Component | Pin | Wire Color | STM32 Pin | Function |
| :--- | :--- | :--- | :--- | :--- |
| **Industrial Encoder** | A | Green | **PB6** | TIM4_CH1 |
| | B | White | **PB7** | TIM4_CH2 |
| | VCC | Red | **5V** | Power |
| | GND | Black | **GND** | Ground |
| **Menu Encoder (KY-040)** | CLK | Yellow | **PB12** | Menu CLK |
| | DT | Orange | **PB13** | Menu DT |
| | SW | Blue | **PB14** | Menu SW |
| | + | Red | **5V** | Power |
| | GND | Black | **GND** | Ground |
| **LCD Display (I2C)** | SDA | Green | **PB9** | I2C SDA |
| | SCL | Yellow | **PB8** | I2C SCL |
| | VCC | Red | **5V** | Power |
| | GND | Black | **GND** | Ground |

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

| Encoder Pin | Wire Color | Connection | Notes |
| :--- | :--- | :--- | :--- |
| **VCC** | Red | **5V** | |
| **GND** | Black | **GND** | |
| **A** | Green | **PB6** | Requires 4.7kΩ Pull-up to 5V |
| **B** | White | **PB7** | Requires 4.7kΩ Pull-up to 5V |

### 2. KY-040 Menu Encoder → STM32

| Module Pin | Wire Color | STM32 Pin |
| :--- | :--- | :--- |
| **+** | Red | **5V** |
| **GND** | Black | **GND** |
| **CLK** | Yellow | **PB12** |
| **DT** | Orange | **PB13** |
| **SW** | Blue | **PB14** |

### 3. 20×4 LCD I2C → STM32

| Backpack Pin | Wire Color | STM32 Pin |
| :--- | :--- | :--- |
| **VCC** | Red | **5V** |
| **GND** | Black | **GND** |
| **SDA** | Green | **PB9** |
| **SCL** | Yellow | **PB8** |

---

## Power Distribution

| Source | Destination | Voltage |
| :--- | :--- | :--- |
| **USB Power Bank / PSU** | **STM32 Black Pill** | 5V (via USB or 5V pin) |
| **STM32 5V Pin** | **Industrial Encoder** | 5V |
| | **KY-040 Encoder** | 5V |
| | **LCD Display** | 5V |
| | **Pull-up Resistors** | 5V |
| **STM32 GND** | **All Components** | GND |

**Note:** The STM32 Black Pill has an onboard regulator that takes 5V from the USB or 5V pin and converts it to 3.3V for the chip. **Do not feed 5V into the 3.3V pin!**
