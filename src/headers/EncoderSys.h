#ifndef ENCODERSYS_H
#define ENCODERSYS_H

#include <Arduino.h>
#include <Encoder.h>
#include "Config.h"

class EncoderSys {
public:
    EncoderSys();
    
    void init();
    void update(); // Call in main loop
    
    void reset();
    long getRawCount();
    float getDistanceMM();
    
    void setWheelDiameter(float diameterMM);
    float getWheelDiameter();
    void setOffset(float offsetMM);  // For Auto-Zero feature

private:
    Encoder* _encoder;
    long _lastCount;
    float _wheelDiameter;
    float _mmPerPulse;
    float _offsetMM;  // Position offset for Auto-Zero
    
    void recalculateCalibration();
};

#endif // ENCODERSYS_H
