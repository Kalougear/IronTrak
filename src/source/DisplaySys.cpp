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
    // GEMINI.md Rule 4.2: ZERO dynamic allocation - use static instances
    static LiquidCrystal_I2C lcdInstance(LCD_ADDR, LCD_COLS, LCD_ROWS);
    _lcd = &lcdInstance;
    
    // Using LCDBigNumbers 3x2 VARIANT_2 (no 0xFF blocks, all custom chars)
    static LCDBigNumbers bigNumInstance(&lcdInstance, BIG_NUMBERS_FONT_3_COLUMN_2_ROWS_VARIANT_2);
    _bigNumbers = &bigNumInstance;
    
    // GEMINI.md Rule 4.5: Float literals must have 'f' suffix
    _lastMM = -999.9f;
    _lastIsInch = false;
    _lastBigValue = -999.9f;
    _lastBigUnit = "";
    _lastValueChangeMillis = 0;
    _lastRawValue = -999.9f;
    _wasSettled = false;
    _lastVelocity = 0.0f;
    _inIdleMode = false;
    _isTransitioning = false;
    _transitionStartMillis = 0;
    
    // Backlight power management initialization
    _lastActivityMillis = 0;
    _backlightOn = true;
    _timeoutSeconds = 0;  // Will be set by main.cpp from settings
    
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
    // GEMINI.md Rule 4.2: Backlight auto-off logic
    // Skip if timeout disabled (0 = always ON)
    if (_timeoutSeconds == 0) return;
    
    // Calculate idle time
    uint32_t currentMillis = millis();
    uint32_t idleMillis = currentMillis - _lastActivityMillis;
    uint32_t timeoutMillis = (uint32_t)_timeoutSeconds * 1000;
    
    // Auto-off after timeout
    if (_backlightOn && idleMillis > timeoutMillis) {
        _lcd->noBacklight();
        _backlightOn = false;
    }
}

void DisplaySys::showIdle(float currentMM, float /* targetMM */, uint8_t cutMode, uint8_t stockType, const char* stockStr, uint8_t faceVal, bool isInch, bool /* reverseDir  */) {
    // GEMINI.md Rule 4.2: Assertion 1 - Validate pointer parameters
    if (stockStr == nullptr) {
        stockStr = "???";  // Safe fallback
    }
    
    // ==========================================
    // GEMINI.md Rule 4.2: Refactored to < 60 lines
    // Logic extracted to helper functions
    // ==========================================
    
    // CRITICAL: Reload library characters when returning from menu
    if (!_inIdleMode) {
        _inIdleMode = true;
        _isTransitioning = true;
        _transitionStartMillis = millis();
        
        // Clear screen - fast and prevents corruption during char reload
        _lcd->clear();
        
        // Reload big number custom characters
        _bigNumbers->begin();
        
        // Force full redraw
        _lastBigValue = -999.9f;
        _lastBigUnit = "";
        
        // Reset all line caches
        for (int i = 0; i < 4; i++) {
            _lastLine[i] = "";
        }
    }
    // Settling delay: suppress rendering for 200ms
if (_isTransitioning) {
    if (millis() - _transitionStartMillis < 200) {
        return;  // Exit early during settling
    }
    _isTransitioning = false;
}
    
    // Convert measurement to display value
    float rawValue;
    String unitStr;
    
    if (isInch) {
        rawValue = currentMM / 25.4f;  // mm to inches
        unitStr = "IN";
    } else {
        rawValue = currentMM / 10.0f;  // mm to cm
        unitStr = "CM";
    }
    
    // Apply velocity-based adaptive filtering (Helper 1)
    unsigned long currentMillis = millis();
    float displayValue = applyVelocityFiltering(rawValue, isInch, currentMillis);
    
    // Detect settling transition to force exact value update
    bool isSettled = (currentMillis - _lastValueChangeMillis) > 150;
    bool justSettled = (isSettled && !_wasSettled);
    _wasSettled = isSettled;
    
    // Render big number with unit (Helper 2)
    renderBigNumber(displayValue, unitStr, justSettled);
    
    // Render separator line (Helper 3)
    renderSeparatorLine();
    
    // Render stock info line (Helper 4)
    renderInfoLine(stockType, stockStr, cutMode, faceVal);
}

