#include "headers/DisplaySys.h"

// LCDBigNumbers library configuration
#define USE_SERIAL_2004_LCD
#include <LCDBigNumbers.hpp>
#include <Wire.h>

// Helper to print string with custom char 0 support
static void printStr(LiquidCrystal_I2C* lcd, String s) {
    for (unsigned int i = 0; i < s.length(); i++) {
        char c = s[i];
        if (c == '\x08') lcd->write((uint8_t)0); // Map \x08 backspace char to Custom Char 0 (Gear)
        else lcd->write((uint8_t)c);
    }
}

DisplaySys::DisplaySys() {
    _lcd = new LiquidCrystal_I2C(LCD_ADDR, LCD_COLS, LCD_ROWS);
    // Using LCDBigNumbers 3x2 VARIANT_2 (no 0xFF blocks, all custom chars)
    _bigNumbers = new LCDBigNumbers(_lcd, BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_2);
    _lastMM = -999.9;
    _lastIsInch = false;
    _lastBigValue = -999.9;
    _lastBigUnit = "";
    _lastValueChangeMillis = 0;
    _lastRawValue = -999.9;
    _wasSettled = false;
    _lastVelocity = 0.0;
    _inIdleMode = false;
    for (int i = 0; i < 4; i++) {
        _lastLine[i] = "";
    }
}

