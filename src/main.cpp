#include <Arduino.h>
#include <avr/wdt.h>
#include "headers/Config.h"
#include "headers/StateMachine.h"
#include "headers/EncoderSys.h"
#include "headers/DisplaySys.h"
#include "headers/UserInput.h"
#include "headers/Storage.h"
#include "headers/MenuSys.h"
#include "headers/StatsSys.h"

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================
EncoderSys encoderSys;
DisplaySys displaySys;
UserInput userInput;
MenuSys menuSys;
StatsSys statsSys;
SystemSettings settings;

SystemState currentState = STATE_IDLE;

// Auto-Zero State Machine
enum AutoZeroState { AZ_DISABLED, AZ_MEASURING, AZ_ARMED };
AutoZeroState azState = AZ_DISABLED;
float lockedPosition = 0.0;
unsigned long stillnessStartTime = 0;

// Double-click detection
unsigned long lastClickTime = 0;
const unsigned long DOUBLE_CLICK_WINDOW = 500;  // 500ms

// Hidden menu state
bool hiddenMenuActive = false;

// ============================================================================
// INTERRUPT SERVICE ROUTINES
// ============================================================================

// Timer1 Compare Match A ISR - Runs at 1kHz
ISR(TIMER1_COMPA_vect) {
    userInput.isrTick();
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

float getTargetMM() {
    if (settings.cutMode == 0) return 0;  // 0° mode, no target
    
    // Get selected face dimension
    if (settings.stockType == 0) {  // Rectangular
        if (settings.isInch) {
            return STOCK_RECT_IN_DIMS[settings.stockIdx][settings.faceIdx];
        } else {
            return STOCK_RECT_MM_DIMS[settings.stockIdx][settings.faceIdx];
        }
    } else if (settings.stockType == 1) {  // Angle iron
        if (settings.isInch) {
            return STOCK_ANGLE_IN_DIMS[settings.stockIdx][0];
        } else {
            return STOCK_ANGLE_MM_DIMS[settings.stockIdx][0];
        }
    } else {  // Cylinder
        if (settings.isInch) {
            return STOCK_CYL_IN_DIMS[settings.stockIdx];
        } else {
            return STOCK_CYL_MM_DIMS[settings.stockIdx];
        }
    }
}

const char* getStockString() {
    if (settings.isInch) {
        if (settings.stockType == 0) return STOCK_RECT_IN[settings.stockIdx];
        else if (settings.stockType == 1) return STOCK_ANGLE_IN[settings.stockIdx];
        else return STOCK_CYL_IN[settings.stockIdx];
    } else {
        if (settings.stockType == 0) return STOCK_RECT_MM[settings.stockIdx];
        else if (settings.stockType == 1) return STOCK_ANGLE_MM[settings.stockIdx];
        else return STOCK_CYL_MM[settings.stockIdx];
    }
}

uint8_t getFaceValue() {
    if (settings.stockType != 0) return 0;  // Only rectangular has face selection
    
    if (settings.isInch) {
        return STOCK_RECT_IN_DIMS[settings.stockIdx][settings.faceIdx];
    } else {
        return STOCK_RECT_MM_DIMS[settings.stockIdx][settings.faceIdx];
    }
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
    // 1. Initialize Serial
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println(F("IronTrak Phase 2 Booting..."));

    // 2. Initialize Subsystems
    Storage::init();
    settings = Storage::load();
    
    displaySys.init();
    encoderSys.init();
    encoderSys.setWheelDiameter(settings.wheelDiameter);
    
    userInput.init();
    statsSys.init(&settings);
    menuSys.init(&settings, &statsSys);

    // 3. Configure Timer1 for 1kHz Interrupt
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
    
    OCR1A = 249;            // Compare match register 16MHz/64/1000Hz
    TCCR1B |= (1 << WGM12); // CTC mode
    TCCR1B |= (1 << CS11) | (1 << CS10); // 64 prescaler
    TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt
    interrupts();

    // 4. Enable Watchdog Timer (2 Seconds)
    wdt_enable(WDTO_2S);
    
    Serial.println(F("System Ready. Phase 2 Active."));
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
    wdt_reset(); // Pet the dog

    // 1. Handle User Input
    InputEvent event = userInput.getEvent();
    
    // 2. State Machine
    switch (currentState) {
        case STATE_IDLE: {
            // Get current measurement
            float currentMM = encoderSys.getDistanceMM();
            float targetMM = getTargetMM();
            const char* stockStr = getStockString();
            uint8_t faceVal = getFaceValue();
            
            // Handle events
            if (event == EVENT_SUPER_LONG_PRESS) {
                // Hidden Menu - show while held
                displaySys.showHiddenInfo(settings.kerfMM, settings.wheelDiameter, 
                                         settings.reverseDirection, settings.autoZeroEnabled);
                hiddenMenuActive = true;
            } else if (hiddenMenuActive && event == EVENT_NONE) {
                // Button released, exit hidden menu
                hiddenMenuActive = false;
                displaySys.clear();
            } else if (event == EVENT_CLICK) {
                // Double-click detection
                unsigned long now = millis();
                if (now - lastClickTime < DOUBLE_CLICK_WINDOW) {
                    // Double-click! Toggle mode
                    settings.cutMode = (settings.cutMode == 0) ? 1 : 0;
                    Storage::save(settings);
                    lastClickTime = 0;  // Reset
                } else {
                    // Single click: Register Cut + Zero
                    statsSys.registerCut(currentMM);
                    encoderSys.reset();
                    lastClickTime = now;
                    
                    // Reset Auto-Zero state
                    azState = AZ_DISABLED;
                }
            } else if (event == EVENT_LONG_PRESS) {
                // Enter Menu
                currentState = STATE_MENU;
                displaySys.clear();
                menuSys.init(&settings, &statsSys);
            } else if (event == EVENT_CW || event == EVENT_CCW) {
                // Knob in Idle
                if (settings.cutMode > 0 && settings.stockType == 0) {
                    // Only in angle mode with rectangular stock: cycle face
                    settings.faceIdx = (settings.faceIdx == 0) ? 1 : 0;
                    Storage::save(settings);
                }
                // In 0° mode, knob does nothing (safety)
            }
            
            // Auto-Zero Logic (only in 0° mode)
            if (settings.autoZeroEnabled && settings.cutMode == 0 && !hiddenMenuActive) {
                if (azState == AZ_DISABLED || azState == AZ_MEASURING) {
                    // Check for stillness
                    if (abs(currentMM - lockedPosition) < 2.0) {
                        // Stock stable
                        if (millis() - stillnessStartTime > 5000) {
                            // Lock position after 5s
                            lockedPosition = currentMM;
                            azState = AZ_ARMED;
                        }
                    } else {
                        // Stock moving
                        stillnessStartTime = millis();
                        lockedPosition = currentMM;
                        azState = AZ_MEASURING;
                    }
                } else if (azState == AZ_ARMED) {
                    // Wait for movement threshold
                    if (abs(currentMM - lockedPosition) > settings.autoZeroThresholdMM) {
                        // Triggered!
                        statsSys.registerCut(lockedPosition);
                        encoderSys.reset();
                        float offset = currentMM - lockedPosition;
                        encoderSys.setOffset(offset);
                        
                        azState = AZ_MEASURING;
                        stillnessStartTime = millis();
                    }
                }
            }
            
            // Update display (if not in hidden menu)
            if (!hiddenMenuActive) {
                displaySys.showIdle(currentMM, targetMM, settings.cutMode, stockStr, faceVal, settings.isInch);
            }
            break;
        }

        case STATE_MENU:
            if (!menuSys.update(event, &displaySys, &encoderSys)) {
                // Menu requested exit
                currentState = STATE_IDLE;
                displaySys.clear();
                // Apply new settings
                encoderSys.setWheelDiameter(settings.wheelDiameter);
                // Reset Auto-Zero
                azState = AZ_DISABLED;
            }
            break;

        case STATE_MEASURING:
            // Legacy state, redirect to IDLE
            currentState = STATE_IDLE;
            break;

        case STATE_CALIBRATION:
            // Handled in MenuSys
            break;
            
        case STATE_ERROR:
            displaySys.showError("System Halted");
            break;
    }

    // 3. Update Hardware Wrappers
    encoderSys.update();
    displaySys.update();
}
