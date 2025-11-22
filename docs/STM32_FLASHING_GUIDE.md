# STM32F4 Black Pill - Flashing & USB Serial Guide

## Quick Start - Flashing Firmware

### Step 1: Prepare Hardware
1. Connect a **jumper wire** from **PA10** to any **GND** pin on the Black Pill
2. Connect the Black Pill to your PC via **USB cable** (must support data transfer)

### Step 2: Enter DFU Bootloader Mode
1. **Press and HOLD** the **BOOT0** button on the board
2. **Press and RELEASE** the **RESET** button (while still holding BOOT0)
3. **Release** the **BOOT0** button
4. The board is now in DFU mode (ready to receive firmware)

### Step 3: Install USB Drivers (First Time Only)
1. Download **Zadig** from https://zadig.akeo.ie/
2. Run `zadig.exe`
3. Click **Options** → Check ✅ **List All Devices**
4. Select **STM32 BOOTLOADER** from the dropdown
5. Choose **WinUSB** as the target driver (use arrows if needed)
6. Click **Install Driver** or **Replace Driver**
7. Wait for completion

### Step 4: Upload Firmware
```bash
pio run --target upload
```

### Step 5: Run Your Code
1. **Remove** the PA10 jumper wire (no longer needed during normal operation)
2. **Press RESET** button to start your firmware
3. Your code is now running!

---

## USB Serial Debugging

### Open Serial Monitor
Once your code is running, monitor serial output:
```bash
pio device monitor
```

Press `Ctrl+C` to exit the monitor.

### In Your Code
Use `Serial` normally:
```cpp
Serial.begin(115200);
Serial.println("Hello from Black Pill!");
```

The USB cable handles both programming **and** serial communication!

---

## Understanding the Theory

### Why PA10 Must Be Grounded

The STM32F411 chip has a **built-in bootloader** that automatically decides how to communicate with the outside world when it powers up. It checks multiple interfaces in this order:

1. **Serial UART** (via PA9/PA10 pins)
2. **USB DFU** (via the USB port)

**The Problem:**  
PA10 is the **USART1 RX** pin. When it's **floating** (not connected to anything), electrical noise from the environment can induce small voltage fluctuations. The bootloader interprets these random signals as "someone is trying to talk to me via Serial!" and **waits forever** for a valid serial handshake.

**The Solution:**  
Grounding PA10 during boot forces the pin to a stable LOW state. The bootloader sees no serial activity and moves on to check the **USB interface**, where it finds your PC and enters DFU mode successfully.

**Why Remove It Later?**  
Once your firmware is running, the bootloader is no longer active. Your code can use PA10 normally (if needed), so the jumper can be removed.

---

### What is DFU Mode?

**DFU** = **Device Firmware Update**

It's a USB standard protocol that allows microcontrollers to receive new firmware over USB without needing an external programmer (like ST-Link).

The STM32F411 has a **factory-programmed bootloader** stored in a protected area of memory (ROM). This bootloader:
- Cannot be erased or corrupted
- Activates when BOOT0 is HIGH during reset
- Presents itself to the PC as a "STM32 BOOTLOADER" USB device
- Accepts new firmware and writes it to Flash memory

**Why BOOT0?**  
The BOOT0 pin controls where the chip starts executing code:
- **BOOT0 = LOW (normal):** Run user firmware from Flash memory
- **BOOT0 = HIGH (bootloader):** Run built-in DFU bootloader from ROM

---

### USB CDC Serial (Virtual COM Port)

Your `platformio.ini` has these flags:
```ini
-D PIO_FRAMEWORK_ARDUINO_ENABLE_CDC
-D USBCON
```

These enable **USB CDC (Communications Device Class)**, which makes the Black Pill appear as a **Virtual COM Port** to your PC when your firmware is running.

**Two Modes, One Cable:**
- **DFU Mode:** Board appears as "STM32 BOOTLOADER" (used for uploading)
- **Running Firmware:** Board appears as "USB Serial Device" (used for Serial.print debugging)

When you upload new firmware, Windows sees the device "disconnect" (COM port) and "reconnect" (DFU bootloader), then switch back again.

---

## Troubleshooting

### "No DFU device found"
- Check PA10 is grounded
- Verify BOOT0 button sequence
- Try different USB port
- Reinstall WinUSB driver with Zadig

### "USB device not recognized" (Code 43)
- Bad USB cable (must support data, not just power)
- USB port issue (try motherboard rear ports)
- PA10 not grounded

### Serial Monitor shows nothing
- Make sure code is running (not in DFU mode)
- Press RESET after upload to start firmware
- Wait 2-3 seconds for COM port to appear
- Check baud rate matches (115200 recommended)

### Can't enter DFU mode on subsequent uploads
- Your code is running, so the board is a COM port, not DFU device
- Manually enter DFU mode again (BOOT0 + RESET sequence)
- PA10 jumper only needed during DFU entry if you're having issues

---

## Quick Reference

| Action | Hardware | Command |
| :--- | :--- | :--- |
| **Enter DFU** | PA10→GND, BOOT0+RESET | - |
| **Upload firmware** | In DFU mode | `pio run -t upload` |
| **Run firmware** | Remove PA10, press RESET | - |
| **Debug output** | Firmware running | `pio device monitor` |
| **Full build & upload** | In DFU mode | `pio run` then upload |

---

**Note:** Once you're familiar with the process, you may find PA10 grounding is only needed when DFU mode is being stubborn. Many users can enter DFU without it, but it's a guaranteed fix when things go wrong.