void DisplaySys::init() {
#if defined(STM32F4xx)
    Wire.setSDA(PIN_LCD_SDA);
    Wire.setSCL(PIN_LCD_SCL);
    Wire.begin(); 
#endif
    _lcd->begin();  // fdebrabander begin() takes no arguments
    _lcd->backlight();
    
    // Initialize big numbers font
    _bigNumbers->begin();
    
    // Override library's custom characters with consistent 2-pixel versions
    // VARIANT_2 uses 8 custom characters (0-7) with NO 0xFF blocks
    uint8_t customChar0[8] = { 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Upper bar (2 rows)
    uint8_t customChar1[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F }; // Lower bar (2 rows)
    uint8_t customChar2[8] = { 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F }; // Upper and lower bar (2+2 rows)
    uint8_t customChar3[8] = { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 }; // Left bar (2 pixels wide, full height)
    uint8_t customChar4[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18 }; // Left lower bar (2 pixels)
    uint8_t customChar5[8] = { 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x18, 0x18 }; // Left upper and lower (2 pixels)
    uint8_t customChar6[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x0E, 0x0E }; // Decimal point
    uint8_t customChar7[8] = { 0x00, 0x00, 0x0E, 0x0E, 0x0E, 0x00, 0x00, 0x00 }; // Colon
    
    _lcd->createChar(0, customChar0);
    _lcd->createChar(1, customChar1);
    _lcd->createChar(2, customChar2);
    _lcd->createChar(3, customChar3);
    _lcd->createChar(4, customChar4);
    _lcd->createChar(5, customChar5);
    _lcd->createChar(6, customChar6);
    _lcd->createChar(7, customChar7);
    
    _lcd->clear();
}

void DisplaySys::update() {
    // Placeholder
}

void DisplaySys::showIdle(float currentMM, float targetMM, uint8_t cutMode, uint8_t stockType, const char* stockStr, uint8_t faceVal, bool isInch) {
    // ==========================================
    // BIG NUMBER LAYOUT (2x2 INDUSTRIAL on rows 0-1)
    // ==========================================
    // Row 0-1: [52.3]           (CUSTOM INDUSTRIAL 2x2, CRISP 2-PIXEL BARS)
    //            CM (bottom right)
    // Row 2:   ====================
    // Row 3:   █ 20x40 ANG 45° F:20
    // ==========================================

    // CRITICAL: Reload library characters when returning from menu
    if (!_inIdleMode) {
        _bigNumbers->begin();  // Reload big number custom characters
        _inIdleMode = true;
        // Force full redraw
        _lastBigValue = -999.9;
        _lastBigUnit = "";
        // Clear only line 3 cache to force stock info redraw
        _lastLine[3] = "";
    }

    // Convert measurement to display value
    float rawValue;
    String unitStr;
    
    if (isInch) {
        rawValue = currentMM / 25.4;  // mm to inches
        unitStr = "IN";
    } else {
        rawValue = currentMM / 10.0;  // mm to cm
        unitStr = "CM";
    }
    
    // === VELOCITY-BASED ADAPTIVE FILTERING ===
    // Track value changes for velocity calculation and settling detection
    unsigned long currentMillis = millis();
    const unsigned long SETTLING_DELAY = 150;  // 150ms settling time
    
    // Calculate velocity (rate of change in cm/s or in/s)
    float velocity = 0.0;
    unsigned long deltaTime = currentMillis - _lastValueChangeMillis;
    
    if (abs(rawValue - _lastRawValue) > 0.01 && deltaTime > 0) {
        float deltaValue = abs(rawValue - _lastRawValue);
        velocity = (deltaValue / deltaTime) * 1000.0;  // Convert to per second
        _lastValueChangeMillis = currentMillis;
        _lastRawValue = rawValue;
        _lastVelocity = velocity;  // Store for smoothing
    }
    
    // Use smoothed velocity (prevents jitter)
    velocity = _lastVelocity;
    
    // Determine if settled
    bool isSettled = (currentMillis - _lastValueChangeMillis) > SETTLING_DELAY;
    float displayValue;
    
    // === VELOCITY-BASED ROUNDING ===
    // Velocity thresholds (adjust these for your preference)
    const float FAST_THRESHOLD = 5.0;   // cm/s or in/s - fast movement
    const float SLOW_THRESHOLD = 2.0;   // cm/s or in/s - slow movement
    
    if (isSettled) {
        // Settled: show exact value with 0.1 precision
        displayValue = rawValue;
    } else if (velocity > FAST_THRESHOLD) {
        // Fast movement: round to 1.0 (ultra smooth)
        displayValue = round(rawValue);
    } else if (velocity > SLOW_THRESHOLD) {
        // Medium movement: round to 0.5
        displayValue = round(rawValue * 2.0) / 2.0;
    } else {
        // Slow movement: round to 0.2 for finer granularity
        displayValue = round(rawValue * 5.0) / 5.0;
    }
    
    // Detect settling transition to force exact value update
    bool justSettled = (isSettled && !_wasSettled);
    _wasSettled = isSettled;
    
    // Redraw if: value changed OR unit changed OR just settled (to show exact value)
    // Use smaller threshold (0.15) for more responsive updates with velocity filtering
    if (abs(displayValue - _lastBigValue) > 0.15 || unitStr != _lastBigUnit || justSettled) {
        // === AUTO-RANGING LOGIC ===
        // If value > 999.9 CM, switch to Meters
        // 1000.0 CM -> 10.0 M (Single Decimal to fit 4 chars)
        float effectiveValue = displayValue;
        String effectiveUnit = "CM"; 
        int precision = 1; 
        
        // Strict check using unitStr
        if (unitStr == "CM" && effectiveValue >= 999.95) { 
            effectiveValue /= 100.0;
            effectiveUnit = "M";
            precision = 1; // Must use 1 decimal to keep length = 4 chars (fits without shifting)
        } else if (unitStr == "IN") {
             effectiveUnit = "IN";
        }
        
        // Format number
        char buffer[10];
        dtostrf(effectiveValue, 0, precision, buffer);
        String numStr = String(buffer);
        
        int originalLength = numStr.length();
        
        // === FIXED ANCHOR FORMATTING ===
        // Start at Column 4 (Shifted 1 box right from 3)
        // "99.9" (4 chars) -> Cols 4-19 (Ends at 19).
        // "CM" at 18,19 -> Overlap at 18,19.
        // User explicitly requested "move 1 column right" despite overlap.
        int startCol = 4;
        
        // === SMART CLEARING: Only clear when digit count changes ===
        static int lastNumLength = 0;
        if (originalLength != lastNumLength) {
            _lcd->setCursor(0, 0);
            _lcd->print("                    "); // 20 spaces
            _lcd->setCursor(0, 1);
            _lcd->print("                    "); // 20 spaces
            lastNumLength = originalLength;
        }
        
        // Draw big numbers
        _bigNumbers->setBigNumberCursor(startCol, 0);
        _bigNumbers->print(numStr.c_str());
        
        _lastBigValue = displayValue;
        _lastBigUnit = unitStr; // Track original unit
        
        // Unit label at row 1, fixed at column 18
        _lcd->setCursor(18, 1);
        if (effectiveUnit == "M") {
            _lcd->print(" M"); // Space then M (aligns M at 19)
        } else {
            _lcd->print("CM"); // CM at 18,19
        }
    } else {
        // Even if value didn't change, ensure unit is visible
        String currentUnit = "CM";
        if (unitStr == "IN") currentUnit = "IN";
        else if (displayValue >= 999.95) currentUnit = "M";
        
        _lcd->setCursor(18, 1);
        if (currentUnit == "M") {
            _lcd->print(" M");
        } else {
            _lcd->print("CM");
        }
    }
    
    // --- Line 2: Separator ---
    String line2 = "====================";
    if (line2 != _lastLine[2]) {
        printLine(2, line2);
        _lastLine[2] = line2;
    }
    
    // --- Line 3: Stock Info ---
    String stockIcon = "";
    if (stockType == 0) stockIcon = "\xDB";      // Rect: █ (full block) - WORKING!
    else if (stockType == 1) stockIcon = "L";    // Angle: L
    else if (stockType == 2) stockIcon = "D";    // Round: D (diameter)
    
    String line3 = stockIcon + " " + String(stockStr);
    line3 += " ANG " + String(cutMode) + "\xDF"; // "ANG 45°"
    
    if (cutMode > 0 && faceVal > 0) {
        line3 += " F:" + String(faceVal);
    }
    
    while(line3.length() < 20) line3 += " ";
    
    if (line3 != _lastLine[3]) {
        printLine(3, line3);
        _lastLine[3] = line3;
    }
}

void DisplaySys::showHiddenInfo(float kerfMM, float diameter, bool reverseDir, bool autoZeroEnabled) {
    _lcd->clear();
    
    // Line 0: Kerf and Diameter
    String line0 = "K:" + String(kerfMM, 1) + " D:" + String(diameter, 2);
    _lcd->setCursor(0, 0);
    printStr(_lcd, line0);
    
    // Line 1: Direction and Auto-Zero
    String line1 = "Dir:" + String(reverseDir ? "REV" : "NORM");
    line1 += " AZ:" + String(autoZeroEnabled ? "ON" : "OFF");
    _lcd->setCursor(0, 1);
    printStr(_lcd, line1);
    
    // Line 2: Instructions
    _lcd->setCursor(0, 2);
    printStr(_lcd, "Hold to view...");
    
    _lcd->setCursor(0, 3);
    printStr(_lcd, "Release to exit");
}

void DisplaySys::showMeasurement(float mm, bool isInch) {
    // Legacy method - can be removed or redirected to showIdle
    showIdle(mm, 0, 0, 0, "", 0, isInch);
}

void DisplaySys::showMenu(const char* title, String value, bool isEditMode) {
    // Mark that we're leaving idle mode (so custom chars get reloaded on return)
    if (_inIdleMode) {
        _inIdleMode = false;
        // Reload menu-specific custom characters (icons)
        createCustomChars();
    }
    
    // Optimized to prevent flickering (no clear())
    
    // Line 0: Title
    String line0 = String(title);
    while(line0.length() < 20) line0 += " ";
    
    if (line0 != _lastLine[0]) {
        _lcd->setCursor(0, 0);
        printStr(_lcd, line0);
        _lastLine[0] = line0;
    }
    
    // Line 1: Value
    String line1 = "";
    if (isEditMode) line1 += "> ";
    line1 += value;
    while(line1.length() < 20) line1 += " ";
    
    if (line1 != _lastLine[1]) {
        _lcd->setCursor(0, 1);
        printStr(_lcd, line1);
        _lastLine[1] = line1;
    }
    
    // Clear lines 2 & 3 if they are not empty (from previous screen)
    // We assume lines 2/3 are unused in menu for now
    String blank = "                    ";
    if (_lastLine[2] != blank) {
        _lcd->setCursor(0, 2);
        printStr(_lcd, blank);
        _lastLine[2] = blank;
    }
    if (_lastLine[3] != blank) {
        _lcd->setCursor(0, 3);
        printStr(_lcd, blank);
        _lastLine[3] = blank;
    }
}

void DisplaySys::showMenu4(String l0, String l1, String l2, String l3) {
    // Mark that we're leaving idle mode
    if (_inIdleMode) {
        _inIdleMode = false;
        // Reload menu-specific custom characters (icons)
        createCustomChars();
    }
    
    // Truncate to 20 chars max, then pad if needed
    if (l0.length() > 20) l0 = l0.substring(0, 20);
    if (l1.length() > 20) l1 = l1.substring(0, 20);
    if (l2.length() > 20) l2 = l2.substring(0, 20);
    if (l3.length() > 20) l3 = l3.substring(0, 20);
    
    while(l0.length() < 20) l0 += " ";
    while(l1.length() < 20) l1 += " ";
    while(l2.length() < 20) l2 += " ";
    while(l3.length() < 20) l3 += " ";
    
    // Update if changed
    if (l0 != _lastLine[0]) {
        _lcd->setCursor(0, 0);
        printStr(_lcd, l0);
        _lastLine[0] = l0;
    }
    if (l1 != _lastLine[1]) {
        _lcd->setCursor(0, 1);
        printStr(_lcd, l1);
        _lastLine[1] = l1;
    }
    if (l2 != _lastLine[2]) {
        _lcd->setCursor(0, 2);
        printStr(_lcd, l2);
        _lastLine[2] = l2;
    }
    if (l3 != _lastLine[3]) {
        _lcd->setCursor(0, 3);
        printStr(_lcd, l3);
        _lastLine[3] = l3;
    }
}

void DisplaySys::showError(const char* msg) {
    _lcd->clear();
    _lcd->setCursor(0, 0);
    printStr(_lcd, "ERROR:");
    _lcd->setCursor(0, 1);
    printStr(_lcd, msg);
}

void DisplaySys::clear() {
    _lcd->clear();
    for (int i = 0; i < 4; i++) {
        _lastLine[i] = "";
    }
}

void DisplaySys::printLine(int row, String text) {
    _lcd->setCursor(0, row);
    // Pad with spaces to clear old content
    while (text.length() < LCD_COLS) {
        text += " ";
    }
    printStr(_lcd, text);
}

void DisplaySys::createCustomChars() {
    // 0: Stats (Bar Chart) - Mapped via \x08
    // Replaces Gear icon to satisfy "Statistics icons are not as they were"
    uint8_t iconStats[8] = {0x00, 0x02, 0x0A, 0x0A, 0x1A, 0x1F, 0x00, 0x00};
    _lcd->createChar(0, iconStats);

    // 1: Rectangular Tube
    uint8_t iconRect[8] = {0x00, 0x1F, 0x11, 0x11, 0x11, 0x1F, 0x00, 0x00};
    _lcd->createChar(1, iconRect);
    
    // 2: Angle Iron (L)
    uint8_t iconAngle[8] = {0x00, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, 0x00};
    _lcd->createChar(2, iconAngle);
    
    // 3: Phi (Diameter)
    uint8_t iconPhi[8] = {0x04, 0x0E, 0x15, 0x15, 0x0E, 0x04, 0x04, 0x00};
    _lcd->createChar(3, iconPhi);

    // 4: Blade (Saw/Kerf)
    // Replaces Gear icon to satisfy "Kerf must have a blade"
    uint8_t iconBlade[8] = {0x00, 0x04, 0x0E, 0x1F, 0x0E, 0x04, 0x00, 0x00};
    _lcd->createChar(4, iconBlade);
}
