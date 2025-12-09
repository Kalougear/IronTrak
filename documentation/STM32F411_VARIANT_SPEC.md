# STM32F411 Hardware Variant Specification

**IronTrak Hardware Configuration**

---

## 📋 Hardware Identification

**Board Variant:** STM32F411CEU6 Black Pill with External Flash  
**Marking:** F411 25M HSE 8MFlash  
**Manufacturer:** WeAct Studio (or clone)

---

## 🔧 Core Specifications

### Microcontroller: STM32F411CEU6

| Parameter             | Specification  | Notes                   |
| --------------------- | -------------- | ----------------------- |
| **Core**              | ARM Cortex-M4F | 32-bit RISC with FPU    |
| **Clock Speed**       | 100MHz (max)   | Configurable via PLL    |
| **Flash (Internal)**  | 512KB          | Program storage         |
| **RAM**               | 128KB          | SRAM (112KB + 16KB CCM) |
| **Package**           | LQFP48         | 7×7mm, 0.5mm pitch      |
| **Operating Voltage** | 1.7V - 3.6V    | Core voltage            |
| **I/O Voltage**       | 3.3V nominal   | 5V tolerant pins        |

---

## ⚡ Clock Configuration

### High-Speed External Crystal (HSE): 25MHz

**Standard Configuration:**

```
HSE Input: 25MHz crystal
↓
PLL Multiplier: ×4
↓
System Clock (SYSCLK): 100MHz
↓
AHB Clock: 100MHz (prescaler ÷1)
APB1 Clock: 50MHz (prescaler ÷2, max 50MHz)
APB2 Clock: 100MHz (prescaler ÷1, max 100MHz)
```

**PlatformIO Configuration:**

```ini
[env:blackpill_f411ce]
platform = ststm32
board = blackpill_f411ce
framework = arduino

; 25MHz HSE Crystal Configuration
board_build.f_cpu = 100000000L
build_flags =
    -DHSE_VALUE=25000000UL
    -DUSE_FULL_LL_DRIVER
```

**Alternative (8MHz HSE boards):**
If your board uses 8MHz HSE instead, use:

```ini
build_flags = -DHSE_VALUE=8000000UL
```

> [!NOTE]
> Most WeAct Black Pill F411 boards use **25MHz HSE** by default. Verify your crystal marking (labeled on oscillator can).

---

## 💾 External Flash: 8MB SPI Flash

### Flash Chip Specifications

| Parameter        | Typical Specification             |
| ---------------- | --------------------------------- |
| **Capacity**     | 8MB (64Mbit)                      |
| **Interface**    | SPI (Serial Peripheral Interface) |
| **Chip Model**   | W25Q64 or compatible              |
| **Speed**        | 104MHz clock (max)                |
| **Voltage**      | 2.7V - 3.6V                       |
| **Erase Cycles** | 100,000 typical                   |

---

### SPI Flash Pinout (Standard Mapping)

| Flash Pin | STM32 Pin | Function            | Notes           |
| --------- | --------- | ------------------- | --------------- |
| **CS**    | PA4       | Chip Select         | Active LOW      |
| **CLK**   | PA5       | SPI Clock (SCK)     | SPI1_SCK        |
| **MISO**  | PA6       | Master In Slave Out | SPI1_MISO       |
| **MOSI**  | PA7       | Master Out Slave In | SPI1_MOSI       |
| **VCC**   | 3.3V      | Power               | Shared with MCU |
| **GND**   | GND       | Ground              | Common ground   |

> [!CAUTION] > **Pin Conflict Warning:** If using external flash, **PA4-PA7 are NOT available** for other purposes (e.g., encoders, I2C). IronTrak uses **PB6-PB9** for encoders/I2C, so **no conflict** exists.

---

### External Flash Use Cases

#### 1. **Data Logging** (IronTrak Future Feature)

- Store cut history records (timestamp, length, material type)
- Capacity: ~500,000 cut records (16 bytes each)
- Persist across power cycles

#### 2. **Configuration Backup**

