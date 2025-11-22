#include "headers/StatsSys.h"

StatsSys::StatsSys() {
    // Logic moved to init/Storage
}

void StatsSys::init(SystemSettings* settings) {
    _settings = settings;
}

void StatsSys::registerCut(float lengthMM) {
    // Smart Algorithm: Only count if length > Threshold
    if (abs(lengthMM) > MIN_CUT_LENGTH_MM) {
        float lenMeters = abs(lengthMM) / 1000.0;
        float kerfMeters = _settings->kerfMM / 1000.0;  // Kerf compensation
        
        // Update Project Stats (includes kerf waste)
        _settings->projectCuts++;
        _settings->projectLengthMeters += (lenMeters + kerfMeters);
        
        // Update Total Stats (Persistent, includes kerf waste)
        _settings->totalCuts++;
        _settings->totalLengthMeters += (lenMeters + kerfMeters);
        
        // Save to EEPROM
        Storage::save(*_settings);
    }
}

void StatsSys::resetProject() {
    _settings->projectCuts = 0;
    _settings->projectLengthMeters = 0.0;
    Storage::save(*_settings);
}

unsigned long StatsSys::getProjectCuts() {
    return _settings->projectCuts;
}

float StatsSys::getProjectLengthMeters() {
    return _settings->projectLengthMeters;
}

unsigned long StatsSys::getTotalCuts() {
    return _settings->totalCuts;
}

float StatsSys::getTotalLengthMeters() {
    return _settings->totalLengthMeters;
}

float StatsSys::getProjectWasteMeters() {
    return (_settings->projectCuts * _settings->kerfMM) / 1000.0;
}

float StatsSys::getTotalWasteMeters() {
    return (_settings->totalCuts * _settings->kerfMM) / 1000.0;
}
