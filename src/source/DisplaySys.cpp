#include "headers/DisplaySys.h"

// Helper to print string with custom char 0 support
static void printStr(LiquidCrystal_I2C* lcd, String s) {
    for (unsigned int i = 0; i < s.length(); i++) {
        char c = s[i];
        if (c == '\x08') lcd->write(0); // Map \x08 backspace char to Custom Char 0 (Gear)
        else lcd->write(c);
    }
}

DisplaySys::DisplaySys() {
    _lcd = new LiquidCrystal_I2C(LCD_ADDR, LCD_COLS, LCD_ROWS);
    _lastMM = -999.9;
    _lastIsInch = false;
    for (int i = 0; i < 4; i++) {
        _lastLine[i] = "";
    }
}

void DisplaySys::init() {
#if defined(STM32F4xx)
    // Explicitly set I2C pins for STM32 (PB8/PB9)
    Wire.setSDA(PIN_LCD_SDA);
    Wire.setSCL(PIN_LCD_SCL);
    Wire.begin(); 
#endif
    _lcd->init();
    _lcd->backlight();
    createCustomChars(); 
    _lcd->clear();
}

void DisplaySys::update() {
    // Placeholder
}

void DisplaySys::showIdle(float currentMM, float targetMM, uint8_t cutMode, uint8_t stockType, const char* stockStr, uint8_t faceVal, bool isInch) {
    // ==========================================
    // MEASUREMENT-FIRST LAYOUT
    // ==========================================
    // Row 0: ══════════════════ (Top frame)
    // Row 1:     52.3 CM        (HERO - centered)
    // Row 2: ══════════════════ (Bottom frame)
    // Row 3: ▯ 20x40  ⦨45° F:20 (Compact info)
    // ==========================================

    // --- Line 0: Top Separator ---
    String line0 = "====================";
    
    // --- Line 1: Measurement (THE STAR - Centered) ---
    String lenStr;
    String unitStr;
    
    if (isInch) {
        // Convert mm to inches
        lenStr = String(currentMM / 25.4, 3);
        unitStr = "IN";
    } else {
        // Convert mm to cm
        lenStr = String(currentMM / 10.0, 1);
        unitStr = "CM";
    }
    
    String mainVal = lenStr + " " + unitStr;
    
    // Center the value
    int padding = (20 - mainVal.length()) / 2;
    if (padding < 0) padding = 0;
    
    String line1 = "";
    for(int i=0; i<padding; i++) line1 += " ";
    line1 += mainVal;
    // Fill rest with spaces
    while(line1.length() < 20) line1 += " ";
    
    // --- Line 2: Bottom Separator ---
    String line2 = "====================";
    
    // --- Line 3: Compact Info (Stock + Angle) ---
    String icon = "";
    if (stockType == 0) icon = "\x01";      // Rect (custom char)
    else if (stockType == 1) icon = "\x02"; // Angle iron (custom char L-shape)
    else if (stockType == 2) icon = "\x03"; // Phi (custom char)
    
    String line3 = icon + " " + String(stockStr);
    
    // Add angle info with Angle Symbol (7)
    line3 += " \x07 " + String(cutMode) + "\xDF"; // angle symbol + space + degrees
    
    // Add Face info if relevant
    if (cutMode > 0 && faceVal > 0) {
        line3 += " F:" + String(faceVal);
    }
    
    // Pad to 20
    while(line3.length() < 20) line3 += " ";
    
    // --- Update LCD (Only if changed) ---
    if (line0 != _lastLine[0]) {
        printLine(0, line0);
        _lastLine[0] = line0;
    }
    if (line1 != _lastLine[1]) {
        printLine(1, line1);
        _lastLine[1] = line1;
    }
    if (line2 != _lastLine[2]) {
        printLine(2, line2);
        _lastLine[2] = line2;
    }
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
    uint8_t iconBlade[8] = {0x04, 0x0C, 0x1C, 0x0E, 0x07, 0x02, 0x00, 0x00};
    _lcd->createChar(4, iconBlade);

    // 5: Units (U) - User Defined
    // Replaces Stats icon
    uint8_t iconUnits[8] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00};
    _lcd->createChar(5, iconUnits);

    // 6: Direction (Up Arrow) - User Defined
    // Replaces Blade icon
    uint8_t iconDirection[8] = {0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x04, 0x00};
    _lcd->createChar(6, iconDirection);

    // 7: Angle Symbol (diagonal line, vertically centered)
    // Replaces Exit icon to fit within 7 char limit
    uint8_t iconAngleSymbol[8] = {0x00, 0x10, 0x18, 0x1C, 0x1E, 0x1F, 0x00, 0x00};
    _lcd->createChar(7, iconAngleSymbol);
    
    // NOTE: LCD only supports 8 custom chars (0-7). 
    // We avoid 0 because String(char(0)) can cause termination issues.
    // Indices 8+ wrap around and overwrite 0+, which caused the "A" bug.
}
