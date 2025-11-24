#ifndef LIQUIDCRYSTAL_I2C_COMPAT_H
#define LIQUIDCRYSTAL_I2C_COMPAT_H

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Compatibility wrapper to add command() method for LCDBigNumbers library
class LiquidCrystal_I2C_Compat : public LiquidCrystal_I2C {
public:
    LiquidCrystal_I2C_Compat(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows) 
        : LiquidCrystal_I2C(lcd_Addr, lcd_cols, lcd_rows) {}
    
    // Add the missing command() method required by LCDBigNumbers
    // Implements direct I2C communication to send command bytes
    void command(uint8_t value) {
        write4bits((value & 0xF0) | 0x00);  // High nibble, RS=0 (command)
        write4bits(((value << 4) & 0xF0) | 0x00);  // Low nibble, RS=0
    }
};

#endif
