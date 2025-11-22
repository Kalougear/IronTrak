#include "headers/DisplaySys.h"

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
    // Must be done before Wire.begin()
    Wire.setSDA(PIN_LCD_SDA);
    Wire.setSCL(PIN_LCD_SCL);
    Wire.begin(); // Initialize I2C bus
#endif
    _lcd->init();
    _lcd->backlight();
    _lcd->clear();
}

void DisplaySys::update() {
    // Placeholder for periodic updates if needed
}

void DisplaySys::showIdle(float lengthMM, float targetMM, uint8_t cutMode, const char* stockStr, uint8_t faceVal, bool isInch) {
    // ==========================================
    // NEW PREMIUM LAYOUT
    // ==========================================
    // Row 0:      2143.5 mm       (Centered)
    // Row 1: -------------------- (Separator)
    // Row 2: Stock: 20x20         (Stock Info)
    // Row 3: Angle: 0 deg         (Cut Info)
    // ==========================================

    // --- Line 0: Measurement (Centered) ---
    String lenStr = String(lengthMM, 1);
    String unitStr = isInch ? "in" : "mm";
    String mainVal = lenStr + " " + unitStr;
    
    // Center the value
    int padding = (20 - mainVal.length()) / 2;
    if (padding < 0) padding = 0;
    
    String line0 = "";
    for(int i=0; i<padding; i++) line0 += " ";
    line0 += mainVal;
    // Fill rest with spaces
    while(line0.length() < 20) line0 += " ";
    
    // --- Line 1: Separator ---
    String line1 = "--------------------";
    
    // --- Line 2: Stock Info ---
    String line2 = "Stock: " + String(stockStr);
    // Pad to 20
    while(line2.length() < 20) line2 += " ";
    
    // --- Line 3: Angle / Mode ---
    String line3 = "Angle: ";
    if (cutMode == 0) line3 += "0 deg";
    else if (cutMode == 1) line3 += "45 deg";
    else line3 += "Custom";
    
    // Add Face info if relevant
    if (cutMode > 0 && faceVal > 0) {
        line3 = "F:" + String(faceVal) + " " + line3;
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
    _lcd->print(line0);
    
    // Line 1: Direction and Auto-Zero
    String line1 = "Dir:" + String(reverseDir ? "REV" : "NORM");
    line1 += " AZ:" + String(autoZeroEnabled ? "ON" : "OFF");
    _lcd->setCursor(0, 1);
    _lcd->print(line1);
    
    // Line 2: Instructions
    _lcd->setCursor(0, 2);
    _lcd->print("Hold to view...");
    
    _lcd->setCursor(0, 3);
    _lcd->print("Release to exit");
}

void DisplaySys::showMeasurement(float mm, bool isInch) {
    // Legacy method - can be removed or redirected to showIdle
    showIdle(mm, 0, 0, "", 0, isInch);
}

void DisplaySys::showMenu(const char* title, String value, bool isEditMode) {
    // Optimized to prevent flickering (no clear())
    
    // Line 0: Title
    String line0 = String(title);
    while(line0.length() < 20) line0 += " ";
    
    if (line0 != _lastLine[0]) {
        _lcd->setCursor(0, 0);
        _lcd->print(line0);
        _lastLine[0] = line0;
    }
    
    // Line 1: Value
    String line1 = "";
    if (isEditMode) line1 += "> ";
    line1 += value;
    while(line1.length() < 20) line1 += " ";
    
    if (line1 != _lastLine[1]) {
        _lcd->setCursor(0, 1);
        _lcd->print(line1);
        _lastLine[1] = line1;
    }
    
    // Clear lines 2 & 3 if they are not empty (from previous screen)
    // We assume lines 2/3 are unused in menu for now
    String blank = "                    ";
    if (_lastLine[2] != blank) {
        _lcd->setCursor(0, 2);
        _lcd->print(blank);
        _lastLine[2] = blank;
    }
    if (_lastLine[3] != blank) {
        _lcd->setCursor(0, 3);
        _lcd->print(blank);
        _lastLine[3] = blank;
    }
}

void DisplaySys::showError(const char* msg) {
    _lcd->clear();
    _lcd->setCursor(0, 0);
    _lcd->print("ERROR:");
    _lcd->setCursor(0, 1);
    _lcd->print(msg);
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
    _lcd->print(text);
}
