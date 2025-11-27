#ifndef DISPLAYSYS_H
#define DISPLAYSYS_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Config.h"

// Forward declaration for LCDBigNumbers
class LCDBigNumbers;

class DisplaySys {
public:
    DisplaySys();
    void init();
    void update(); // Call in main loop
    
    void showMeasurement(float mm, bool isInch);
    void showIdle(float currentMM, float targetMM, uint8_t cutMode, uint8_t stockType, const char* stockStr, uint8_t faceVal, bool isInch, bool reverseDir);
    void showHiddenInfo(float kerfMM, float diameter, bool reverseDir, bool autoZeroEnabled);
    void showMenu(const char* title, String value, bool isEditMode);
    void showMenu4(String l0, String l1, String l2, String l3);
    void showError(const char* msg);
    
    // Clears the screen and resets the display cache to force a full redraw
    void clear();

private:
    LiquidCrystal_I2C* _lcd;
    LCDBigNumbers* _bigNumbers;
    
    // Cache to prevent flickering
    float _lastMM;
    bool _lastIsInch;
    String _lastLine[4];  // Cache all 4 lines for 20x4 LCD
    
    // Big number cache
    float _lastBigValue;
    String _lastBigUnit;
    
    // Temporal filtering for smooth updates
    unsigned long _lastValueChangeMillis;
    float _lastRawValue;
    bool _wasSettled;  // Track if we just transitioned to settled state
    float _lastVelocity;  // Track rate of change (cm/s or in/s)
    
    // Display mode tracking for proper custom char management
    bool _inIdleMode;  // Track if we're displaying idle screen
    
    
    // GEMINI.md Rule 4.2: Helper functions to keep showIdle() < 60 lines
    float applyVelocityFiltering(float rawValue, bool isInch, unsigned long currentMillis);
    void renderBigNumber(float displayValue, const String& unitStr, bool justSettled);
    void renderSeparatorLine();
    void renderInfoLine(uint8_t stockType, const char* stockStr, uint8_t cutMode, uint8_t faceVal);
    
    void printLine(int row, String text);
    void createCustomChars();

};

#endif // DISPLAYSYS_H
