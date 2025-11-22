#ifndef USERINPUT_H
#define USERINPUT_H

#include <Arduino.h>
#include "Config.h"

enum InputEvent {
    EVENT_NONE,
    EVENT_CW,
    EVENT_CCW,
    EVENT_CLICK,
    EVENT_LONG_PRESS,       // 3s - Enter Menu
    EVENT_SUPER_LONG_PRESS  // 10s - Hidden Info
};

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
