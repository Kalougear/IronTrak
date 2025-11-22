#ifndef STATEMACHINE_H
#define STATEMACHINE_H

enum SystemState {
    STATE_IDLE,         // Waiting for user input, showing last measurement
    STATE_MEASURING,    // Actively counting pulses (maybe not needed if always counting, but useful for UI focus)
    STATE_MENU,         // User is navigating settings
    STATE_CALIBRATION,  // Specific mode for setting wheel diameter
    STATE_ERROR         // Hardware fault or invalid state
};

#endif // STATEMACHINE_H
