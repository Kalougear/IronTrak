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

    // 2. Initialize Subsystems
    Serial1.println("Initializing Storage...");
    Storage::init();
    Serial1.println("Storage OK");
    
    Serial1.println("Loading settings...");
    settings = Storage::load();
    Serial1.println("Settings loaded OK");
    
    // RAW I2C TEST - Before any library init
    Serial1.println("=== RAW I2C TEST ===");
    
    // Enable internal pull-ups on I2C pins (4.7kΩ - 10kΩ typically needed)
    pinMode(PB9, INPUT_PULLUP);  // SDA
    pinMode(PB8, INPUT_PULLUP);  // SCL
    delay(10);
    
    Wire.setSDA(PB9);
    Wire.setSCL(PB8);
    Wire.begin();
    Wire.setClock(100000); // 100kHz (slow and reliable)
    delay(100);
    
    Serial1.println("I2C pins configured: PB9(SDA), PB8(SCL)");
    Serial1.println("Scanning I2C (raw)...");
    byte found = 0;
    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial1.print("  -> 0x");
            if (addr < 16) Serial1.print("0");
            Serial1.println(addr, HEX);
            found++;
        }
    }
    Serial1.print("Found ");
    Serial1.print(found);
    Serial1.println(" device(s)");
    Serial1.println("===================");
    
    Serial1.println("Initializing Display (20x4 LCD I2C)...");
    displaySys.init();
    Serial1.println("Display OK");
    
    // I2C Scanner - find what address the LCD is actually at
    Serial1.println("Scanning I2C bus...");
    // Wire was already initialized in displaySys.init(), but let's be sure
    Wire.begin();
    delay(100);
    
    byte deviceCount = 0;
    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        byte error = Wire.endTransmission();
        if (error == 0) {
            Serial1.print("Found I2C device at 0x");
            if (addr < 16) Serial1.print("0");
            Serial1.println(addr, HEX);
            deviceCount++;
        }
    }
    if (deviceCount == 0) {
        Serial1.println("WARNING: NO I2C devices found! Check wiring!");
    } else {
        Serial1.print("I2C scan complete - found ");
        Serial1.print(deviceCount);
        Serial1.println(" device(s)");
    }
    
    // Test: Write to LCD via displaySys
    Serial1.println("Testing LCD write at 0x27...");
    displaySys.showError("** BOOT TEST **");
    Serial1.println("Message sent to LCD");
    delay(3000);  // Hold for 3s
    displaySys.clear();
    
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
    menuSys.init(&settings, &statsSys);
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
    Serial1.println("Watchdog DISABLED for debugging...");
    // TEMPORARILY DISABLED - causing resets in loop
    //#if defined(STM32F4xx)
    //    IWatchdog.begin(2000000);
    //    Serial1.println("Watchdog OK");
    //#else
    //    wdt_enable(WDTO_2S);
    //#endif
    
    Serial1.println("========================================");
    Serial1.println("   SYSTEM READY - ENTERING MAIN LOOP");
    Serial1.println("========================================");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
#if defined(STM32F4xx)
    // IWatchdog.reload();  // DISABLED
#else
    // wdt_reset();  // DISABLED
#endif

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
                    Serial1.println("Mode toggled via double-click");
                    lastClickTime = 0;  // Reset
                } else {
                    // Single click: Register Cut + Zero
                    statsSys.registerCut(currentMM);
                    encoderSys.reset();
                    Serial1.print("Cut registered: ");
                    Serial1.print(currentMM);
                    Serial1.println("mm, encoder reset");
                    lastClickTime = now;
                    
                    // Reset Auto-Zero state
                    azState = AZ_DISABLED;
                }
            } else if (event == EVENT_LONG_PRESS) {
                // Enter Menu
                currentState = STATE_MENU;
                displaySys.clear();
                menuSys.init(&settings, &statsSys);
                Serial1.println("Entering menu...");
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
                        // Triggered!
                        statsSys.registerCut(lockedPosition);
                        encoderSys.reset();
                        float offset = currentMM - lockedPosition;
                        encoderSys.setOffset(offset);
                        
                        Serial1.println("Auto-Zero TRIGGERED!");
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
                Serial1.println("Exiting menu...");
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
