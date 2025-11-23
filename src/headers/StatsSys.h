#ifndef STATSSYS_H
#define STATSSYS_H

#include <Arduino.h>
#include "Storage.h"

// Minimum length to register a cut (prevent false positives)
#define MIN_CUT_LENGTH_MM 20.0

class StatsSys {
public:
    StatsSys();
    void init(SystemSettings* settings);
    void update(); // Call in main loop for time tracking
    
    // Call this when user ZEROs the system
    void registerCut(float lengthMM);
    
    void resetProject();
    
    unsigned long getProjectCuts();
    float getProjectLengthMeters();
    
    unsigned long getTotalCuts();
    float getTotalLengthMeters();
    
    // Waste tracking (Kerf compensation)
    float getProjectWasteMeters();  // Cuts × Kerf
    float getTotalWasteMeters();    // Total × Kerf
    
    // Advanced Stats
    float getAverageCutLengthMM();
    float getLastCutLengthMM();
    unsigned long getUptimeMinutes(); // Project Minutes
    float getCutsPerHour();
    
    // Cost & Time
    float getLaborCost();
    float getTotalHours(); // Global Hours

private:
    SystemSettings* _settings;
    float _lastCutLen; // Store last cut length
};

#endif // STATSSYS_H