// ============================================================================
// GEMINI.md Rule 4.2: Helper Functions (Extracted from showIdle)
// ============================================================================

// Helper 1: Apply velocity-based filtering to smooth measurement display
float DisplaySys::applyVelocityFiltering(float rawValue, bool /* isInch */, unsigned long currentMillis) {
    const unsigned long SETTLING_DELAY = 150;  // 150ms settling time
    
    // Calculate velocity (rate of change in cm/s or in/s)
    float velocity = 0.0f;
    unsigned long deltaTime = currentMillis - _lastValueChangeMillis;
    
    if (abs(rawValue - _lastRawValue) > 0.01f && deltaTime > 0) {
        float deltaValue = abs(rawValue - _lastRawValue);
        velocity = (deltaValue / deltaTime) * 1000.0f;  // Convert to per second
        _lastValueChangeMillis = currentMillis;
        _lastRawValue = rawValue;
        _lastVelocity = velocity;  // Store for smoothing
    }
    
    // Use smoothed velocity (prevents jitter)
    velocity = _lastVelocity;
    
    // Determine if settled
    bool isSettled = (currentMillis - _lastValueChangeMillis) > SETTLING_DELAY;
    
    // Velocity-based rounding
    const float FAST_THRESHOLD = 5.0f;   // cm/s or in/s - fast movement
    const float SLOW_THRESHOLD = 2.0f;   // cm/s or in/s - slow movement
    
    float displayValue;
    if (isSettled) {
        displayValue = rawValue;  // Settled: show exact value
    } else if (velocity > FAST_THRESHOLD) {
        displayValue = round(rawValue);  // Fast: round to 1.0
    } else if (velocity > SLOW_THRESHOLD) {
        displayValue = round(rawValue * 2.0f) / 2.0f;  // Medium: round to 0.5
    } else {
        displayValue = round(rawValue * 5.0f) / 5.0f;  // Slow: round to 0.2
    }
    
    return displayValue;
}

// Helper 2: Render big number with unit label
void DisplaySys::renderBigNumber(float displayValue, const String& unitStr, bool justSettled) {
    // Redraw if: value changed OR unit changed OR just settled
    if (abs(displayValue - _lastBigValue) > 0.15f || unitStr != _lastBigUnit || justSettled) {
        // Auto-ranging: CM → M if > 999.9
        float effectiveValue = displayValue;
        String effectiveUnit = "CM";
        int precision =  1;
        
        if (unitStr == "CM" && effectiveValue >= 999.95f) {
            effectiveValue /= 100.0f;
            effectiveUnit = "M";
            precision = 1;  // Keep length = 4 chars
        } else if (unitStr == "IN") {
            effectiveUnit = "IN";
        }
        
        // Format number
        char buffer[10];
        dtostrf(effectiveValue, 0, precision, buffer);
        String numStr = String(buffer);
        int originalLength = numStr.length();
        
        // Smart clearing: Only clear when digit count changes
        static int lastNumLength = 0;
        if (originalLength != lastNumLength) {
            _lcd->setCursor(0, 0);
            _lcd->print("                    ");  // 20 spaces
            _lcd->setCursor(0, 1);
            _lcd->print("                    ");
            lastNumLength = originalLength;
        }
        
        // Draw big numbers at column 4
        _bigNumbers->setBigNumberCursor(4, 0);
        _bigNumbers->print(numStr.c_str());
        
        _lastBigValue = displayValue;
        _lastBigUnit = unitStr;
        
        // Unit label at row 1, column 18
        _lcd->setCursor(18, 1);
        _lcd->print(effectiveUnit == "M" ? " M" : "CM");
    } else {
        // Ensure unit remains visible even if value didn't change
        String currentUnit = (unitStr == "IN") ? "IN" : ((displayValue >= 999.95f) ? "M" : "CM");
        _lcd->setCursor(18, 1);
        _lcd->print(currentUnit == "M" ? " M" : "CM");
    }
}

