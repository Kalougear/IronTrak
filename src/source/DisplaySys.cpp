#include "headers/DisplaySys.h"

DisplaySys::DisplaySys() {
    _lcd = new LiquidCrystal_I2C(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);
    _lastMM = -999.9;
    _lastIsInch = false;
    for (int i = 0; i < 4; i++) {
        _lastLine[i] = "";
    }
}

void DisplaySys::init() {
    _lcd->init();
    _lcd->backlight();
    _lcd->clear();
}

void DisplaySys::update() {
    // Placeholder for periodic updates if needed
}

void DisplaySys::showIdle(float lengthMM, float targetMM, uint8_t cutMode, const char* stockStr, uint8_t faceVal, bool isInch) {
    // Line 0: Length and Target
    String line0 = "Len:";
    if (isInch) {
        float lenInch = lengthMM / 25.4;
        line0 += String(lenInch, 2) + "\"";
    } else {
        line0 += String(lengthMM, 1);
    }
    
    // Add target if in angle mode
    if (cutMode > 0) {  // Not 0° mode
        line0 += "  Tgt:";
        if (isInch) {
            line0 += String(targetMM / 25.4, 2);
        } else {
            line0 += String(targetMM, 1);
        }
    }
    
    // Line 1: Empty or unit indicator
    String line1 = isInch ? "        (inches)" : "         (mm)";
    
    // Line 2: Mode and Stock
    String line2 = "[";
    if (cutMode == 0) {
        line2 += "0";
    } else if (cutMode == 1) {
        line2 += "45";
    } else {
        line2 += "??";
    }
    line2 += "] " + String(stockStr);
    
    // Line 3: Face selection (if applicable)
    String line3 = "";
    if (cutMode > 0 && faceVal > 0) {
        line3 = "Face: " + String(faceVal) + (isInch ? "\"" : "mm");
    }
    
    // Only update if changed (prevent flicker)
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
    // Clear screen for menu
    _lcd->clear();
    
    // Title on line 0
    _lcd->setCursor(0, 0);
    _lcd->print(title);
    
    // Value on line 1 (with edit indicator if applicable)
    _lcd->setCursor(0, 1);
    if (isEditMode) {
        _lcd->print("> ");
    }
    _lcd->print(value);
    
    // Cache cleared (menu is transient)
    for (int i = 0; i < 4; i++) {
        _lastLine[i] = "";
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
