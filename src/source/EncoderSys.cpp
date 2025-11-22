```cpp
#include "headers/EncoderSys.h"

EncoderSys::EncoderSys() {
    _wheelDiameter = DEFAULT_WHEEL_DIA_MM;
    _offsetMM = 0.0f;
    recalculateCalibration();
    
#if defined(STM32F4xx)
    _timer = nullptr;
    _overflowCount = 0;
    _lastTimerCount = 0;
#else
    _encoder = nullptr;
#endif
}

void EncoderSys::init() {
#if defined(STM32F4xx)
    // STM32 Hardware Timer Implementation
    // Uses TIM4 (PB6/PB7) in Encoder Mode
    _timer = new HardwareTimer(TIM4);
    
    // Configure channels for Encoder Mode (TI1 and TI2)
    _timer->setMode(1, TIMER_INPUT_ENCODER_MODE12, PIN_ENCODER_A);
    _timer->setMode(2, TIMER_INPUT_ENCODER_MODE12, PIN_ENCODER_B);
    
    // Set max count (16-bit timer)
    _timer->setOverflow(65535); 
    
    // Start the timer
    _timer->resume();
    
    // Initialize state
    _timer->setCount(0);
    _lastTimerCount = 0;
    _overflowCount = 0;
#else
    // AVR Software Interrupt Implementation
    _encoder = new Encoder(PIN_ENCODER_A, PIN_ENCODER_B);
#endif
    
    reset();
}

void EncoderSys::update() {
#if defined(STM32F4xx)
    // Handle 16-bit Timer Overflow/Underflow
    // This must be called frequently (at least once every few seconds)
    if (_timer == nullptr) return;
    
    uint16_t currentTimerCount = _timer->getCount();
    int32_t diff = currentTimerCount - _lastTimerCount;
    
    // Detect wrap-around
    // If diff is very negative (e.g. 0 - 65535 = -65535), we wrapped forward
    // If diff is very positive (e.g. 65535 - 0 = 65535), we wrapped backward
    // Threshold is half range (32768)
    
    if (diff < -32768) {
        _overflowCount++;
    } else if (diff > 32768) {
        _overflowCount--;
    }
    
    _lastTimerCount = currentTimerCount;
#endif
}

void EncoderSys::reset() {
#if defined(STM32F4xx)
    if (_timer != nullptr) {
        _timer->setCount(0);
        _lastTimerCount = 0;
        _overflowCount = 0;
    }
#else
    if (_encoder != nullptr) {
        _encoder->write(0);
    }
#endif
    _lastCount = 0;
    _offsetMM = 0.0f;
}

long EncoderSys::getRawCount() {
#if defined(STM32F4xx)
    if (_timer == nullptr) return 0;
    
    // Update overflow tracking first to ensure consistency
    update();
    
    // Calculate total count: (Overflows * 65536) + CurrentCount
    return (_overflowCount * 65536) + _timer->getCount();
#else
    if (_encoder == nullptr) return 0;
    return _encoder->read();
#endif
}

float EncoderSys::getDistanceMM() {
    long raw = getRawCount();
    return (raw * _mmPerPulse) - _offsetMM;
}

void EncoderSys::setWheelDiameter(float diameterMM) {
    _wheelDiameter = diameterMM;
    recalculateCalibration();
}

float EncoderSys::getWheelDiameter() {
    return _wheelDiameter;
}

void EncoderSys::setOffset(float offsetMM) {
    _offsetMM = offsetMM;
}

void EncoderSys::recalculateCalibration() {
    float circumference = _wheelDiameter * PI;
    _mmPerPulse = circumference / PULSES_PER_REV;
}
```
