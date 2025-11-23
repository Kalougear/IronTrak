#include "../headers/MenuSys.h"

MenuSys::MenuSys()
{
    // Constructor - Initialization handled in init()
}

void MenuSys::init(SystemSettings *settings, StatsSys *stats, AngleSensor *angleSensor)
{
    _settings = settings;
    _stats = stats;
    _angleSensor = angleSensor;

    // Reset menu state
    _state = MENU_NAVIGATE;
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
            _calibRealLen += 1.0;
            _needsRedraw = true;
        }
        else if (e == EVENT_PREV)
        {
            _calibRealLen -= 1.0;
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
            _tempRate += 1.0;
            _needsRedraw = true;
        }
        else if (e == EVENT_PREV)
        {
            _tempRate -= 1.0;
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
                _tempDia += 0.1;
            else if (e == EVENT_PREV)
                _tempDia -= 0.1;
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
                _tempKerf += 0.1;
            else if (e == EVENT_PREV)
                _tempKerf -= 0.1;
            else if (e == EVENT_CLICK)
            {
                _settings->kerfMM = max(0.0f, _tempKerf);
                _state = MENU_CALIBRATION_SUBMENU;
            }
            _needsRedraw = true;
        }
        else if (_calibSubItem == 3)
        {
            if (e == EVENT_NEXT)
                _tempAZThresh += 0.5;
            else if (e == EVENT_PREV)
                _tempAZThresh -= 0.5;
            else if (e == EVENT_CLICK)
            {
                _settings->autoZeroThresholdMM = constrain(_tempAZThresh, 2.0, 20.0);
                _state = MENU_CALIBRATION_SUBMENU;
            }
            _needsRedraw = true;
        }
    }
}