- Store calibration data (wheel diameter, PPR settings)
- Menu preferences (backlight timeout, units)
- Wi-Fi credentials (if future networking added)

#### 3. **Firmware Updates**

- Store new firmware image during OTA (Over-The-Air) update
- Bootloader swaps to new firmware after verification
- Rollback capability if update fails

#### 4. **Font/Graphics Storage**

- Custom LCD fonts for multiple languages
- Icons or bitmap graphics (if upgrading to graphical LCD)

---

### SPI Flash Library (Arduino Framework)

**Recommended Library:** [SPIFlash by LowPowerLab](https://github.com/LowPowerLab/SPIFlash)

**Installation (PlatformIO):**

```ini
lib_deps =
    lowpowerlab/SPIFlash@^101.1.3
```

**Basic Usage Example:**

```cpp
#include <SPIFlash.h>

// Initialize flash (CS on PA4)
SPIFlash flash(PA4);

void setup() {
    // Initialize SPI flash
    if (flash.initialize()) {
        Serial.println("Flash initialized");
        Serial.print("Capacity: ");
        Serial.print(flash.getCapacity() / 1024);
        Serial.println(" KB");
    }

    // Write data
    uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    flash.writeBytes(0x0000, data, 4);

    // Read data
    uint8_t buffer[4];
    flash.readBytes(0x0000, buffer, 4);
}
```

---

## 🔌 Available GPIO After Flash Mapping

### Pins Used by External Flash (SPI1)

- ❌ PA4 (CS)
- ❌ PA5 (SCK)
- ❌ PA6 (MISO)
- ❌ PA7 (MOSI)

### IronTrak Pin Usage

- ✅ PB6, PB7 (Encoder A/B - TIM4)
- ✅ PB8, PB9 (I2C SCL/SDA)
- ✅ PB12, PB13, PB14 (Menu encoder)

**GPIO Available for Expansion:** 28 pins remaining

---

## 📊 Memory Map Summary

### Internal Flash (512KB)

```
0x0800 0000 ─┬─ Bootloader (optional, 16KB)
             ├─ User Application (IronTrak firmware)
             │  • Code (.text section)
             │  • Constants (.rodata)
             │  • Initialized data (.data init values)
0x0807 FFFF ─┴─ End of flash
```

### SRAM (128KB)

```
0x2000 0000 ─┬─ Data (.data section)
             ├─ BSS (.bss section, zero-initialized)
             ├─ Heap (dynamic allocation, if used)
             ├─ Stack (grows downward)
0x2001 FFFF ─┴─ End of SRAM
```

### External SPI Flash (8MB)

```
0x0000 0000 ─┬─ Configuration sector (4KB)
             ├─ Data logging area (7.9MB)
             │  • Cut history records
             │  • Calibration backups
0x007F FFFF ─┴─ End of external flash (8,388,608 bytes)
```

---

## ⚙️ PlatformIO Configuration (Complete)

**File:** `platformio.ini`

```ini
[env:blackpill_f411ce]
platform = ststm32
board = blackpill_f411ce
framework = arduino

; Core configuration
board_build.f_cpu = 100000000L
board_build.mcu = stm32f411ceu6
upload_protocol = dfu

; 25MHz HSE crystal
build_flags =
    -DHSE_VALUE=25000000UL
    -DUSE_FULL_LL_DRIVER
    -DHAL_TIM_MODULE_ENABLED
    -DHAL_IWDG_MODULE_ENABLED

; Enable external flash support (optional)
    -DUSE_EXTERNAL_FLASH
    -DFLASH_CS_PIN=PA4
    -DFLASH_SIZE_MB=8

; Libraries
lib_deps =
    fdebrabander/LiquidCrystal_I2C@^1.1.2
    arminjo/LCDBigNumbers@^2.0.0
    lowpowerlab/SPIFlash@^101.1.3

; Serial monitor
monitor_speed = 115200
monitor_filters = default, time
```

---

## 🔍 Hardware Verification Commands

### Check HSE Crystal Frequency

```cpp
// In setup()
Serial.print("HSE Frequency: ");
Serial.print(HSE_VALUE / 1000000);
Serial.println(" MHz");

Serial.print("System Clock: ");
Serial.print(SystemCoreClock / 1000000);
Serial.println(" MHz");
```

**Expected Output:**

```
HSE Frequency: 25 MHz
System Clock: 100 MHz
```

### Check External Flash

```cpp
#include <SPIFlash.h>

SPIFlash flash(PA4);

void setup() {
    Serial.begin(115200);

    if (flash.initialize()) {
        Serial.println("✓ Flash detected");

        // Read JEDEC ID
        uint32_t jedec = flash.readDeviceId();
        Serial.print("JEDEC ID: 0x");
        Serial.println(jedec, HEX);

        // Typical: 0xEF4017 (Winbond W25Q64)
    } else {
        Serial.println("✗ Flash NOT detected");
    }
}
```

---

## 🛑 Known Issues & Workarounds

### Issue 1: USB DFU Upload Fails

**Symptom:** "No DFU device found" when uploading firmware

**Cause:** Black Pill not in DFU mode

**Solution:**

1. Hold BOOT0 button
2. Press and release RESET button
3. Release BOOT0 button
4. Run `pio run -t upload` within 5 seconds

### Issue 2: External Flash Not Detected

**Symptom:** `flash.initialize()` returns `false`

**Possible Causes:**

1. Flash chip not soldered (some boards ship without flash installed)
2. Incorrect CS pin (verify PA4 is CS, check schematic)
3. SPI clock too fast (reduce to 1MHz for testing)

**Workaround:**

```cpp
// Slow down SPI clock for testing
SPISettings flashSettings(1000000, MSBFIRST, SPI_MODE0);
flash.begin(flashSettings);
```

### Issue 3: Incorrect System Clock

**Symptom:** Timers running slower/faster than expected

**Cause:** Wrong `HSE_VALUE` in build flags

**Verification:**

```cpp
// Should print 100000000
Serial.println(SystemCoreClock);
```

**Fix:** Ensure `platformio.ini` has:

```ini
build_flags = -DHSE_VALUE=25000000UL
```

---

## 📚 Additional Resources

### Datasheets

- [STM32F411xC/xE Datasheet](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)
- [STM32F411 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00119316.pdf)
- [W25Q64 Flash Datasheet](https://www.winbond.com/resource-files/w25q64jv%20revj%2003272018%20plus.pdf)

### Community Resources

- [WeAct STM32F411 GitHub](https://github.com/WeActTC/MiniSTM32F4x1)
- [STM32duino Forum](https://www.stm32duino.com/)
- [PlatformIO STM32 Platform](https://docs.platformio.org/en/latest/platforms/ststm32.html)

---

## ✅ Hardware Compatibility Matrix

| Feature            | Standard Black Pill | Your Variant (8M Flash)   | IronTrak Usage               |
| ------------------ | ------------------- | ------------------------- | ---------------------------- |
| **MCU**            | STM32F411CEU6       | ✓ Same                    | ✓ Compatible                 |
| **HSE Crystal**    | 25MHz               | ✓ 25MHz                   | ✓ Standard config            |
| **Internal Flash** | 512KB               | ✓ 512KB                   | ✓ Sufficient                 |
| **RAM**            | 128KB               | ✓ 128KB                   | ✓ Sufficient                 |
| **External Flash** | ❌ None             | ✓ 8MB SPI                 | ✓ Optional (future features) |
| **USB**            | ✓ USB-C             | ✓ USB-C                   | ✓ For power + programming    |
| **Available GPIO** | 32 pins             | 28 pins (4 used by flash) | ✓ IronTrak uses PB pins      |

---

**Conclusion:** Your hardware variant is **fully compatible** with IronTrak and provides **8MB of bonus storage** for future data logging features!

---

**Last Updated:** 2025-11-28  
**Hardware Version:** v1.0 (8MB Flash Variant)  
**Firmware Compatibility:** v1.0.0+
