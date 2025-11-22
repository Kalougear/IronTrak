#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// HARDWARE DEFINITIONS (STM32F4 Black Pill)
// ============================================================================
// Encoder (TIM4)
#define PIN_ENCODER_A           PB6
#define PIN_ENCODER_B           PB7

// Menu Encoder (KY-040)
#define PIN_MENU_CLK            PB12
#define PIN_MENU_DT             PB13
#define PIN_MENU_SW             PB14

// Display (I2C) - Using PB8/PB9 with external 4.7k pull-ups
#define PIN_LCD_SDA             PB9   // SDA with 4.7kΩ to 3.3V
#define PIN_LCD_SCL             PB8   // SCL with 4.7kΩ to 3.3V

// Display Settings
#define LCD_COLS                20
#define LCD_ROWS                4
#define LCD_ADDR                0x27

// Measurement Settings
#define DEFAULT_WHEEL_DIA_MM    50.0
#define ENCODER_PPR             1024
#define PULSES_PER_REV          (ENCODER_PPR * 4) // Quadrature decoding

// ============================================================================
// EEPROM ADDRESS MAP
// ============================================================================
#define EEPROM_ADDR_MAGIC       0   // Byte: Magic number (0x42)
#define EEPROM_ADDR_DIA         4   // Float: Wheel Diameter
#define EEPROM_ADDR_UNITS       8   // Byte: 0=MM, 1=INCH
#define EEPROM_ADDR_DIR         9   // Byte: 0=Norm, 1=Rev
#define EEPROM_ADDR_TOT_CUTS    10  // Long: Total Cuts
#define EEPROM_ADDR_TOT_LEN     14  // Float: Total Length (Meters)
#define EEPROM_ADDR_PROJ_CUTS   18  // Long: Project Cuts
#define EEPROM_ADDR_PROJ_LEN    22  // Float: Project Length (Meters)
#define EEPROM_ADDR_KERF        26  // Float: Blade thickness in mm
#define EEPROM_ADDR_AUTO_ZERO_EN 30 // Byte: Auto-Zero enabled (0/1)
#define EEPROM_ADDR_AUTO_ZERO_TH 31 // Float: Auto-Zero threshold in mm
#define EEPROM_ADDR_CUT_MODE    35  // Byte: 0=0°, 1=45°, 2=Custom
#define EEPROM_ADDR_CUT_ANGLE   36  // Float: Custom angle in degrees
#define EEPROM_ADDR_STOCK_TYPE  40  // Byte: 0=Rect, 1=Angle, 2=Cyl
#define EEPROM_ADDR_STOCK_IDX   41  // Byte: Index into stock table
#define EEPROM_ADDR_FACE_IDX    42  // Byte: Face selection index

// ============================================================================
// SYSTEM SETTINGS
// ============================================================================
#define SERIAL_BAUD_RATE        115200
#define WATCHDOG_TIMEOUT_MS     2000

// ============================================================================
// STOCK LIBRARY (Metric)
// ============================================================================
extern const char* STOCK_RECT_MM[];
extern const uint8_t STOCK_RECT_MM_DIMS[][2];
extern const uint8_t STOCK_RECT_MM_COUNT;

extern const char* STOCK_ANGLE_MM[];
extern const uint8_t STOCK_ANGLE_MM_DIMS[][3];
extern const uint8_t STOCK_ANGLE_MM_COUNT;

extern const char* STOCK_CYL_MM[];
extern const uint8_t STOCK_CYL_MM_DIMS[];
extern const uint8_t STOCK_CYL_MM_COUNT;

// ============================================================================
// STOCK LIBRARY (Imperial)
// ============================================================================
extern const char* STOCK_RECT_IN[];
extern const float STOCK_RECT_IN_DIMS[][2];
extern const uint8_t STOCK_RECT_IN_COUNT;

extern const char* STOCK_ANGLE_IN[];
extern const float STOCK_ANGLE_IN_DIMS[][3];
extern const uint8_t STOCK_ANGLE_IN_COUNT;

extern const char* STOCK_CYL_IN[];
extern const float STOCK_CYL_IN_DIMS[];
extern const uint8_t STOCK_CYL_IN_COUNT;

#endif // CONFIG_H

