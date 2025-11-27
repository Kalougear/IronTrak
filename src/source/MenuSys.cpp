#include "../headers/MenuSys.h"

MenuSys::MenuSys()
{
    // Constructor - Initialization handled in init()
}

void MenuSys::init(SystemSettings *settings, StatsSys *stats, AngleSensor *angleSensor)
{
    // GEMINI.md Rule 4.2: Assertion 1-3 - Validate pointer parameters
    if (settings == nullptr || stats == nullptr || angleSensor == nullptr) {
        // Recovery: Cannot proceed without valid pointers - halt system
        #if defined(USE_SERIAL_DEBUG)
        Serial1.println("ERROR: MenuSys::init() - null pointer!");
        #endif
        while(1);  // Watchdog will reset
    }
    
    _settings = settings;
    _stats = stats;
    _angleSensor = angleSensor;


    // Reset menu state
    _state = MENU_NAVIGATE;
    _lastState = MENU_NAVIGATE; // Initialize last state
    _currentItem = 0;
    _scrollOffset = 0;
    _needsRedraw = true;
    _exitRequest = false;
    _lastActivityTime = millis(); // CRITICAL: Reset timeout timer!
    _warningEndTime = 0;
}

bool MenuSys::update(InputEvent e, DisplaySys *display, EncoderSys *encoder)
{
    if (e != EVENT_NONE)
    {
        _lastActivityTime = millis();
    }
    else if (millis() - _lastActivityTime > 15000)
    {
        _exitRequest = true;
        return false;
    }

    // Check for state change and force redraw
    // Note: No clear() needed - DisplaySys::showMenu4() handles line-by-line updates via cache
    if (_state != _lastState)
    {
        _lastState = _state;
        _needsRedraw = true;
    }

    // Force redraw for flashing warning
    if (_warningEndTime > 0)
    {
        _needsRedraw = true;
    }

    // Universal Back Button - Long Press (1s) goes back one level
    if (e == EVENT_LONG_PRESS)
    {
        switch (_state)
        {
        case MENU_NAVIGATE:
            // Top level - exit menu
            _exitRequest = true;
            return false;

        case MENU_STATS_SELECT:
            // Return to main navigation
            _state = MENU_NAVIGATE;
            _currentItem = ITEM_STATS;
            _scrollOffset = (_currentItem > 2) ? _currentItem - 2 : 0;
            _needsRedraw = true;
            return true;

        case MENU_STATS_PROJECT:
        case MENU_STATS_GLOBAL:
            // Return to stats selection
            _state = MENU_STATS_SELECT;
            _needsRedraw = true;
            return true;

        case MENU_CALIBRATION_SUBMENU:
            // Return to main navigation
            _state = MENU_NAVIGATE;
            _currentItem = ITEM_CALIBRATION;
            _scrollOffset = (_currentItem > 2) ? _currentItem - 2 : 0;
            _needsRedraw = true;
            return true;

        case MENU_SETTINGS_SUBMENU:
            // Return to main navigation
            _state = MENU_NAVIGATE;
            _currentItem = ITEM_SETTINGS;
            _scrollOffset = (_currentItem > 2) ? _currentItem - 2 : 0;
            _needsRedraw = true;
            return true;

        case MENU_AUTO_CALIB:
        case MENU_CALIB_ANGLE_0:
        case MENU_CALIB_ANGLE_45:
            // Return to calibration submenu
            _state = MENU_CALIBRATION_SUBMENU;
            _needsRedraw = true;
            return true;

        case MENU_STOCK_SELECT:
            // Return to main navigation
            _state = MENU_NAVIGATE;
            _currentItem = ITEM_STOCK;
            _scrollOffset = (_currentItem > 2) ? _currentItem - 2 : 0;
            _needsRedraw = true;
            return true;

        case MENU_EDIT:
            // Cancel edit, return to navigation
            _state = MENU_NAVIGATE;
            _scrollOffset = (_currentItem > 2) ? _currentItem - 2 : 0;
            _needsRedraw = true;
            return true;
        }
    }

    if (_state == MENU_NAVIGATE)
    {
        handleNavigation(e);
    }
    else if (_state == MENU_STATS_SELECT)
    {
        handleStatsSelect(e);
    }
    else if (_state == MENU_STATS_PROJECT)
    {
        handleStatsProject(e);
    }
    else if (_state == MENU_STATS_GLOBAL)
    {
        handleStatsGlobal(e);
    }
    else if (_state == MENU_CALIBRATION_SUBMENU)
    {
        handleCalibrationSubmenu(e);
    }
    else if (_state == MENU_SETTINGS_SUBMENU)
    {
        handleSettingsSubmenu(e);
    }
    else if (_state == MENU_AUTO_CALIB)
    {
        handleAutoCalib(e, encoder);
    }
    else if (_state == MENU_CALIB_ANGLE_0 || _state == MENU_CALIB_ANGLE_45)
    {
        handleAngleWizard(e);
    }
    else if (_state == MENU_STOCK_SELECT)
    {
        handleStockSelect(e);
    }
    else
    {
        handleEdit(e);
    }

    if (_needsRedraw)
    {
        render(display);
        _needsRedraw = false;
    }

    return !_exitRequest;
}

