#ifndef DISPLAYSYS_H
#define DISPLAYSYS_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Config.h"

class DisplaySys {
public:
    DisplaySys();
    void init();
    void update(); // Call in main loop
    
    void showMeasurement(float mm, bool isInch);
    void showIdle(float lengthMM, float targetMM, uint8_t cutMode, const char* stockStr, uint8_t faceVal, bool isInch);
    void showHiddenInfo(float kerfMM, float diameter, bool reverseDir, bool autoZeroEnabled);
    void showMenu(const char* title, String value, bool isEditMode);
    void showError(const char* msg);
    void clear();

private:
    LiquidCrystal_I2C* _lcd;
    
    // Cache to prevent flickering
    float _lastMM;
    bool _lastIsInch;
    String _lastLine[4];  // Cache all 4 lines for 20x4 LCD
    
    void printLine(int row, String text);
};

#endif // DISPLAYSYS_H