// Helper 3: Render separator line
void DisplaySys::renderSeparatorLine() {
    String line2 = "====================";
    if (line2 != _lastLine[2]) {
        printLine(2, line2);
        _lastLine[2] = line2;
    }
}

// Helper 4: Render stock info line
void DisplaySys::renderInfoLine(uint8_t stockType, const char* stockStr, uint8_t cutMode, uint8_t faceVal) {
    String stockIcon = "";
    if (stockType == 0) stockIcon = "\xDB";       // Rect: █
    else if (stockType == 1) stockIcon = "L";     // Angle: L
    else if (stockType == 2) stockIcon = "D";     // Round: D
    
    String line3 = stockIcon + " " + String(stockStr);
    line3 += " ANG " + String(cutMode) + "\xDF";  // "ANG 45°"
    
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
    showIdle(mm, 0, 0, 0, "", 0, isInch, false);
}

void DisplaySys::showMenu(const char* title, String value, bool isEditMode) {
    // Mark that we're leaving idle mode (so custom chars get reloaded on return)
    if (_inIdleMode) {
        _inIdleMode = false;
        // Clear screen - necessary to prevent gibberish during char redefinition
        _lcd->clear();
        // Then reload menu-specific custom characters (icons)
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
        // Clear screen - necessary to prevent gibberish
        _lcd->clear();
        // Then reload menu-specific custom characters (icons)
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

void DisplaySys::resetIdleMode() {
    _inIdleMode = false;
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
    // 0: Statistics/Bar Chart (User Defined - Updated)
    // Used for Units in Settings and Statistics menus (via \x08 mapping)
    uint8_t iconStats[8] = {0x00, 0x10, 0x10, 0x14, 0x15, 0x15, 0x1F, 0x00};
    _lcd->createChar(0, iconStats);

    // 1: Rectangular Tube
    uint8_t iconRect[8] = {0x00, 0x1F, 0x11, 0x11, 0x11, 0x1F, 0x00, 0x00};
    _lcd->createChar(1, iconRect);
    
    // 2: Angle Iron (L)
    uint8_t iconAngle[8] = {0x00, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, 0x00};
    _lcd->createChar(2, iconAngle);
    
    // 3: Gear (Calibration Icon)
    uint8_t iconGear[8] = {0x00, 0x0E, 0x15, 0x15, 0x17, 0x11, 0x0E, 0x00};
    _lcd->createChar(3, iconGear);

    // 4: Blade (User Defined - Updated)
    // Used for Auto-Zero, Kerf Thickness, and AZ Threshold
    uint8_t iconBlade[8] = {0x00, 0x18, 0x1C, 0x0E, 0x06, 0x07, 0x03, 0x00};
    _lcd->createChar(4, iconBlade);

    // 5: Settings (Crosshair) - User Defined
    // Replaces Gear icon
    uint8_t iconSettings[8] = {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00, 0x00};
    _lcd->createChar(5, iconSettings);

    // 6: Direction (Double Arrow) - User Defined
    // Replaces Calibration icon
    uint8_t iconArrows[8] = {0x04, 0x0E, 0x1F, 0x00, 0x00, 0x1F, 0x0E, 0x04};
    _lcd->createChar(6, iconArrows);

    // 7: Angle (User Defined - Refined)
    uint8_t iconAngleSym[8] = {0x00, 0x10, 0x18, 0x1C, 0x1E, 0x1F, 0x00, 0x00};
    _lcd->createChar(7, iconAngleSym);
}

// ============================================================================
// GEMINI.md Rule 4.2: Backlight Power Management Methods
// ============================================================================

void DisplaySys::setBacklightTimeout(uint16_t seconds) {
    _timeoutSeconds = seconds;
    _lastActivityMillis = millis();  // Reset timer
}

void DisplaySys::wakeBacklight() {
    // Wake backlight if OFF
    if (!_backlightOn) {
        _lcd->backlight();
        _backlightOn = true;
    }
    // Reset activity timer
    _lastActivityMillis = millis();
}
