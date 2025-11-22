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
static const char* STOCK_RECT_MM[] = {
    "20x20", "20x40", "25x25", "30x30", "35x35", "40x40", "50x50"
};
static const uint8_t STOCK_RECT_MM_DIMS[][2] = {
    {20,20}, {20,40}, {25,25}, {30,30}, {35,35}, {40,40}, {50,50}
};
static const uint8_t STOCK_RECT_MM_COUNT = 7;

static const char* STOCK_ANGLE_MM[] = {
    "L20x20x3", "L25x25x3", "L30x30x3", "L40x40x4", "L50x50x5"
};
static const uint8_t STOCK_ANGLE_MM_DIMS[][3] = {
    {20,20,3}, {25,25,3}, {30,30,3}, {40,40,4}, {50,50,5}
};
static const uint8_t STOCK_ANGLE_MM_COUNT = 5;

static const char* STOCK_CYL_MM[] = {
    "D16", "D20", "D25", "D30", "D40"
};
static const uint8_t STOCK_CYL_MM_DIMS[] = {16, 20, 25, 30, 40};
static const uint8_t STOCK_CYL_MM_COUNT = 5;

// ============================================================================
// STOCK LIBRARY (Imperial)
// ============================================================================
static const char* STOCK_RECT_IN[] = {
    "1/2x1/2", "1x1", "1x2", "2x2"
};
static const float STOCK_RECT_IN_DIMS[][2] = {
    {12.7,12.7}, {25.4,25.4}, {25.4,50.8}, {50.8,50.8}
};
static const uint8_t STOCK_RECT_IN_COUNT = 4;

static const char* STOCK_ANGLE_IN[] = {
    "L1x1x1/8", "L1.5x1.5x1/8", "L2x2x1/8"
};
static const float STOCK_ANGLE_IN_DIMS[][3] = {
    {25.4,25.4,3.2}, {38.1,38.1,3.2}, {50.8,50.8,3.2}
};
static const uint8_t STOCK_ANGLE_IN_COUNT = 3;

static const char* STOCK_CYL_IN[] = {
    "D1/2", "D3/4", "D1", "D1.5"
};
static const float STOCK_CYL_IN_DIMS[] = {12.7, 19.05, 25.4, 38.1};
static const uint8_t STOCK_CYL_IN_COUNT = 4;

#endif // CONFIG_H
