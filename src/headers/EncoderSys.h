#ifndef ENCODERSYS_H
#define ENCODERSYS_H

#include <Arduino.h>
#include "Config.h"

// STM32 Hardware Timer for Encoder
#if defined(STM32F4xx)
  #include <HardwareTimer.h>
#else
  #include <Encoder.h> // Fallback for AVR
#endif

class EncoderSys {
public:
    EncoderSys();
    
    void init();
    void update(); // Call in main loop to handle overflow tracking
    
    void reset();
    long getRawCount();
    float getDistanceMM();
    
    void setWheelDiameter(float diameterMM);
    float getWheelDiameter();
    void setOffset(float offsetMM);

private:
#if defined(STM32F4xx)
    HardwareTimer* _timer;
    volatile long _overflowCount;
    uint16_t _lastTimerCount;
#else
    Encoder* _encoder;
#endif

    long _lastCount;
    float _wheelDiameter;
    float _mmPerPulse;
    float _offsetMM;
    
    void recalculateCalibration();
};

#endif // ENCODERSYS_H