void MenuSys::render(DisplaySys *display)
{
    auto header = [](String s) -> String
    {
        s = " " + s + " ";
        int len = s.length();
        if (len >= 20)
            return s.substring(0, 20);
        int padLeft = (20 - len) / 2;
        String res = "";
        for (int i = 0; i < padLeft; i++)
            res += "=";
        res += s;
        while (res.length() < 20)
            res += "=";
        return res;
    };

    auto center = [](String s) -> String
    {
        int len = s.length();
        if (len >= 20)
            return s.substring(0, 20);
        int pad = (20 - len) / 2;
        String res = "";
        for (int i = 0; i < pad; i++)
            res += " ";
        res += s;
        while (res.length() < 20)
            res += " ";
        return res;
    };

    // Check for flashing warning message
    if (_warningEndTime > 0)
    {
        if (millis() < _warningEndTime)
        {
            display->showMenu4(
                header("! WARNING !"),
                center("CHANGE ANGLE SRC"),
                center("TO AUTO"),
                center("IN SETTINGS"));
            return;
        }
        else
        {
            _warningEndTime = 0;
        }
    }

    if (_state == MENU_AUTO_CALIB)
    {
        String l0, l1, l2, l3;
        if (_calibStep == 0)
        {
            // Step 1: Zero the encoder
            l0 = header("\x04 AUTO CALIB");
            l1 = center(" => STEP 1 <=");
            l2 = center("TOUCH BLADE AND ZERO");
            l3 = center("THEN CLICK");
        }
        else if (_calibStep == 1)
        {
            // Step 2: Cut a piece - no real-time display needed
            l0 = header("\x04 STEP 2: CUT");
            l1 = center("CUT A PIECE");
            l2 = center("~50 CM OR MORE");
            l3 = center("CLICK WHEN DONE");
        }
        else if (_calibStep == 2)
        {
            // Step 3: Enter the actual measured length - show in CM
            l0 = header("\x04 STEP 3: MEASURE");
            l1 = "\x7E REAL CUT: " + String(_calibRealLen / 10.0, 1) + " CM";
            l2 = center("TURN TO ADJUST");
            l3 = center("CLICK TO CONFIRM");
        }
        else
        {
            // Step 4: Show calculated wheel diameter - 1 space before icon
            float mmPerPulse = abs(_calibRealLen) / (float)abs(_calibPulses);
            float newDia = (mmPerPulse * PULSES_PER_REV) / PI;
            l0 = header("\x04 STEP 4: SAVE");
            l1 = " \x04 NEW WHEEL DIA:";
            l2 = " \x04 " + String(newDia, 3) + " MM";
            l3 = center("CLICK TO SAVE");
        }
        display->showMenu4(l0, l1, l2, l3);
        return;
    }

    // START NEW ANGLE WIZARD RENDERING
    if (_state == MENU_CALIB_ANGLE_0)
    {
        display->showMenu4(
            header("\x07 ANGLE WIZARD"),
            center("STEP 1: ZERO"),
            center("SET TO 0 DEGREES"),
            center("CLICK TO CAPTURE"));
        return;
    }
    else if (_state == MENU_CALIB_ANGLE_45)
    {
        display->showMenu4(
            header("\x07 ANGLE WIZARD"),
            center("STEP 2: 45 DEG"),
            center("SET TO 45 DEGREES"),
            center("CLICK TO SAVE"));
        return;
    }
    // END NEW ANGLE WIZARD RENDERING

    if (_state == MENU_STOCK_SELECT)
    {
        String l0, l1, l2, l3;
        if (_stockPage == 0)
        {
            const char *types[] = {"RECTANGULAR", "ANGLE IRON", "CYLINDRICAL"};
            const char icons[] = {1, 2, 3};
            l0 = header("STOCK TYPE");
            auto fmt = [&](int i) -> String
            {
                String s = (i == _settings->stockType) ? "> " : "  ";
                s += String(icons[i]) + " " + String(types[i]);
                return s;
            };
            l1 = fmt(0);
            l2 = fmt(1);
            l3 = fmt(2);
        }
        else if (_stockPage == 1)
        {
            l0 = header("STOCK SIZE");
            int count = 0;
            if (_settings->isInch)
            {
                if (_settings->stockType == 0)
                    count = STOCK_RECT_IN_COUNT;
                else if (_settings->stockType == 1)
                    count = STOCK_ANGLE_IN_COUNT;
                else
                    count = STOCK_CYL_IN_COUNT;
            }
            else
            {
                if (_settings->stockType == 0)
                    count = STOCK_RECT_MM_COUNT;
                else if (_settings->stockType == 1)
                    count = STOCK_ANGLE_MM_COUNT;
                else
                    count = STOCK_CYL_MM_COUNT;
            }
            auto getStr = [&](int idx) -> String
            {
                if (idx < 0 || idx >= count)
                    return "";
                String s = "";
                if (_settings->isInch)
                {
                    if (_settings->stockType == 0)
                        s = STOCK_RECT_IN[idx];
                    else if (_settings->stockType == 1)
                        s = STOCK_ANGLE_IN[idx];
                    else
                        s = STOCK_CYL_IN[idx];
                }
                else
                {
                    if (_settings->stockType == 0)
                        s = STOCK_RECT_MM[idx];
                    else if (_settings->stockType == 1)
                        s = STOCK_ANGLE_MM[idx];
                    else
                        s = STOCK_CYL_MM[idx];
                }
                char icon = (_settings->stockType == 0) ? 1 : (_settings->stockType == 1 ? 2 : 3);
                return String(icon) + " " + s;
            };
            int current = _settings->stockIdx;
            int startIdx = current - 1;
            if (startIdx < 0)
                startIdx = 0;
            if (startIdx > count - 3)
                startIdx = count - 3;
            if (startIdx < 0)
                startIdx = 0;
            auto fmtLine = [&](int row) -> String
            {
                int idx = startIdx + row;
                if (idx >= count)
                    return "";
                String s = (idx == current) ? "> " : "  ";
                s += getStr(idx);
                return s;
            };
            l1 = fmtLine(0);
            l2 = fmtLine(1);
            l3 = fmtLine(2);
        }
        else
        {
            l0 = header("FACE SELECT");
            l3 = center("TURN TO TOGGLE");
        }
        display->showMenu4(l0, l1, l2, l3);
        return;
    }

    String l0, l1, l2, l3;
    int currentItem = 0;
    int scrollOffset = 0;
    int itemCount = 0;

    if (_state == MENU_NAVIGATE || (_state == MENU_EDIT && _currentItem == ITEM_CUT_MODE))
    {
        l0 = header("MAIN MENU");
        currentItem = _currentItem;
        scrollOffset = _scrollOffset;
        itemCount = ITEM_COUNT;
    }
    else if (_state == MENU_STATS_SELECT || (_state == MENU_EDIT && _statsSubItem == 2))
    {
        l0 = header("STATISTICS");
        currentItem = _statsSubItem;
        scrollOffset = _statsScrollOffset;
        itemCount = 4;
    }
    else if (_state == MENU_STATS_PROJECT)
    {
        l0 = header("PROJECT STATS");
        currentItem = _statsPage;
        scrollOffset = _statsScrollOffset;
        itemCount = 7;
    }
    else if (_state == MENU_STATS_GLOBAL)
    {
        l0 = header("GLOBAL STATS");
        currentItem = _statsPage;
        scrollOffset = _statsScrollOffset;
        itemCount = 5;
    }
    else if (_state == MENU_CALIBRATION_SUBMENU || (_state == MENU_EDIT && _calibSubItem >= 0 && _settingsSubItem < 0))
    {
        l0 = header("CALIBRATION");
        currentItem = _calibSubItem;
        scrollOffset = _calibScrollOffset;
        itemCount = 5;
    }
    else if (_state == MENU_SETTINGS_SUBMENU || (_state == MENU_EDIT && _settingsSubItem >= 0 && _calibSubItem < 0))
    {
        l0 = header("SETTINGS");
        currentItem = _settingsSubItem;
        scrollOffset = _settingsScrollOffset;
        itemCount = 6;
    }

    auto renderItem = [&](int idx) -> String
    {
        if (idx >= itemCount)
            return "";
        bool selected = (idx == currentItem);
        String s = selected ? "> " : "  ";

        if (_state == MENU_NAVIGATE || (_state == MENU_EDIT && _currentItem == ITEM_CUT_MODE))
        {
            const char *items[] = {"STOCK PROFILE", "CUT ANGLE", "STATISTICS", "CALIBRATION", "SETTINGS", "EXIT MENU"};
            // Use dynamic icon for stock based on current selection: 1=Rect, 2=Angle, 3=Cyl
            char stockIcon = (_settings->stockType == 0) ? 1 : (_settings->stockType == 1 ? 2 : 3);
            // Icons: Stock(Dynamic), Angle(7), Stats(8), Calib(3-Phi), Settings(5-Units), Exit(Space)
            // Changed Calib to Phi (3) per request
            const char icons[] = {stockIcon, 7, 8, 3, 5, 32};

            s += String(icons[idx]) + " " + items[idx];

            if (idx == ITEM_CUT_MODE)
            {
                bool editing = (_state == MENU_EDIT && selected);
                uint8_t mode = editing ? _tempCutMode : _settings->cutMode;
                s = selected ? "> " : "  ";
                // Use Angle Symbol (7)

                if (_settings->useAngleSensor)
                {
                    // Auto Mode: Show simple text
                    s += "\x07 ANGLE IS AUTO";
                }
                else
                {
                    // Manual Mode: Normal display
                    s += "\x07 CUT ANGLE: ";
                    if (editing)
                        s += "\x7E";
                    s += String(mode) + "\xDF";
                }
            }
        }
        else if (_state == MENU_STATS_SELECT || (_state == MENU_EDIT && _statsSubItem == 2))
        {
            bool editing = (_state == MENU_EDIT && selected);
            if (idx == 0)
                s += "\x08 PROJECT STATS"; // Stats icon (Index 0 via \x08)
            else if (idx == 1)
                s += "\x08 GLOBAL STATS"; // Stats icon (Index 0 via \x08)
            else if (idx == 2)
            {
                s += "\x08 RATE: $"; // Stats icon + Dollar
                if (editing)
                    s = "> \x7E \x08 RATE: $" + String(_tempRate, 2);
                else
                    s += String(_settings->hourlyRate, 2) + "/HR";
            }
            else if (idx == 3)
                s += "  BACK";
        }
        else if (_state == MENU_STATS_PROJECT)
        {
            if (idx == 0)
                s += "\x04 CUTS: " + String(_stats->getProjectCuts()); // Blade icon (Index 4)
            else if (idx == 1)
                s += "\x04 LEN: " + String(_stats->getProjectLengthMeters(), 1) + " M"; // Blade icon
            else if (idx == 2)
                s += "\x01 WASTE: " + String(_stats->getProjectWasteMeters(), 2) + " M"; // Rect icon (Material)
            else if (idx == 3)
            {
                unsigned long mins = _stats->getUptimeMinutes();
                s += "\x08 TIME: " + String(mins / 60) + "H " + String(mins % 60) + "M"; // Stats icon
            }
            else if (idx == 4)
                s += "$ COST: $" + String(_stats->getLaborCost(), 2); // Dollar
            else if (idx == 5)
                s += "\x08 [ RESET PROJECT ]"; // Stats icon
            else if (idx == 6)
                s += "  BACK";
        }
        else if (_state == MENU_STATS_GLOBAL)
        {
            if (idx == 0)
                s += "\x04 TOT CUTS: " + String(_stats->getTotalCuts()); // Blade icon
            else if (idx == 1)
                s += "\x04 TOT LEN: " + String(_stats->getTotalLengthMeters(), 1) + " M"; // Blade icon
            else if (idx == 2)
                s += "\x01 TOT WASTE: " + String(_stats->getTotalWasteMeters(), 1) + " M"; // Rect icon
            else if (idx == 3)
                s += "\x08 TOT TIME: " + String((int)_stats->getTotalHours()) + " H"; // Stats icon
            else if (idx == 4)
                s += "  BACK";
        }
        else if (_state == MENU_CALIBRATION_SUBMENU || (_state == MENU_EDIT && _calibSubItem >= 0 && _settingsSubItem < 0))
        {
            // Calibration submenu rendering (5 items)
            bool editing = (_state == MENU_EDIT && selected);

            if (idx == 0)
            {
                // Wheel Wizard - use Phi icon (3)
                s += "\x03 WHEEL WIZARD";
            }
            else if (idx == 1)
            {
                // Angle Wizard - use Angle icon (7)
                s += "\x07 ANGLE WIZARD";
            }
            else if (idx == 2)
            {
                // Wheel Diameter (manual) - use Phi icon (3)
                if (editing)
                {
                    s = "> \x7E \x03 WHEEL: " + String(_tempDia, 1);
                }
                else
                {
                    s += "\x03 WHEEL: " + String(_settings->wheelDiameter, 1) + " MM";
                }
            }
            else if (idx == 3)
            {
                // Kerf - use Blade icon (4)
                if (editing)
                {
                    s = "> \x7E \x04 KERF: " + String(_tempKerf, 1);
                }
                else
                {
                    s += "\x04 KERF: " + String(_settings->kerfMM, 1) + " MM";
                }
            }
            else if (idx == 4)
            {
                // Back
                s += "  BACK";
            }
        }
        else if (_state == MENU_SETTINGS_SUBMENU || (_state == MENU_EDIT && _settingsSubItem >= 0 && _calibSubItem < 0))
        {
            // Settings submenu rendering (6 items)
            bool editing = (_state == MENU_EDIT && selected);

            if (idx == 0)
            {
                // Units - use Units icon (5)
                s += "\x05 UNITS: " + String(_settings->isInch ? "IMPERIAL" : "METRIC");
            }
            else if (idx == 1)
            {
                // Angle Source - use Angle icon (7)
                s += "\x07 ANGLE SRC: " + String(_settings->useAngleSensor ? "AUTO" : "MAN");
            }
            else if (idx == 2)
            {
                // Auto-Zero toggle - use Blade icon (4)
                s += "\x04 AUTO-ZERO: " + String(_settings->autoZeroEnabled ? "ON" : "OFF");
            }
            else if (idx == 3)
            {
                // Auto-Zero Threshold - use Blade icon (4)
                if (editing)
                {
                    s = "> \x7E \x04 AZ THRESH: " + String(_tempAZThresh, 1);
                }
                else
                {
                    s += "\x04 AZ THRESH: " + String(_settings->autoZeroThresholdMM, 1);
                }
            }
            else if (idx == 4)
            {
                // Direction - use Direction icon (6)
                s += "\x06 DIR: " + String(_settings->reverseDirection ? "REV" : "FWD");
            }
            else if (idx == 5)
            {
                // Back
                s += "  BACK";
            }
        }
        return s;
    };

    l1 = renderItem(scrollOffset);
    l2 = renderItem(scrollOffset + 1);
    l3 = renderItem(scrollOffset + 2);

    display->showMenu4(l0, l1, l2, l3);
}
