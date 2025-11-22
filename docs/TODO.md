# IronTrak Future Improvements

## Hardware Upgrades
- [ ] **External EEPROM (AT24C256)**
  - **Purpose:** Enable instant saving of statistics on every cut without freezing the system.
  - **Wiring:**
    - VCC -> 3.3V
    - GND -> GND
    - SDA -> PB9 (I2C1 SDA, shared with LCD)
    - SCL -> PB8 (I2C1 SCL, shared with LCD)
  - **Address:** 0x50 (Default)

## Software Features
- [ ] Implement `I2C_EEPROM` class to replace internal Flash emulation.
- [ ] Re-enable `Storage::save()` call in `StatsSys::registerCut()` once external EEPROM is active.
