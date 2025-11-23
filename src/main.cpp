#include <Arduino.h>
#if defined(STM32F4xx)
  #include <IWatchdog.h>
  #include <HardwareTimer.h>
#else
  #include <avr/wdt.h>
#endif

#include "headers/Config.h"
#include "headers/StateMachine.h"
#include "headers/EncoderSys.h"
#include "headers/DisplaySys.h"
#include "headers/UserInput.h"
#include "headers/Storage.h"
#include "headers/MenuSys.h"
#include "headers/StatsSys.h"
#include "headers/AngleSensor.h" // Added

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================
EncoderSys encoderSys;
DisplaySys displaySys;
UserInput userInput;
MenuSys menuSys;
StatsSys statsSys;
AngleSensor angleSensor; // Added
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

#if defined(STM32F4xx)
// STM32 Timer Callback
void Timer1_Callback() {
    userInput.isrTick();
}
HardwareTimer *tickTimer = nullptr;
#else
// AVR Timer1 ISR
ISR(TIMER1_COMPA_vect) {
    userInput.isrTick();
}
#endif

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
    // ===== ABSOLUTE FIRST - Serial Debug BEFORE anything else =====
    Serial1.setRx(PA10);
    Serial1.setTx(PA9);
    Serial1.begin(115200);
    Serial1.println("\n\n*** BOOT START ***");
    
    // LED & Boot sequence
    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH);  // LED off initially
    Serial1.println("LED configured");
    
    // Blink 3 times on boot
    for (int i = 0; i < 3; i++) {
        digitalWrite(PC13, LOW);
        delay(200);
        digitalWrite(PC13, HIGH);
        delay(200);
    }
    Serial1.println("Boot blink complete");
    
    Serial1.println("========================================");
    Serial1.println("  IronTrak STM32F4 Boot Sequence");
    Serial1.println("========================================");
    
    // USB CDC
    Serial.begin(SERIAL_BAUD_RATE);
    delay(100);
    Serial1.println("USB CDC initialized");

    // 2. Initialize Subsystems (ORDER MATTERS FOR I2C)
    
    // Display starts Wire.begin(), so it must be first!
    Serial1.println("Initializing Display (20x4 LCD I2C)...");
    displaySys.init();
    Serial1.println("Display OK (I2C Started)");

    Serial1.println("Settings initialized with defaults (no persistence)");
    
    Serial1.println("Initializing Angle Sensor...");
    if (settings.useAngleSensor) {
        if (angleSensor.init()) {
            Serial1.println("Angle Sensor FOUND");
        } else {
            Serial1.println("Angle Sensor NOT FOUND - Reverting to Manual");
            settings.useAngleSensor = false;
        }
    } else {
        Serial1.println("Angle Sensor DISABLED (Manual Mode)");
    }
    
    Serial1.println("Initializing Encoder (TIM4 Hardware)...");
    encoderSys.init();
    Serial1.println("Encoder init OK");
    
    encoderSys.setWheelDiameter(settings.wheelDiameter);
    Serial1.println("Encoder calibrated");
    
    Serial1.println("Initializing User Input (KY-040)...");
    userInput.init();
    Serial1.println("UserInput OK");
    
    Serial1.println("Initializing Stats System...");
    statsSys.init(&settings);
    Serial1.println("Stats OK");
    
    Serial1.println("Initializing Menu System...");
    menuSys.init(&settings, &statsSys, &angleSensor); // Pass sensor
    Serial1.println("Menu OK");

    // 3. Configure Timer for 1kHz Interrupt
    Serial1.println("Configuring TIM3 for 1kHz tick...");
#if defined(STM32F4xx)
    tickTimer = new HardwareTimer(TIM3);
    tickTimer->setOverflow(1000, HERTZ_FORMAT);
    tickTimer->attachInterrupt(Timer1_Callback);
    tickTimer->resume();
    Serial1.println("TIM3 OK");
#else
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
    OCR1A = 249;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS11) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);
    interrupts();
#endif

    // 4. Enable Watchdog Timer (2 Seconds)
    Serial1.println("Enabling Watchdog (2s timeout)...");
#if defined(STM32F4xx)
    IWatchdog.begin(2000000);
    Serial1.println("Watchdog OK");
#else
    wdt_enable(WDTO_2S);
#endif
    
    Serial1.println("========================================");
    Serial1.println("   SYSTEM READY - ENTERING MAIN LOOP");
    Serial1.println("========================================");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
#if defined(STM32F4xx)
    IWatchdog.reload();
#else
    wdt_reset();
