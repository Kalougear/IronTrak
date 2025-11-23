#ifndef MENUSYS_H
#define MENUSYS_H

#include <Arduino.h>
#include "Config.h"
#include "Storage.h"
#include "DisplaySys.h"
#include "EncoderSys.h"
#include "StatsSys.h"
#include "UserInput.h"

enum MenuState {
    MENU_NAVIGATE,
    MENU_STATS_SELECT,   // Select Project vs Global vs Rate
    MENU_STATS_PROJECT,  // Project Stats View
    MENU_STATS_GLOBAL,   // Global Stats View
    MENU_CALIB_SUBMENU,
    MENU_AUTO_CALIB,
    MENU_STOCK_SELECT,
    MENU_EDIT
};

enum MenuItem {
    ITEM_STOCK,      // Most used - Stock selection
    ITEM_CUT_MODE,   // Most used - Blade angle
    ITEM_STATS,      // Project statistics
    ITEM_CALIB,      // System Setup
    ITEM_EXIT,       // Exit menu
    ITEM_COUNT
};

class MenuSys {
public:
    void init(SystemSettings* settings, StatsSys* stats);
    
    // Returns true if menu is still active, false if exited
    bool update(InputEvent e, DisplaySys* display, EncoderSys* encoder);

private:
    SystemSettings* _settings;
    StatsSys* _stats;
    MenuState _state;
    int8_t _currentItem;
    int8_t _scrollOffset; // Index of the item at the top of the screen
    
    // Stats submenu state
    int8_t _statsPage;  // Page index for stats
    int8_t _statsScrollOffset;
    int8_t _statsSubItem; // For Stats Select menu
    
    // Calibration state
    int8_t _calibSubItem;  // 0-5 (Wheel, Kerf, AutoZero, Thresh, AutoCalib, Back)
    int8_t _calibScrollOffset;
    int8_t _calibStep;    // For auto-calib wizard
    long _calibPulses;
    float _calibRealLen;
    
    // Stock selection state
    int8_t _stockPage;  // 0=Type selection, 1=Size selection, 2=Face selection
    
    // Temp edit values
    float _tempDia;
    float _tempKerf;
    float _tempAZThresh;
    uint8_t _tempCutMode;
    float _tempRate; // For editing hourly rate
    
    bool _needsRedraw;
    bool _exitRequest;
    unsigned long _lastActivityTime;  // For 15-second timeout
    
    void render(DisplaySys* display);
    void handleNavigation(InputEvent e);
    void handleStatsSelect(InputEvent e);
    void handleStatsProject(InputEvent e);
    void handleStatsGlobal(InputEvent e);
    void handleCalibSubmenu(InputEvent e);
    void handleAutoCalib(InputEvent e, EncoderSys* encoder);
    void handleStockSelect(InputEvent e);
    void handleEdit(InputEvent e);
};

#endif // MENUSYS_H