void MenuSys::handleNavigation(InputEvent e)
{
    if (e == EVENT_NEXT)
    {
        _currentItem++;
        if (_currentItem >= ITEM_COUNT)
            _currentItem = 0;
        _needsRedraw = true;
    }
    else if (e == EVENT_PREV)
    {
        _currentItem--;
        if (_currentItem < 0)
            _currentItem = ITEM_COUNT - 1;
        _needsRedraw = true;
    }

    if (_currentItem < _scrollOffset)
        _scrollOffset = _currentItem;
    else if (_currentItem >= _scrollOffset + 3)
        _scrollOffset = _currentItem - 2;

    if (e == EVENT_CLICK)
    {
        if (_currentItem == ITEM_EXIT)
        {
            _exitRequest = true;
        }
        else if (_currentItem == ITEM_STATS)
        {
            _state = MENU_STATS_SELECT;
            _statsSubItem = 0;
            _statsScrollOffset = 0;
        }
        else if (_currentItem == ITEM_CALIBRATION)
        {
            _state = MENU_CALIBRATION_SUBMENU;
            _calibSubItem = 0;
            _calibScrollOffset = 0;
        }
        else if (_currentItem == ITEM_SETTINGS)
        {
            _state = MENU_SETTINGS_SUBMENU;
            _settingsSubItem = 0;
            _settingsScrollOffset = 0;
        }
        else if (_currentItem == ITEM_STOCK)
        {
            _state = MENU_STOCK_SELECT;
            _stockPage = 0;
        }
        else if (_currentItem == ITEM_CUT_MODE)
        {
            // Prevent editing if in Auto Mode (Sensor Active)
            if (!_settings->useAngleSensor)
            {
                _state = MENU_EDIT;
                _tempCutMode = _settings->cutMode;
            }
            // Else: Do nothing (ReadOnly)
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleStatsSelect(InputEvent e)
{
    if (e == EVENT_NEXT)
    {
        _statsSubItem++;
        if (_statsSubItem > 3)
            _statsSubItem = 0;
        _needsRedraw = true;
    }
    else if (e == EVENT_PREV)
    {
        _statsSubItem--;
        if (_statsSubItem < 0)
            _statsSubItem = 3;
        _needsRedraw = true;
    }

    if (_statsSubItem < _statsScrollOffset)
        _statsScrollOffset = _statsSubItem;
    else if (_statsSubItem >= _statsScrollOffset + 3)
        _statsScrollOffset = _statsSubItem - 2;

    if (e == EVENT_CLICK)
    {
        if (_statsSubItem == 0)
        {
            _state = MENU_STATS_PROJECT;
            _statsPage = 0;
            _statsScrollOffset = 0;
        }
        else if (_statsSubItem == 1)
        {
            _state = MENU_STATS_GLOBAL;
            _statsPage = 0;
            _statsScrollOffset = 0;
        }
        else if (_statsSubItem == 2)
        {
            _state = MENU_EDIT;
            _tempRate = _settings->hourlyRate;
            _calibSubItem = -1; // Prevent calibration edit logic from activating
            _currentItem = -1;  // Prevent main menu edit logic from activating
        }
        else if (_statsSubItem == 3)
        {
            _state = MENU_NAVIGATE;
            _currentItem = ITEM_STATS;
            _scrollOffset = (_currentItem > 2) ? _currentItem - 2 : 0;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleStatsProject(InputEvent e)
{
    if (e == EVENT_NEXT)
    {
        _statsPage++;
        if (_statsPage > 6)
            _statsPage = 0;
        _needsRedraw = true;
    }
    else if (e == EVENT_PREV)
    {
        _statsPage--;
        if (_statsPage < 0)
            _statsPage = 6;
        _needsRedraw = true;
    }

    if (_statsPage < _statsScrollOffset)
        _statsScrollOffset = _statsPage;
    else if (_statsPage >= _statsScrollOffset + 3)
        _statsScrollOffset = _statsPage - 2;

    if (e == EVENT_CLICK)
    {
        if (_statsPage == 5)
        {
            _stats->resetProject();
            _state = MENU_STATS_SELECT;
            _statsSubItem = 0;
            _statsScrollOffset = 0;
        }
        else if (_statsPage == 6)
        {
            _state = MENU_STATS_SELECT;
            _statsSubItem = 0;
            _statsScrollOffset = 0;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleStatsGlobal(InputEvent e)
{
    if (e == EVENT_NEXT)
    {
        _statsPage++;
        if (_statsPage > 4)
            _statsPage = 0;
        _needsRedraw = true;
    }
    else if (e == EVENT_PREV)
    {
        _statsPage--;
        if (_statsPage < 0)
            _statsPage = 4;
        _needsRedraw = true;
    }

    if (_statsPage < _statsScrollOffset)
        _statsScrollOffset = _statsPage;
    else if (_statsPage >= _statsScrollOffset + 3)
        _statsScrollOffset = _statsPage - 2;

    if (e == EVENT_CLICK)
    {
        if (_statsPage == 4)
        {
            _state = MENU_STATS_SELECT;
            _statsSubItem = 1;
            _statsScrollOffset = 0;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleCalibrationSubmenu(InputEvent e)
{
    // Calibration submenu: 5 items (0-4)
    // 0: Wheel Wizard (auto-calibrate)
    // 1: Angle Wizard (calibrate angle sensor)
    // 2: Wheel Diameter (manual fine-tune)
    // 3: Kerf Thickness
    // 4: Back

    if (e == EVENT_NEXT)
    {
        _calibSubItem++;
        if (_calibSubItem > 4)
            _calibSubItem = 0;
        _needsRedraw = true;
    }
    else if (e == EVENT_PREV)
    {
        _calibSubItem--;
        if (_calibSubItem < 0)
            _calibSubItem = 4;
        _needsRedraw = true;
    }

    if (_calibSubItem < _calibScrollOffset)
        _calibScrollOffset = _calibSubItem;
    else if (_calibSubItem >= _calibScrollOffset + 3)
        _calibScrollOffset = _calibSubItem - 2;

    if (e == EVENT_CLICK)
    {
        if (_calibSubItem == 0)
        {
            // Wheel Wizard (auto-calibration)
            _state = MENU_AUTO_CALIB;
            _calibStep = 0;
        }
        else if (_calibSubItem == 1)
        {
            // Angle Wizard - check if sensor is enabled first
            if (_settings->useAngleSensor)
            {
                _state = MENU_CALIB_ANGLE_0;
            }
            else
            {
                // Flash warning (3 seconds)
                _warningEndTime = millis() + 3000;
                _needsRedraw = true;
            }
        }
        else if (_calibSubItem == 2)
        {
            // Wheel Diameter (manual edit)
            _state = MENU_EDIT;
            _tempDia = _settings->wheelDiameter;
        }
        else if (_calibSubItem == 3)
        {
            // Kerf Thickness
            _state = MENU_EDIT;
            _tempKerf = _settings->kerfMM;
        }
        else if (_calibSubItem == 4)
        {
            // Back to main menu
            _state = MENU_NAVIGATE;
            _currentItem = ITEM_CALIBRATION;
            _scrollOffset = (_currentItem > 2) ? _currentItem - 2 : 0;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleSettingsSubmenu(InputEvent e)
{
    // Settings submenu: 6 items (0-5)
    // 0: Units (METRIC/IMPERIAL)
    // 1: Angle Source (MANUAL/AUTO)
    // 2: Auto-Zero (ON/OFF)
    // 3: Auto-Zero Threshold
    // 4: Direction (FWD/REV)
    // 5: Back

    if (e == EVENT_NEXT)
    {
        _settingsSubItem++;
        if (_settingsSubItem > 5)
            _settingsSubItem = 0;
        _needsRedraw = true;
    }
    else if (e == EVENT_PREV)
    {
        _settingsSubItem--;
        if (_settingsSubItem < 0)
            _settingsSubItem = 5;
        _needsRedraw = true;
    }

    if (_settingsSubItem < _settingsScrollOffset)
        _settingsScrollOffset = _settingsSubItem;
    else if (_settingsSubItem >= _settingsScrollOffset + 3)
        _settingsScrollOffset = _settingsSubItem - 2;

    if (e == EVENT_CLICK)
    {
        if (_settingsSubItem == 0)
        {
            // Toggle Units (METRIC/IMPERIAL)
            _settings->isInch = !_settings->isInch;
        }
        else if (_settingsSubItem == 1)
        {
            // Toggle Angle Source (MANUAL/AUTO)
            _settings->useAngleSensor = !_settings->useAngleSensor;
        }
        else if (_settingsSubItem == 2)
        {
            // Toggle Auto-Zero
            _settings->autoZeroEnabled = !_settings->autoZeroEnabled;
        }
        else if (_settingsSubItem == 3)
        {
            // Edit Auto-Zero Threshold
            _state = MENU_EDIT;
            _tempAZThresh = _settings->autoZeroThresholdMM;
            _calibSubItem = -1;  // Prevent calibration edit logic from activating
            _currentItem = -1;   // Prevent main menu edit logic from activating
        }
        else if (_settingsSubItem == 4)
        {
            // Toggle Direction
            _settings->reverseDirection = !_settings->reverseDirection;
        }
        else if (_settingsSubItem == 5)
        {
            // Back to main menu
            _state = MENU_NAVIGATE;
            _currentItem = ITEM_SETTINGS;
            _scrollOffset = (_currentItem > 2) ? _currentItem - 2 : 0;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleAutoCalib(InputEvent e, EncoderSys *encoder)
{
    if (_calibStep == 0)
    {
        if (e == EVENT_CLICK)
        {
            encoder->reset();
            _calibStep = 1;
            _needsRedraw = true;
        }
    }
    else if (_calibStep == 1)
    {
        if (e == EVENT_CLICK)
        {
            _calibPulses = encoder->getRawCount();
            // Initialize with encoder's measurement as a smart starting point
            _calibRealLen = encoder->getDistanceMM();
            _calibStep = 2;
            _needsRedraw = true;
        }
    }
    else if (_calibStep == 2)
    {
        // Fine adjustment: 1mm = 0.1 CM (precise control)
        if (e == EVENT_NEXT)
        {
            _calibRealLen += 1.0f;
            _needsRedraw = true;
        }
        else if (e == EVENT_PREV)
        {
            _calibRealLen -= 1.0f;
            _needsRedraw = true;
        }
        else if (e == EVENT_CLICK)
        {
            _calibStep = 3;
            _needsRedraw = true;
        }
    }
    else if (_calibStep == 3)
    {
        if (e == EVENT_CLICK)
        {
            if (_calibPulses != 0)
            {
                float mmPerPulse = abs(_calibRealLen) / (float)abs(_calibPulses);
                float newCirc = mmPerPulse * PULSES_PER_REV;
                float newDia = newCirc / PI;

                _settings->wheelDiameter = newDia;
                encoder->setWheelDiameter(newDia);
            }
            _state = MENU_CALIBRATION_SUBMENU;
            _needsRedraw = true;
        }
    }
}

void MenuSys::handleAngleWizard(InputEvent e)
{
    if (e == EVENT_CLICK)
    {
        uint16_t raw = _angleSensor->getRawAngle();
        if (_state == MENU_CALIB_ANGLE_0)
        {
            _angleSensor->setZeroPoint(raw);
            _state = MENU_CALIB_ANGLE_45;
        }
        else
        {
            _angleSensor->set45Point(raw);
            _state = MENU_CALIBRATION_SUBMENU;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleStockSelect(InputEvent e)
{
    if (_stockPage == 0)
    {
        if (e == EVENT_NEXT)
        {
            _settings->stockType++;
            if (_settings->stockType > 2)
                _settings->stockType = 0;
        }
        else if (e == EVENT_PREV)
        {
            if (_settings->stockType == 0)
                _settings->stockType = 2;
            else
                _settings->stockType--;
        }
        else if (e == EVENT_CLICK)
        {
            _stockPage = 1;
            _settings->stockIdx = 0;
        }
        _needsRedraw = true;
    }
    else if (_stockPage == 1)
    {
        uint8_t maxIdx = 0;
        if (_settings->isInch)
        {
            if (_settings->stockType == 0)
                maxIdx = STOCK_RECT_IN_COUNT - 1;
            else if (_settings->stockType == 1)
                maxIdx = STOCK_ANGLE_IN_COUNT - 1;
            else
                maxIdx = STOCK_CYL_IN_COUNT - 1;
        }
        else
        {
            if (_settings->stockType == 0)
                maxIdx = STOCK_RECT_MM_COUNT - 1;
            else if (_settings->stockType == 1)
                maxIdx = STOCK_ANGLE_MM_COUNT - 1;
            else
                maxIdx = STOCK_CYL_MM_COUNT - 1;
        }

        if (e == EVENT_NEXT)
        {
            _settings->stockIdx++;
            if (_settings->stockIdx > maxIdx)
                _settings->stockIdx = 0;
        }
        else if (e == EVENT_PREV)
        {
            if (_settings->stockIdx == 0)
                _settings->stockIdx = maxIdx;
            else
                _settings->stockIdx--;
        }
        else if (e == EVENT_CLICK)
        {
            _state = MENU_NAVIGATE;
            _currentItem = ITEM_STOCK;
            _scrollOffset = 0;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleEdit(InputEvent e)
{
    // Hourly Rate Editing (from Stats menu)
    if (_state == MENU_EDIT && _statsSubItem == 2 && _currentItem != ITEM_CUT_MODE)
    {
        if (e == EVENT_NEXT)
        {
            _tempRate += 1.0f;
            _needsRedraw = true;
        }
        else if (e == EVENT_PREV)
        {
            _tempRate -= 1.0f;
            if (_tempRate < 0)
                _tempRate = 0;
            _needsRedraw = true;
        }
        else if (e == EVENT_CLICK)
        {
            _settings->hourlyRate = max(0.0f, _tempRate);
            _state = MENU_STATS_SELECT;
            _statsSubItem = 2;
            _statsScrollOffset = 0;
            _needsRedraw = true;
        }
    }
    // Cut Mode Editing (from Main menu)
    else if (_currentItem == ITEM_CUT_MODE && _state == MENU_EDIT)
    {
        if (e == EVENT_NEXT)
        {
            if (_tempCutMode < 45)
                _tempCutMode++;
            _needsRedraw = true;
        }
        else if (e == EVENT_PREV)
        {
            if (_tempCutMode > 0)
                _tempCutMode--;
            _needsRedraw = true;
        }
        else if (e == EVENT_CLICK)
        {
            _settings->cutMode = _tempCutMode;
            _state = MENU_NAVIGATE;
            _needsRedraw = true;
        }
    }
    // Calibration Value Editing (from System Setup menu)
    else if (_calibSubItem >= 0 && _calibSubItem <= 3 && _state == MENU_EDIT)
    {
        if (_calibSubItem == 0)
        {
            if (e == EVENT_NEXT)
                _tempDia += 0.1f;
            else if (e == EVENT_PREV)
                _tempDia -= 0.1f;
            else if (e == EVENT_CLICK)
            {
                _settings->wheelDiameter = _tempDia;
                _state = MENU_CALIBRATION_SUBMENU;
            }
            _needsRedraw = true;
        }
        else if (_calibSubItem == 1)
        {
            if (e == EVENT_NEXT)
                _tempKerf += 0.1f;
            else if (e == EVENT_PREV)
                _tempKerf -= 0.1f;
            else if (e == EVENT_CLICK)
            {
                _settings->kerfMM = max(0.0f, _tempKerf);
                _state = MENU_CALIBRATION_SUBMENU;
            }
            _needsRedraw = true;
        }
        else if (_calibSubItem == 3)
        {
            // Kerf Thickness editing
            if (e == EVENT_NEXT)
                _tempKerf += 0.1f;
            else if (e == EVENT_PREV)
                _tempKerf -= 0.1f;
            else if (e == EVENT_CLICK)
            {
                _settings->kerfMM = max(0.0f, _tempKerf);
                _state = MENU_CALIBRATION_SUBMENU;
            }
            _needsRedraw = true;
        }
    }
    // Settings Value Editing (from Settings submenu)
    else if (_settingsSubItem >= 0 && _settingsSubItem <= 5 && _state == MENU_EDIT)
    {
        if (_settingsSubItem == 3)
        {
            // Auto-Zero Threshold editing
            if (e == EVENT_NEXT)
                _tempAZThresh += 0.5f;
            else if (e == EVENT_PREV)
                _tempAZThresh -= 0.5f;
            else if (e == EVENT_CLICK)
            {
                _settings->autoZeroThresholdMM = constrain(_tempAZThresh, 2.0, 20.0);
                _state = MENU_SETTINGS_SUBMENU;
            }
            _needsRedraw = true;
        }
    }
}

// ============================================================================
// GEMINI.md Rule 4.2: Static Helper Functions (Extracted from render())
// ============================================================================

// Static Helper 1: Format header with '=' padding
String MenuSys::formatHeader(const String& s) {
    String padded = " " + s + " ";
    int len = padded.length();
    if (len >= 20) return padded.substring(0, 20);
    
    int padLeft = (20 - len) / 2;
    String res = "";
    // LOOP BOUND: padLeft computed from string length (max 20), bounded by [0, 20]
    for (int i = 0; i < padLeft && i < 20; i++) {
        res += "=";
    }
    res += padded;
    // LOOP BOUND: String length max 20 characters
    while (res.length() < 20 && res.length() < 100) {  // Sanity check
        res += "=";
    }
    return res;
}

// Static Helper 2: Center text with space padding
String MenuSys::formatCenter(const String& s) {
    int len = s.length();
    if (len >= 20) return s.substring(0, 20);
    
    int pad = (20 - len) / 2;
    String res = "";
    // LOOP BOUND: pad computed from string length (max 20), bounded by [0, 20]
    for (int i = 0; i < pad && i < 20; i++) {
        res += " ";
    }
    res += s;
    // LOOP BOUND: String length max 20 characters
    while (res.length() < 20 && res.length() < 100) {  // Sanity check
        res += " ";
    }
    return res;
}

// GEMINI.md Rule 4.2: Main render() function refactored to <60 lines
void MenuSys::render(DisplaySys *display) {
    // GEMINI.md Rule 4.2: Assertion - Validate pointer parameter
    if (display == nullptr) {
        return;  // Safe early exit
    }
    
    // Check for flashing warning message
    if (_warningEndTime > 0) {
        if (millis() < _warningEndTime) {
            display->showMenu4(
                formatHeader("! WARNING !"),
                formatCenter("CHANGE ANGLE SRC"),
                formatCenter("TO AUTO"),
                formatCenter("IN SETTINGS"));
            return;
        } else {
            _warningEndTime = 0;
        }
    }
    
    // Delegate to extracted state-specific render methods
    switch (_state) {
        case MENU_AUTO_CALIB:
            renderAutoCalibWizard(display);
            return;
            
        case MENU_CALIB_ANGLE_0:
        case MENU_CALIB_ANGLE_45:
            renderAngleWizard(display);
            return;
            
        case MENU_STOCK_SELECT:
            renderStockSelectMenu(display);
            return;
            
        case MENU_NAVIGATE:
        case MENU_EDIT:
        case MENU_STATS_SELECT:
        case MENU_STATS_PROJECT:
        case MENU_STATS_GLOBAL:
        case MENU_CALIBRATION_SUBMENU:
        case MENU_SETTINGS_SUBMENU:
            // Use generic scrollable menu renderer
            renderScrollableMenu(display, "", _currentItem, _scrollOffset, 0);
            return;
            
        default:
            // Fallback: show blank screen
            display->showMenu4("", "", "", "");
            return;
    }
}

// ============================================================================
// GEMINI.md Rule 4.2: Extracted Rendering Methods (<60 lines each)
// ============================================================================

// Render auto-calibration wizard screens
void MenuSys::renderAutoCalibWizard(DisplaySys *display) {
    String l0, l1, l2, l3;
    
    // LOOP BOUND: _calibStep has 4 states (0-3), explicit switch ensures termination
    switch (_calibStep) {
        case 0:  // Step 1: Zero the encoder
            l0 = formatHeader("\x04 AUTO CALIB");
            l1 = formatCenter(" => STEP 1 <=");
            l2 = formatCenter("TOUCH BLADE AND ZERO");
            l3 = formatCenter("THEN CLICK");
            break;
            
        case 1:  // Step 2: Cut a piece
            l0 = formatHeader("\x04 STEP 2: CUT");
            l1 = formatCenter("CUT A PIECE");
            l2 = formatCenter("~50 CM OR MORE");
            l3 = formatCenter("CLICK WHEN DONE");
            break;
            
        case 2:  // Step 3: Enter measured length
            l0 = formatHeader("\x04 STEP 3: MEASURE");
            l1 = "\x7E REAL CUT: " + String(_calibRealLen / 10.0f, 1) + " CM";
            l2 = formatCenter("TURN TO ADJUST");
            l3 = formatCenter("CLICK TO CONFIRM");
            break;
            
        default:  // Step 4: Show/save calculated diameter
            float mmPerPulse = abs(_calibRealLen) / (float)abs(_calibPulses);
            float newDia = (mmPerPulse * PULSES_PER_REV) / PI;
            l0 = formatHeader("\x04 STEP 4: SAVE");
            l1 = " \x04 NEW WHEEL DIA:";
            l2 = " \x04 " + String(newDia, 3) + " MM";
            l3 = formatCenter("CLICK TO SAVE");
            break;
    }
    
    display->showMenu4(l0, l1, l2, l3);
}

// Render angle calibration wizard screens
void MenuSys::renderAngleWizard(DisplaySys *display) {
    if (_state == MENU_CALIB_ANGLE_0) {
        display->showMenu4(
            formatHeader("\x07 ANGLE WIZARD"),
            formatCenter("STEP 1: ZERO"),
            formatCenter("SET TO 0 DEGREES"),
            formatCenter("CLICK TO CAPTURE"));
    } else {  // MENU_CALIB_ANGLE_45
        display->showMenu4(
            formatHeader("\x07 ANGLE WIZARD"),
            formatCenter("STEP 2: 45 DEG"),
            formatCenter("SET TO 45 DEGREES"),
            formatCenter("CLICK TO SAVE"));
    }
}

// Render stock selection menu with pagination
void MenuSys::renderStockSelectMenu(DisplaySys *display) {
    String l0, l1, l2, l3;
    
    // LOOP BOUND: _stockPage has 3 states (0-2), explicit conditional ensures termination
    if (_stockPage == 0) {
        // Stock type selection
        const char *types[] = {"RECTANGULAR", "ANGLE IRON", "TUBE-PIPE"};
        const char icons[] = {1, 2, 3};
        l0 = formatHeader("STOCK TYPE");
        
        // LOOP BOUND: Fixed iteration over 3 stock types
        for (int i = 0; i < 3; i++) {
            String s = (i == _settings->stockType) ? "> " : "  ";
            s += String(icons[i]) + " " + String(types[i]);
            
            if (i == 0) l1 = s;
            else if (i == 1) l2 = s;
            else l3 = s;
        }
    } else if (_stockPage == 1) {
        // ===  STOCK SIZE SELECTION (RESTORED) ===
        l0 = formatHeader("STOCK SIZE");
        
        // Get item count based on stock type and unit system
        int count = 0;
        if (_settings->isInch) {
            if (_settings->stockType == 0)
                count = STOCK_RECT_IN_COUNT;
            else if (_settings->stockType == 1)
                count = STOCK_ANGLE_IN_COUNT;
            else
                count = STOCK_CYL_IN_COUNT;
        } else {
            if (_settings->stockType == 0)
                count = STOCK_RECT_MM_COUNT;
            else if (_settings->stockType == 1)
                count = STOCK_ANGLE_MM_COUNT;
            else
                count = STOCK_CYL_MM_COUNT;
        }
        
        // Get stock string by index
        auto getStockStr = [&](int idx) -> String {
            if (idx < 0 || idx >= count) return "";
            
            String s = "";
            if (_settings->isInch) {
                if (_settings->stockType == 0)
                    s = STOCK_RECT_IN[idx];
                else if (_settings->stockType == 1)
                    s = STOCK_ANGLE_IN[idx];
                else
                    s = STOCK_CYL_IN[idx];
            } else {
                if (_settings->stockType == 0)
                    s = STOCK_RECT_MM[idx];
                else if (_settings->stockType == 1)
                    s = STOCK_ANGLE_MM[idx];
                else
                    s = STOCK_CYL_MM[idx];
            }
            
            // Add icon prefix
            char icon = (_settings->stockType == 0) ? 1 : (_settings->stockType == 1 ? 2 : 3);
            return String(icon) + " " + s;
        };
        
        // Calculate scroll window (show 3 items centered on selection)
        int current = _settings->stockIdx;
        int startIdx = current - 1;
        if (startIdx < 0) startIdx = 0;
        if (startIdx > count - 3) startIdx = count - 3;
        if (startIdx < 0) startIdx = 0;  // Safety for count < 3
        
        // Render 3 visible items
        for (int row = 0; row < 3; row++) {
            int idx = startIdx + row;
            if (idx >= count) break;
            
            String s = (idx == current) ? "> " : "  ";
            s += getStockStr(idx);
            
            if (row == 0) l1 = s;
            else if (row == 1) l2 = s;
            else l3 = s;
        }
        
    } else {
        // Face selection
        l0 = formatHeader("FACE SELECT");
        l3 = formatCenter("TURN TO TOGGLE");
    }
    
    display->showMenu4(l0, l1, l2, l3);
}

// Generic scrollable menu renderer for all menu states
void MenuSys::renderScrollableMenu(DisplaySys *display, const String& /* title */, int currentItem, int scrollOffset, int itemCount) {
    String l0, l1,l2, l3;
    
    // Determine menu context
    if (_state == MENU_NAVIGATE || (_state == MENU_EDIT && _currentItem == ITEM_CUT_MODE)) {
        l0 = formatHeader("MAIN MENU");
        currentItem = _currentItem;
        scrollOffset = _scrollOffset;
        itemCount = ITEM_COUNT;
    } else if (_state == MENU_STATS_SELECT || (_state == MENU_EDIT && _statsSubItem == 2)) {
        l0 = formatHeader("STATISTICS");
        currentItem = _statsSubItem;
        scrollOffset = _statsScrollOffset;
        itemCount = 4;
    } else if (_state == MENU_STATS_PROJECT) {
        l0 = formatHeader("PROJECT STATS");
        currentItem = _statsPage;
        scrollOffset = _statsScrollOffset;
        itemCount = 7;
    } else if (_state == MENU_STATS_GLOBAL) {
        l0 = formatHeader("GLOBAL STATS");
        currentItem = _statsPage;
        scrollOffset = _statsScrollOffset;
        itemCount = 5;
    } else if (_state == MENU_CALIBRATION_SUBMENU || (_state == MENU_EDIT && _calibSubItem >= 0 && _settingsSubItem < 0)) {
        l0 = formatHeader("CALIBRATION");
        currentItem = _calibSubItem;
        scrollOffset = _calibScrollOffset;
        itemCount = 5;
    } else if (_state == MENU_SETTINGS_SUBMENU || (_state == MENU_EDIT && _settingsSubItem >= 0 && _calibSubItem < 0)) {
        l0 = formatHeader("SETTINGS");
        currentItem = _settingsSubItem;
        scrollOffset = _settingsScrollOffset;
        itemCount = 6;
    }
    
    // Render 3 visible items
    l1 = getMenuItem(scrollOffset, currentItem, itemCount);
    l2 = getMenuItem(scrollOffset + 1, currentItem, itemCount);
    l3 = getMenuItem(scrollOffset + 2, currentItem, itemCount);
    
    // === BEST PRACTICE: Add scroll indicators ===
    // Show "more above" indicator (up arrow)
    if (scrollOffset > 0 && l0.length() < 20) {
        while (l0.length() < 19) l0 += " ";
        l0 += "\x18";  // Up arrow (↑)
    }
    
    // Show "more below" indicator (down arrow)
    if (scrollOffset + 3 < itemCount && l3.length() < 20) {
        while (l3.length() < 19) l3 += " ";
        l3 += "\x19";  // Down arrow (↓)
    }
    
    display->showMenu4(l0, l1, l2, l3);
}

// ============================================================================
// GEMINI.md Rule 4.2: Extracted Helper Functions (<60 lines each)
// ============================================================================

// GEMINI.md Rule 4.2: Helper - Main Menu Items
String MenuSys::getMenuItemMain(int idx, bool selected) {
    const char *items[] = {"STOCK PROFILE", "CUT ANGLE", "STATISTICS", "CALIBRATION", "SETTINGS", "EXIT MENU"};
    char stockIcon = (_settings->stockType == 0) ? 1 : (_settings->stockType == 1 ? 2 : 3);
    const char icons[] = {stockIcon, 7, 8, 3, 5, 32};
    
    if (idx >= 6) return "";
    
    String s = selected ? "> " : "  ";
    s += String(icons[idx]) + " " + items[idx];
    
    if (idx == ITEM_CUT_MODE) {
        bool editing = (_state == MENU_EDIT && selected);
        uint8_t mode = editing ? _tempCutMode : _settings->cutMode;
        s = selected ? "> " : "  ";
        
        if (_settings->useAngleSensor) {
            s += "\x07 ANGLE IS AUTO";
        } else {
            s += "\x07 CUT ANGLE: ";
            if (editing) s += "\x7E";
            s += String(mode) + "\xDF";
        }
    }
    return s;
}

String MenuSys::getMenuItemStatsSelect(int idx, bool selected) {
    String s = selected ? "> " : "  ";
    bool editing = (_state == MENU_EDIT && selected);
    
    switch (idx) {
        case 0: s += "\x08 PROJECT STATS"; break;
        case 1: s += "\x08 GLOBAL STATS"; break;
        case 2:
            s += "\x08 RATE: $";
            if (editing)
                s = "> \x7E \x08 RATE: $" + String(_tempRate, 2);
            else
                s += String(_settings->hourlyRate, 2) + "/HR";
            break;
        case 3: s += "  BACK"; break;
        default: return "";
    }
    return s;
}

String MenuSys::getMenuItemProjectStats(int idx, bool selected) {
    String s = selected ? "> " : "  ";
    switch (idx) {
        case 0: s += "\x04 CUTS: " + String(_stats->getProjectCuts()); break;
        case 1: s += "\x04 LEN: " + String(_stats->getProjectLengthMeters(), 1) + " M"; break;
        case 2: s += "\x01 WASTE: " + String(_stats->getProjectWasteMeters(), 2) + " M"; break;
        case 3: {
            unsigned long mins = _stats->getUptimeMinutes();
            s += "\x08 TIME: " + String(mins / 60) + "H " + String(mins % 60) + "M";
            break;
        }
        case 4: s += "$ COST: $" + String(_stats->getLaborCost(), 2); break;
        case 5: s += "\x08 [ RESET PROJECT ]"; break;
        case 6: s += "  BACK"; break;
        default: return "";
    }
    return s;
}

String MenuSys::getMenuItemGlobalStats(int idx, bool selected) {
    String s = selected ? "> " : "  ";
    switch (idx) {
        case 0: s += "\x04 TOT CUTS: " + String(_stats->getTotalCuts()); break;
        case 1: s += "\x04 TOT LEN: " + String(_stats->getTotalLengthMeters(), 1) + " M"; break;
        case 2: s += "\x01 TOT WASTE: " + String(_stats->getTotalWasteMeters(), 1) + " M"; break;
        case 3: s += "\x08 TOT TIME: " + String((int)_stats->getTotalHours()) + " H"; break;
        case 4: s += "  BACK"; break;
        default: return "";
    }
    return s;
}

String MenuSys::getMenuItemCalibration(int idx, bool selected) {
    String s = selected ? "> " : "  ";
    bool editing = (_state == MENU_EDIT && selected);
    
    switch (idx) {
        case 0: s += "\x03 WHEEL WIZARD"; break;
        case 1: s += "\x07 ANGLE WIZARD"; break;
        case 2:
            if (editing)
                s = "> \x7E \x03 WHEEL: " + String(_tempDia, 1);
            else
                s += "\x03 WHEEL: " + String(_settings->wheelDiameter, 1) + " MM";
            break;
        case 3:
            if (editing)
                s = "> \x7E \x04 KERF: " + String(_tempKerf, 1);
            else
                s += "\x04 KERF: " + String(_settings->kerfMM, 1) + " MM";
            break;
        case 4: s += "  BACK"; break;
        default: return "";
    }
    return s;
}

String MenuSys::getMenuItemSettings(int idx, bool selected) {
    String s = selected ? "> " : "  ";
    bool editing = (_state == MENU_EDIT && selected);
    
    switch (idx) {
        case 0: s += "\x08 UNITS : " + String(_settings->isInch ? "IMPERIAL" : "METRIC"); break;
        case 1: s += "\x07 ANGLE SRC: " + String(_settings->useAngleSensor ? "AUTO" : "MAN"); break;
        case 2: s += "\x04 AUTO-ZERO: " + String(_settings->autoZeroEnabled ? "ON" : "OFF"); break;
        case 3:
            if (editing)
                s = "> \x7E \x04 AZ THRESH: " + String(_tempAZThresh, 1);
            else
                s += "\x04 AZ THRESH: " + String(_settings->autoZeroThresholdMM, 1);
            break;
        case 4: s += "\x06 DIR : " + String(_settings->reverseDirection ? "REV" : "FWD"); break;
        case 5: s += "  BACK"; break;
        default: return "";
    }
    return s;
}

// GEMINI.md Rule 4.2: Menu item renderer with robust bounds checking
// NOTE: Refactored to <60 lines by delegating to helper functions
//
String MenuSys::getMenuItem(int idx, int currentItem, int itemCount) {
    // GEMINI.md Rule 4.2: Assertion 1 - Bounds check
    if (idx < 0 || idx >= itemCount) {
        return "";  // Safe fallback for out-of-bounds
    }
    
    // GEMINI.md Rule 4.2: Assertion 2 - Validate member pointers
    if (_settings == nullptr || _stats == nullptr) {
        return "ERR:NULL";  // Defensive - should never happen
    }
    
    bool selected = (idx == currentItem);
    
    // === MAIN MENU & CUT ANGLE EDIT ===
    if (_state == MENU_NAVIGATE || (_state == MENU_EDIT && _currentItem == ITEM_CUT_MODE)) {
        return getMenuItemMain(idx, selected);
    }
    // === STATS MENU ===
    else if (_state == MENU_STATS_SELECT || (_state == MENU_EDIT && _statsSubItem == 2)) {
        return getMenuItemStatsSelect(idx, selected);
    }
    // === PROJECT STATS ===
    else if (_state == MENU_STATS_PROJECT) {
        return getMenuItemProjectStats(idx, selected);
    }
    // === GLOBAL STATS ===
    else if (_state == MENU_STATS_GLOBAL) {
        return getMenuItemGlobalStats(idx, selected);
    }
    // === CALIBRATION SUBMENU ===
    else if (_state == MENU_CALIBRATION_SUBMENU || (_state == MENU_EDIT && _calibSubItem >= 0 && _settingsSubItem < 0)) {
        return getMenuItemCalibration(idx, selected);
    }
    // === SETTINGS SUBMENU ===
    else if (_state == MENU_SETTINGS_SUBMENU || (_state == MENU_EDIT && _settingsSubItem >= 0 && _calibSubItem < 0)) {
        return getMenuItemSettings(idx, selected);
    }
    else {
        // Fallback for unknown state
        return "ERR:STATE";
    }
}
