#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

struct SystemSettings
{
    float wheelDiameter = 50.0;
    bool isInch = false;
    bool reverseDirection = false;
    unsigned long totalCuts = 0;
    float totalLengthMeters = 0.0;
    unsigned long projectCuts = 0;
    float projectLengthMeters = 0.0;

    // Phase 2 Settings
    float kerfMM = 0.0;
    bool autoZeroEnabled = false;
    float autoZeroThresholdMM = 5.0;
    uint8_t cutMode = 0;
    float cutAngle = 45.0;
    uint8_t stockType = 0;
    uint8_t stockIdx = 0;
    uint8_t faceIdx = 0;

    // Phase 3 Settings (Angle Sensor)
    uint8_t lastAngle = 45;
    uint16_t angleRawZero = 0;
    uint16_t angleRaw45 = 512;
    bool useAngleSensor = false;

    // Time & Cost
    float hourlyRate = 30.0;
    unsigned long projectSeconds = 0;
    unsigned long totalSeconds = 0;
};

#endif // STORAGE_H
