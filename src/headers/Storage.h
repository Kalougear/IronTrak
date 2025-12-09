#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

struct SystemSettings
{
    float wheelDiameter = 50.0f;
    bool isInch = false;
    bool reverseDirection = false;
    unsigned long totalCuts = 0;
    float totalLengthMeters = 0.0f;
    unsigned long projectCuts = 0;
    float projectLengthMeters = 0.0f;

    // Phase 2 Settings
    float kerfMM = 0.0f;
    bool autoZeroEnabled = false;
    float autoZeroThresholdMM = 5.0f;
    uint8_t cutMode = 0;
    float cutAngle = 45.0f;
    uint8_t stockType = 0;
    uint8_t stockIdx = 0;
    uint8_t faceIdx = 0;

    // Phase 3 Settings (Angle Sensor)
    uint8_t lastAngle = 45;
    uint16_t angleRawZero = 0;
    uint16_t angleRaw45 = 512;
    bool useAngleSensor = false;

    // Time & Cost
    float hourlyRate = 30.0f;
    unsigned long projectSeconds = 0;
    unsigned long totalSeconds = 0;

    // Power Saving
    uint16_t backlightTimeoutSec = 300;  // Default: 5 minutes (0 = always ON)
};

class Storage
{
public:
    static void init();
    static void load(SystemSettings *settings);
    static void save(const SystemSettings &settings);
};

#endif // STORAGE_H
