#include "headers/UserInput.h"

UserInput::UserInput() {
    _lastEncoded = 0;
    _encoderValue = 0;
    _btnState = 0xFFFF; // Assume high (pullup)
    _pendingEvent = EVENT_NONE;
    _longPressHandled = false;
    _superLongPressHandled = false;
}

void UserInput::init() {
    pinMode(PIN_MENU_CLK, INPUT_PULLUP);
    pinMode(PIN_MENU_DT, INPUT_PULLUP);
    pinMode(PIN_MENU_SW, INPUT_PULLUP);
    
    // Read initial state
    int MSB = digitalRead(PIN_MENU_DT);
    int LSB = digitalRead(PIN_MENU_CLK);
    _lastEncoded = (MSB << 1) | LSB;
}

// This function is called at 1kHz by Timer1 ISR
void UserInput::isrTick() {
    handleEncoder();
    handleButton();
}

InputEvent UserInput::getEvent() {
    InputEvent e = _pendingEvent;
    _pendingEvent = EVENT_NONE; // Clear event after reading
    return e;
}

void UserInput::handleEncoder() {
    int MSB = digitalRead(PIN_MENU_DT);
    int LSB = digitalRead(PIN_MENU_CLK);
    int encoded = (MSB << 1) | LSB;
    int sum = (_lastEncoded << 2) | encoded;

    // State transition table for rotary encoder
    // This handles the 4 steps of a quadrature encoder
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) _encoderValue++;
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) _encoderValue--;

    _lastEncoded = encoded; // Store this value for next time
    
    // KY-040 often has 2 or 4 steps per detent. 
    // We usually want 1 event per detent.
    // Let's assume 4 steps per detent for standard encoders, or 2.
    // Adjust divisor based on feel.
    if (_encoderValue >= 4) {
        _pendingEvent = EVENT_CW;
        _encoderValue = 0;
    } else if (_encoderValue <= -4) {
        _pendingEvent = EVENT_CCW;
        _encoderValue = 0;
    }
}

void UserInput::handleButton() {
    // Shift register debounce
    // Read pin (active low)
    bool pinVal = digitalRead(PIN_MENU_SW);
    
    // Shift left and insert new value at bit 0
    _btnState = (_btnState << 1) | pinVal | 0xE000; // Keep top bits high to prevent false positives on overflow? No, just mask.
    // Actually, standard debounce:
    // 0x0000 = Stable LOW (Pressed)
    // 0xFFFF = Stable HIGH (Released)
    
    // We mask to 16 bits.
    
    if (_btnState == 0xF000) { // Transition to Pressed (detected after 12 stable 1s then 4 0s? No.)
        // Simple check: if we have 16 samples of LOW, it's pressed.
        // But we want edge detection.
    }
    
    // Let's use a simpler counter approach for 1kHz tick
    // 20ms debounce
    static uint8_t stableCount = 0;
    static bool lastStableState = HIGH;
    
    if (pinVal == lastStableState) {
        stableCount = 0;
    } else {
        stableCount++;
    }
    
    if (stableCount >= 20) { // 20ms stable
        if (lastStableState == HIGH && pinVal == LOW) {
            // Falling Edge (Press)
            _btnPressTime = millis();
            _longPressHandled = false;
        } else if (lastStableState == LOW && pinVal == HIGH) {
            // Rising Edge (Release)
            if (!_longPressHandled) {
                _pendingEvent = EVENT_CLICK;
            }
        }
        lastStableState = pinVal;
        stableCount = 0;
    }
    
    // Long press check (3s for menu, 10s for hidden info)
    if (lastStableState == LOW) {
        unsigned long pressedDuration = millis() - _btnPressTime;
        
        if (!_superLongPressHandled && pressedDuration > 10000) {  // 10 seconds
            _pendingEvent = EVENT_SUPER_LONG_PRESS;
            _superLongPressHandled = true;
            _longPressHandled = true;  // Also mark long press as handled
        } else if (!_longPressHandled && pressedDuration > 3000) {  // 3 seconds
            _pendingEvent = EVENT_LONG_PRESS;
            _longPressHandled = true;
        }
    }
}
