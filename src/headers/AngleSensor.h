#ifndef ANGLESENSOR_H
#define ANGLESENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "Config.h"
#include "Storage.h"

class AngleSensor {
public:
    AngleSensor();
    
    bool init();
    bool isConnected();
    
    // Main function to get current angle in degrees
    // Returns negative value if error or not connected
    float getAngleDegrees();
    
    // Raw access for calibration
    uint16_t getRawAngle();
    
    // Calibration helpers
    void setZeroPoint(uint16_t rawVal);
    void set45Point(uint16_t rawVal);

private:
    uint16_t readRegister12(uint8_t reg);
    float _lastDegrees;
    
    // Calibration data (cached from settings)
    uint16_t _rawZero;
    uint16_t _raw45;
};

#endif // ANGLESENSOR_H
