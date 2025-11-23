#ifndef USERINPUT_H
#define USERINPUT_H

#include <Arduino.h>
#include "Config.h"

enum InputEvent {
    EVENT_NONE,
    EVENT_CW,               // Raw hardware: Clockwise rotation
    EVENT_CCW,              // Raw hardware: Counter-clockwise rotation
    EVENT_CLICK,
    EVENT_LONG_PRESS,       // 1s - Enter Menu / Go Back
    EVENT_SUPER_LONG_PRESS, // 10s - Hidden Info
    // Semantic UI events (use these in menu handlers for consistency)
    EVENT_NEXT,             // UI: Move forward/down in list, increase value
    EVENT_PREV              // UI: Move backward/up in list, decrease value
};

// Translate raw encoder events to semantic UI events
// Current convention: CCW = Next/Increase, CW = Prev/Decrease
inline InputEvent toSemanticEvent(InputEvent raw) {
    if (raw == EVENT_CCW) return EVENT_NEXT;
    if (raw == EVENT_CW) return EVENT_PREV;
    return raw;  // CLICK, LONG_PRESS, etc. pass through unchanged
}

class UserInput {
public:
    UserInput();
    void init();
    
    // Called from Timer1 ISR (1kHz)
    void isrTick();
    
    // Called from Main Loop to get latest event
    InputEvent getEvent();

private:
    // Rotary Encoder State
    volatile uint8_t _lastEncoded;
    volatile int _encoderValue;
    
    // Button Debounce State
    volatile uint16_t _btnState; // Shift register for debounce
    volatile bool _btnPressed;
    volatile unsigned long _btnPressTime;
    volatile bool _longPressHandled;
    volatile bool _superLongPressHandled;
    
    // Event Buffer (Simple 1-item buffer for now)
    volatile InputEvent _pendingEvent;
    
    void handleEncoder();
    void handleButton();
};

#endif // USERINPUT_H
