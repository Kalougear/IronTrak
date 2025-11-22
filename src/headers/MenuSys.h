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
    MENU_STATS_VIEW,
    MENU_CALIB_SUBMENU,
    MENU_AUTO_CALIB,
    MENU_STOCK_SELECT,
    MENU_EDIT
};

enum MenuItem {
    ITEM_STATS,
    ITEM_CALIB,
    ITEM_CUT_MODE,
    ITEM_STOCK,
    ITEM_UNITS,
    ITEM_DIRECTION,
    ITEM_EXIT,
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
    
    // Stats submenu state
    int8_t _statsPage;  // 0=Proj, 1=ProjWaste, 2=Tot, 3=TotWaste, 4=Reset
    
    // Calibration sub menu state
    int8_t _calibSubItem; // 0=ViewDia, 1=Kerf, 2=AutoZero, 3=AutoZeroThresh, 4=AutoCalib, 5=Back
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
    
    void render(DisplaySys* display);
    void handleNavigation(InputEvent e);
    void handleStats(InputEvent e);
    void handleCalibSubmenu(InputEvent e);
    void handleAutoCalib(InputEvent e, EncoderSys* encoder);
    void handleStockSelect(InputEvent e);
    void handleEdit(InputEvent e);
};

#endif // MENUSYS_H
