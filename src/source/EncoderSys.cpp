#include "headers/EncoderSys.h"

EncoderSys::EncoderSys() {
    _encoder = new Encoder(PIN_ENCODER_A, PIN_ENCODER_B);
    _lastCount = 0;
    _wheelDiameter = DEFAULT_WHEEL_DIA_MM;
    _offsetMM = 0.0;
    recalculateCalibration();
}

void EncoderSys::init() {
    // Encoder library handles pin setup automatically
    reset();
}

void EncoderSys::update() {
    // In a polling setup, we might check for errors here
    // For now, the interrupt driver does the heavy lifting
}

void EncoderSys::reset() {
    _encoder->write(0);
    _lastCount = 0;
    _offsetMM = 0.0;  // Clear offset on manual reset
}

long EncoderSys::getRawCount() {
    return _encoder->read();
}

float EncoderSys::getDistanceMM() {
    long count = _encoder->read();
    return ((float)count * _mmPerPulse) + _offsetMM;
}

void EncoderSys::setWheelDiameter(float diameterMM) {
    if (diameterMM > 0) {
        _wheelDiameter = diameterMM;
        recalculateCalibration();
    }
}

float EncoderSys::getWheelDiameter() {
    return _wheelDiameter;
}

void EncoderSys::recalculateCalibration() {
    // Circumference = Pi * D
    // MM per Pulse = Circumference / PPR
    float circumference = PI * _wheelDiameter;
    _mmPerPulse = circumference / (float)PULSES_PER_REV;
}

void EncoderSys::setOffset(float offsetMM) {
    _offsetMM = offsetMM;
}
