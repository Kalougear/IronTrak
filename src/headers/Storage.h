#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <EEPROM.h>
#include <IWatchdog.h> // Include IWatchdog for reload()
#include "Config.h"

struct SystemSettings {
    float wheelDiameter;
    bool isInch;
    bool reverseDirection;
    unsigned long totalCuts;
    float totalLengthMeters;
    unsigned long projectCuts;
    float projectLengthMeters;
    
    // Phase 2 Settings
    float kerfMM;                  // Blade thickness
    bool autoZeroEnabled;          // Auto-Zero feature on/off
    float autoZeroThresholdMM;     // Motion threshold for trigger
    uint8_t cutMode;               // 0=0°, 1=45°, 2=Custom
    float cutAngle;                // Custom angle in degrees
    uint8_t stockType;             // 0=Rect, 1=Angle, 2=Cyl
    uint8_t stockIdx;              // Index into stock table
    uint8_t faceIdx;               // Face selection (0 or 1)
    
    // Time & Cost
    float hourlyRate;
    unsigned long projectSeconds;
    unsigned long totalSeconds;
};

class Storage {
public:
    static void init() {
#if defined(STM32F4xx)
        // STM32 EEPROM emulation setup
        EEPROM.begin(); 
#endif
        // Check magic byte
        if (EEPROM.read(EEPROM_ADDR_MAGIC) != 0x42) {
            // First time init
            resetDefaults();
        }
    }

    static void resetDefaults() {
        SystemSettings defaults;
        defaults.wheelDiameter = DEFAULT_WHEEL_DIA_MM;
        defaults.isInch = false;
        defaults.reverseDirection = false;
        defaults.totalCuts = 0;
        defaults.totalLengthMeters = 0.0;
        defaults.projectCuts = 0;
        defaults.projectLengthMeters = 0.0;
        defaults.kerfMM = 0.0;
        defaults.autoZeroEnabled = false;
        defaults.autoZeroThresholdMM = 5.0;
        defaults.cutMode = 0; // 0°
        defaults.cutAngle = 45.0;
        defaults.stockType = 0; // Rectangular
        defaults.stockIdx = 0;
        defaults.faceIdx = 0;
        
        defaults.hourlyRate = 50.0; // Default $50/hr
        defaults.projectSeconds = 0;
        defaults.totalSeconds = 0;
        
        save(defaults);
        EEPROM.write(EEPROM_ADDR_MAGIC, 0x42);
    }

    static SystemSettings load() {
        SystemSettings s;
        EEPROM.get(EEPROM_ADDR_DIA, s.wheelDiameter);
        
        // Sanity check
        if (isnan(s.wheelDiameter) || s.wheelDiameter < 10.0 || s.wheelDiameter > 500.0) {
            s.wheelDiameter = DEFAULT_WHEEL_DIA_MM;
        }
        
        s.isInch = EEPROM.read(EEPROM_ADDR_UNITS);
        s.reverseDirection = EEPROM.read(EEPROM_ADDR_DIR);
        EEPROM.get(EEPROM_ADDR_TOT_CUTS, s.totalCuts);
        EEPROM.get(EEPROM_ADDR_TOT_LEN, s.totalLengthMeters);
        EEPROM.get(EEPROM_ADDR_PROJ_CUTS, s.projectCuts);
        EEPROM.get(EEPROM_ADDR_PROJ_LEN, s.projectLengthMeters);
        EEPROM.get(EEPROM_ADDR_KERF, s.kerfMM);
        s.autoZeroEnabled = EEPROM.read(EEPROM_ADDR_AUTO_ZERO_EN);
        EEPROM.get(EEPROM_ADDR_AUTO_ZERO_TH, s.autoZeroThresholdMM);
        s.cutMode = EEPROM.read(EEPROM_ADDR_CUT_MODE);
        EEPROM.get(EEPROM_ADDR_CUT_ANGLE, s.cutAngle);
        s.stockType = EEPROM.read(EEPROM_ADDR_STOCK_TYPE);
        s.stockIdx = EEPROM.read(EEPROM_ADDR_STOCK_IDX);
        s.faceIdx = EEPROM.read(EEPROM_ADDR_FACE_IDX);
        EEPROM.get(EEPROM_ADDR_HOURLY_RATE, s.hourlyRate);
        EEPROM.get(EEPROM_ADDR_PROJ_SEC, s.projectSeconds);
        EEPROM.get(EEPROM_ADDR_TOT_SEC, s.totalSeconds);
        return s;
    }

    static void save(const SystemSettings& s) {
#if defined(STM32F4xx)
        IWatchdog.reload(); // Feed watchdog before heavy write
#endif
        EEPROM.put(EEPROM_ADDR_DIA, s.wheelDiameter);
        EEPROM.update(EEPROM_ADDR_UNITS, s.isInch);
        EEPROM.update(EEPROM_ADDR_DIR, s.reverseDirection);
        EEPROM.put(EEPROM_ADDR_TOT_CUTS, s.totalCuts);
        EEPROM.put(EEPROM_ADDR_TOT_LEN, s.totalLengthMeters);
        EEPROM.put(EEPROM_ADDR_PROJ_CUTS, s.projectCuts);
        EEPROM.put(EEPROM_ADDR_PROJ_LEN, s.projectLengthMeters);
        EEPROM.put(EEPROM_ADDR_KERF, s.kerfMM);
        EEPROM.update(EEPROM_ADDR_AUTO_ZERO_EN, s.autoZeroEnabled);
        EEPROM.put(EEPROM_ADDR_AUTO_ZERO_TH, s.autoZeroThresholdMM);
        EEPROM.update(EEPROM_ADDR_CUT_MODE, s.cutMode);
        EEPROM.put(EEPROM_ADDR_CUT_ANGLE, s.cutAngle);
        EEPROM.update(EEPROM_ADDR_STOCK_TYPE, s.stockType);
        EEPROM.update(EEPROM_ADDR_STOCK_IDX, s.stockIdx);
        EEPROM.update(EEPROM_ADDR_FACE_IDX, s.faceIdx);
        EEPROM.put(EEPROM_ADDR_HOURLY_RATE, s.hourlyRate);
        EEPROM.put(EEPROM_ADDR_PROJ_SEC, s.projectSeconds);
        EEPROM.put(EEPROM_ADDR_TOT_SEC, s.totalSeconds);
#if defined(STM32F4xx)
        IWatchdog.reload(); // Feed watchdog after write
#endif
    }
};

#endif // STORAGE_H
