#include "headers/StatsSys.h"

StatsSys::StatsSys() {
    // Logic moved to init/Storage
}

void StatsSys::init(SystemSettings* settings) {
    _settings = settings;
    _lastCutLen = 0.0f;
}

void StatsSys::registerCut(float lengthMM) {
    // Smart Algorithm: Only count if length > Threshold
    if (abs(lengthMM) > MIN_CUT_LENGTH_MM) {
        float lenMeters = abs(lengthMM) / 1000.0;
        float kerfMeters = _settings->kerfMM / 1000.0;  // Kerf compensation
        
        _lastCutLen = abs(lengthMM);
        
        // Update Project Stats (includes kerf waste)
        _settings->projectCuts++;
        _settings->projectLengthMeters += (lenMeters + kerfMeters);
        
        // Update Total Stats (Persistent, includes kerf waste)
        _settings->totalCuts++;
        _settings->totalLengthMeters += (lenMeters + kerfMeters);
        
        // Storage::save(*_settings); // Disabled - Will use AT24C256 EEPROM
    }
}

void StatsSys::resetProject() {
    _settings->projectCuts = 0;
    _settings->projectLengthMeters = 0.0f;
    _settings->projectSeconds = 0;
    // Storage::save(*_settings); // Disabled - Will use AT24C256 EEPROM
}

void StatsSys::update() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 1000) {
        lastUpdate = millis();
        _settings->projectSeconds++;
        _settings->totalSeconds++;
    }
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

float StatsSys::getAverageCutLengthMM() {
    if (_settings->projectCuts == 0) return 0.0;
    // Project Length includes Kerf, so subtract it to get raw material average
    float totalRawMeters = _settings->projectLengthMeters - getProjectWasteMeters();
    return (totalRawMeters * 1000.0) / _settings->projectCuts;
}

float StatsSys::getLastCutLengthMM() {
    return _lastCutLen;
}

unsigned long StatsSys::getUptimeMinutes() {
    return _settings->projectSeconds / 60;
}

float StatsSys::getCutsPerHour() {
    unsigned long mins = getUptimeMinutes();
    if (mins < 1) return 0.0;
    return (_settings->projectCuts * 60.0) / mins;
}

float StatsSys::getLaborCost() {
    float hours = _settings->projectSeconds / 3600.0;
    return hours * _settings->hourlyRate;
}

float StatsSys::getTotalHours() {
    return _settings->totalSeconds / 3600.0;
}