#endif

    // 1. Handle User Input
    InputEvent event = userInput.getEvent();
    
    // 2. State Machine
    switch (currentState) {
        case STATE_IDLE: {
            // Angle Sensor Logic
            if (settings.useAngleSensor) {
                float deg = angleSensor.getAngleDegrees();
                // Round to nearest int for cutMode (0-90)
                uint8_t newMode = (uint8_t)(deg + 0.5);
                if (newMode != settings.cutMode) {
                    settings.cutMode = newMode;
                    // No save here to avoid EEPROM wear, save on mode change or periodic?
                    // Actually, cutMode is volatile in sensor mode, we don't save it constantly.
                }
            }

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
                    // Double-click! Toggle mode (Straight <-> Angle)
                    
                    if (settings.cutMode == 0) {
                        // Switch TO Angle Mode (restore last used angle)
                        settings.cutMode = settings.lastAngle; // Use persistent value
                        if (settings.cutMode == 0) settings.cutMode = 45; // Safety fallback
                    } else {
                        // Switch TO Straight Mode (save current angle first)
                        settings.lastAngle = settings.cutMode; // Save persistent value
                        settings.cutMode = 0;
                        // No persistence - settings live in RAM only
                    }
                    
                    Serial1.print("Mode toggled. New Angle: ");
                    Serial1.println(settings.cutMode);
                    lastClickTime = 0;  // Reset
                } else {
                    // Single click: Register Cut + Zero
                    statsSys.registerCut(currentMM);
                    encoderSys.reset();
                    
                    // Miter Offset Logic
                    if (settings.cutMode > 0) {
                        float faceVal = (float)getFaceValue();
                        if (faceVal > 0) {
                            float rad = settings.cutMode * PI / 180.0;
                            float offset = faceVal * tan(rad);
                            encoderSys.setOffset(offset);
                        }
                    }
                    
                    Serial1.print("Cut registered: ");
                    Serial1.print(currentMM);
                    Serial1.println("mm, encoder reset");
                    lastClickTime = now;
                    
                    // Reset Auto-Zero state
                    azState = AZ_DISABLED;
                }
            } else if (event == EVENT_LONG_PRESS) {
                // Enter Menu - Overwrite idle screen directly for instant transition
                menuSys.init(&settings, &statsSys, &angleSensor);
                currentState = STATE_MENU;
                Serial1.println("Entering menu...");
            } else if (event == EVENT_CW || event == EVENT_CCW) {
                // Knob in Idle - only in angle mode with rectangular stock AND MANUAL MODE
                if (!settings.useAngleSensor && settings.cutMode > 0 && settings.stockType == 0) {
                    settings.faceIdx = (settings.faceIdx == 0) ? 1 : 0;
                    // Storage::save(settings); // Disabled - Will use AT24C256 EEPROM
                }
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
                            Serial1.println("Auto-Zero ARMED");
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
                        // Triggered! Register cut and shift zero point
                        statsSys.registerCut(lockedPosition);
                        
                        long rawCount = encoderSys.getRawCount();
                        float mmPerPulse = encoderSys.getWheelDiameter() * PI / PULSES_PER_REV;
                        float absolutePosition = rawCount * mmPerPulse;  // Raw physical position
                        float currentOffset = absolutePosition - currentMM;  // Back-calculate current offset
                        float newOffset = currentOffset + lockedPosition;   // Shift by locked amount
                        encoderSys.setOffset(newOffset);
                        
                        Serial1.print("Auto-Zero TRIGGERED! Locked: ");
                        Serial1.print(lockedPosition);
                        Serial1.println("mm");
                        
                        azState = AZ_MEASURING;
                        stillnessStartTime = millis();
                    }
                }
            }
            
            // Update display (if not in hidden menu)
            if (!hiddenMenuActive) {
                // Freeze display at locked position when Auto-Zero is armed (prevents jiggle confusion)
                float displayMM = (azState == AZ_ARMED) ? lockedPosition : currentMM;
                displaySys.showIdle(displayMM, targetMM, settings.cutMode, settings.stockType, stockStr, faceVal, settings.isInch);
            }
            break;
        }

        case STATE_MENU: {
            // Translate raw encoder events to semantic UI events
            InputEvent semanticEvent = toSemanticEvent(event);
            
            if (!menuSys.update(semanticEvent, &displaySys, &encoderSys)) {
                // Menu requested exit - Overwrite with idle screen directly
                currentState = STATE_IDLE;
                Serial1.println("Exiting menu...");
                // Apply new settings
                encoderSys.setWheelDiameter(settings.wheelDiameter);
                // Reset Auto-Zero
                azState = AZ_DISABLED;
            }
            break;
        }

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
    statsSys.update();
}


