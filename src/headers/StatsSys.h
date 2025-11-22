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

private:
    SystemSettings* _settings;
};

#endif // STATSSYS_H
