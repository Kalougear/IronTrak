#include "headers/AngleSensor.h"

#define AS5600_ADDR 0x36
#define REG_RAW_ANGLE 0x0C
#define REG_STATUS 0x0B

AngleSensor::AngleSensor() {
    _lastDegrees = 0.0;
    _rawZero = 0;
    _raw45 = 512;
}

bool AngleSensor::init() {
#ifdef USE_ANGLE_SENSOR
    Wire.beginTransmission(AS5600_ADDR);
    if (Wire.endTransmission() == 0) {
        // Sensor found - using default calibration values
        return true;
    }
#endif
    return false;
}

bool AngleSensor::isConnected() {
#ifdef USE_ANGLE_SENSOR
    Wire.beginTransmission(AS5600_ADDR);
    return (Wire.endTransmission() == 0);
#else
    return false;
#endif
}

uint16_t AngleSensor::readRegister12(uint8_t reg) {
    Wire.beginTransmission(AS5600_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom(AS5600_ADDR, 2);
    if (Wire.available() >= 2) {
        uint8_t high = Wire.read();
        uint8_t low = Wire.read();
        return (high << 8) | low;
    }
    return 0;
}

uint16_t AngleSensor::getRawAngle() {
#ifdef USE_ANGLE_SENSOR
    return readRegister12(REG_RAW_ANGLE);
#else
    return 0;
#endif
}

float AngleSensor::getAngleDegrees() {
#ifdef USE_ANGLE_SENSOR
    uint16_t raw = getRawAngle();
    
    // Handle wrap-around if needed (not implemented for simple pivot)
    // Assuming sensor range is within 0-4095 without crossing 0
    
    // Map raw to degrees based on calibration
    // Formula: Deg = (Raw - Zero) * (45 / (Raw45 - Zero))
    
    int32_t deltaRaw = (int32_t)raw - (int32_t)_rawZero;
    int32_t rangeRaw = (int32_t)_raw45 - (int32_t)_rawZero;
    
    if (rangeRaw == 0) rangeRaw = 1; // Prevent divide by zero
    
    float degrees = (float)deltaRaw * 45.0 / (float)rangeRaw;
    
    // Clamp to reasonable values
    if (degrees < 0) degrees = 0;
    if (degrees > 90) degrees = 90;
    
    // Hysteresis / Smoothing
    // Only update if change is > 0.2 degrees to prevent jitter
    if (abs(degrees - _lastDegrees) > 0.2) {
        _lastDegrees = degrees;
    }
    
    return _lastDegrees;
#else
    return 0.0;
#endif
}

void AngleSensor::setZeroPoint(uint16_t rawVal) {
    _rawZero = rawVal;
    // No persistence - calibration stored in RAM only
}

void AngleSensor::set45Point(uint16_t rawVal) {
    _raw45 = rawVal;
    // No persistence - calibration stored in RAM only
}
