# ST-Link V3 MINI Wiring Guide

## Hardware Setup

### Wiring Table

| Cable Color | ST-Link Signal | Black Pill Pin | Function |
| :--- | :--- | :--- | :--- |
| **Green** | CLK | **SWCLK (PA14)** | Clock Signal |
| **Yellow** | TMS / SWDIO | **SWDIO (PA13)** | Data Signal |
| **Black** | GND | **G (GND)** | Ground |
| **White** | RX (ST-Link Input) | **A9 (TX1)** | Serial Console (Board → PC) |
| **Brown** | TX (ST-Link Output) | **A10 (RX1)** | Serial Console (PC → Board) |
| **Orange** | RST | **NRST (R)** | Reset (Optional) |
| **Red** | VCC (Sense) | **3.3V** | Voltage Reference |

### Visual Pin Locations

```
ST-Link V3 MINI         STM32F411 Black Pill
┌──────────────┐        ┌────────────────────┐
│              │        │                    │
│  [Green]   ──┼────────┼──► SWCLK (PA14)    │
│  [Yellow]  ──┼────────┼──► SWDIO (PA13)    │
│  [Black]   ──┼────────┼──► GND             │
│  [White]   ──┼────────┼──► A9 (TX1)        │
│  [Brown]   ──┼────────┼──► A10 (RX1)       │
│  [Orange]  ──┼────────┼──► NRST (R)        │
│  [Red]     ──┼────────┼──► 3.3V            │
│              │        │                    │
└──────────────┘        └────────────────────┘
                               ▲
                               │
                        USB-C (Power)
```

---

## ⚠️ Critical Setup Requirements

### 1. Power the Black Pill Separately
- The **Red wire (VCC)** is a **voltage sense line**, NOT a power supply
- **You MUST keep the Black Pill plugged into your PC via USB-C** for power
- The ST-Link will **not** power the board through the red wire

### 2. TX/RX Crossover Explained
The serial wires cross over because data flows in opposite directions:

| ST-Link Side | Direction | Black Pill Side |
| :--- | :---: | :--- |
| **RX** (White) receives | ← | **TX** (A9) transmits |
| **TX** (Brown) transmits | → | **RX** (A10) receives |

**Mnemonic:** "ST-Link RX connects to Black Pill TX" (receiver gets transmitter's data)

---

## PlatformIO Configuration

Update your `platformio.ini`:

```ini
[env:blackpill_f411ce]
platform = ststm32
board = blackpill_f411ce
framework = arduino

; ST-Link Upload
upload_protocol = stlink

; Optional: Monitor via ST-Link serial (if wired)
monitor_port = COM3  ; Change to your ST-Link VCP port
monitor_speed = 115200

build_flags = 
    -D PIO_FRAMEWORK_ARDUINO_ENABLE_CDC
    -D USBCON
    -D USBD_VID=0x0483
    -D USBD_PID=0x5740
    -D USB_MANUFACTURER="Unknown"
    -D USB_PRODUCT="BLACKPILL_F411CE"
    -D HAL_PCD_MODULE_ENABLED
    -D HAL_PKA_MODULE_DISABLED
```

---

## Usage

### Upload Firmware
No need to enter DFU mode or press buttons!

```bash
pio run --target upload
```

The ST-Link handles:
- Automatic reset
- Flash programming
- Verification

### Monitor Serial Output

#### Option 1: Via ST-Link VCP (White/Brown wires)
```bash
pio device monitor
```

This uses the UART connection (A9/A10) through the ST-Link's Virtual COM Port.

#### Option 2: Via Black Pill USB CDC
Keep the Black Pill USB-C plugged in and use USB Serial as before.

---

## Advantages of ST-Link

| Feature | DFU Mode | ST-Link |
| :--- | :---: | :---: |
| **Requires button sequence** | ✅ Yes | ❌ No |
| **Requires PA10 grounding** | Sometimes | ❌ Never |
| **Auto-reset on upload** | ❌ Manual | ✅ Automatic |
| **Debugging (breakpoints)** | ❌ No | ✅ Yes |
| **Serial monitoring** | Via USB CDC | Via UART or USB CDC |
| **Upload reliability** | Can be finicky | ✅ Rock solid |

---

## Troubleshooting

### "No ST-Link detected"
- Check all wire connections (especially GND and power)
- Ensure Black Pill is powered via USB-C
- Try different USB port for ST-Link
- Update ST-Link firmware using STM32CubeProgrammer

### "Target not found" or "Connection failed"
- Verify SWCLK (Green) and SWDIO (Yellow) are correct
- Check GND (Black) is connected
- Ensure 3.3V (Red) is sensing correct voltage
- Try adding RST (Orange) wire if not connected

### Serial monitor shows garbage
- Check baud rate (115200)
- Verify White/Brown wires are on A9/A10 (not swapped)
- Make sure `Serial.begin(115200)` is in your code

### Can't upload after switching from DFU
- Remove any Zadig WinUSB drivers for ST-Link
- ST-Link needs its native drivers, not WinUSB
- Reinstall ST-Link drivers using ST-Link utility

---

## Pin Reference (Black Pill)

```
       ┌─────────────┐
   3.3V│●          ●│GND
    GND│●          ●│GND
     5V│●          ●│3.3V
    PB9│●          ●│PB8   ← I2C (LCD)
   PB7 │●          ●│PB6   ← Encoder (TIM4)
   PB5 │●          ●│PB3
   PB4 │●          ●│PA15
   PB3 │●          ●│PA12
   PA15│●          ●│PA11
   PA10│●          ●│PA10  ← [Brown] ST-Link TX
    PA9│●          ●│PA9   ← [White] ST-Link RX
    PA8│●          ●│PA8
   PB15│●          ●│PB12  ← Menu encoder
   PB14│●          ●│PB13  ← Menu encoder
   PB13│●          ●│PB14  ← Menu encoder
   PB12│●          ●│PB15
     R │●          ●│PA0   ← [Orange] ST-Link RST
  PA13 │●          ●│PA1   ← [Yellow] SWDIO
  PA14 │●          ●│PA2   ← [Green] SWCLK
       └─────────────┘
```

---

## Notes

- **RST wire (Orange)** is optional but recommended for reliable uploads
- **Red wire (VCC sense)** must be connected even though it doesn't provide power
- You can use **both** ST-Link serial (UART) **and** USB CDC serial simultaneously if needed
- ST-Link is generally more reliable than DFU for development work
- Once you switch to `upload_protocol = stlink`, DFU mode is no longer needed

---

## Quick Reference Commands

| Action | Command |
| :--- | :--- |
| **Upload** | `pio run --target upload` |
| **Monitor (UART)** | `pio device monitor` |
| **Upload + Monitor** | `pio run -t upload && pio device monitor` |
| **Clean build** | `pio run --target clean` |
| **Check connection** | Verify in Device Manager (ST-Link VCP) |
