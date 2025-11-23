#ifndef MENUSYS_H
#define MENUSYS_H

#include <Arduino.h>
#include "UserInput.h"
#include "Config.h"
#include "Storage.h"
#include "StatsSys.h"
#include "DisplaySys.h"
#include "EncoderSys.h"
#include "AngleSensor.h"

// Menu States
enum MenuState
{
    MENU_NAVIGATE,
    MENU_EDIT,
    MENU_STOCK_SELECT,
    MENU_STATS_SELECT,
    MENU_STATS_PROJECT,
    MENU_STATS_GLOBAL,
    MENU_CALIBRATION_SUBMENU, // NEW: Calibration submenu
    MENU_SETTINGS_SUBMENU,    // NEW: Settings submenu
    MENU_AUTO_CALIB,
    MENU_CALIB_ANGLE_0,
    MENU_CALIB_ANGLE_45
};

enum MenuItem
{
    ITEM_STOCK,
    ITEM_CUT_MODE,
    ITEM_STATS,
    ITEM_CALIBRATION, // NEW: Hardware calibration (wizards, wheel, kerf)
    ITEM_SETTINGS,    // NEW: User preferences (units, angle source, auto-zero)
    ITEM_EXIT,
    ITEM_COUNT
};

class MenuSys
{
public:
    MenuSys();
    void init(SystemSettings *settings, StatsSys *stats, AngleSensor *angleSensor);

    // Returns true if menu is still active, false if exited
    bool update(InputEvent e, DisplaySys *display, EncoderSys *encoder);

private:
    SystemSettings *_settings;
    StatsSys *_stats;
    AngleSensor *_angleSensor;
    MenuState _state;
    int8_t _currentItem;
    int8_t _scrollOffset; // Index of the item at the top of the screen

    // Stats submenu state
    int8_t _statsPage; // Page index for stats
    int8_t _statsScrollOffset;
    int8_t _statsSubItem; // For Stats Select menu

    // Calibration state
    int8_t _calibSubItem; // 0-4 (Wheel Wizard, Angle Wizard, Wheel Dia, Kerf, Back)
    int8_t _calibScrollOffset;
    int8_t _calibStep; // For auto-calib wizard
    long _calibPulses;
    float _calibRealLen;
    unsigned long _lastCalibAdjustTime; // For acceleration
    uint8_t _calibAdjustSpeed;          // 0=slow, 1=medium, 2=fast

    // Settings state
    int8_t _settingsSubItem; // 0-5 (Units, Angle Src, Auto-Zero, Thresh, Dir, Back)
    int8_t _settingsScrollOffset;

    // Stock selection state
    int8_t _stockPage; // 0=Type selection, 1=Size selection, 2=Face selection

    // Temp edit values
    float _tempDia;
    float _tempKerf;
    float _tempAZThresh;
    uint8_t _tempCutMode;
    float _tempRate; // For editing hourly rate

    bool _editMode;
    bool _needsRedraw;
    bool _exitRequest;
    unsigned long _lastActivityTime; // For 15-second timeout
    unsigned long _warningEndTime;   // For flashing messages

    void render(DisplaySys *display);
    void handleNavigation(InputEvent e);
    void handleStatsSelect(InputEvent e);
    void handleStatsProject(InputEvent e);
    void handleStatsGlobal(InputEvent e);
    void handleCalibrationSubmenu(InputEvent e);
    void handleSettingsSubmenu(InputEvent e);
    void handleAutoCalib(InputEvent e, EncoderSys *encoder);
    void handleAngleWizard(InputEvent e);
    void handleStockSelect(InputEvent e);
    void handleEdit(InputEvent e);
};

#endif // MENUSYS_H
