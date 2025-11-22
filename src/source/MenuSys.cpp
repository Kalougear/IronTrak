#include "headers/MenuSys.h"

void MenuSys::init(SystemSettings* settings, StatsSys* stats) {
    _settings = settings;
    _stats = stats;
    _state = MENU_NAVIGATE;
    _currentItem = 0;
    _statsPage = 0;
    _calibSubItem = 0;
    _calibStep = 0;
    _stockPage = 0;
    _needsRedraw = true;
    _exitRequest = false;
}

bool MenuSys::update(InputEvent e, DisplaySys* display, EncoderSys* encoder) {
    if (_state == MENU_NAVIGATE) {
        handleNavigation(e);
    } else if (_state == MENU_STATS_VIEW) {
        handleStats(e);
    } else if (_state == MENU_CALIB_SUBMENU) {
        handleCalibSubmenu(e);
    } else if (_state == MENU_AUTO_CALIB) {
        handleAutoCalib(e, encoder);
    } else if (_state == MENU_STOCK_SELECT) {
        handleStockSelect(e);
    } else {
        handleEdit(e);
    }
    
    if (_needsRedraw) {
        render(display);
        _needsRedraw = false;
    }
    
    // Return false if user exited
    return !_exitRequest;
}

void MenuSys::handleNavigation(InputEvent e) {
    if (e == EVENT_CW) {
        _currentItem++;
        if (_currentItem >= ITEM_COUNT) _currentItem = 0;
        _needsRedraw = true;
    } else if (e == EVENT_CCW) {
        _currentItem--;
        if (_currentItem < 0) _currentItem = ITEM_COUNT - 1;
        _needsRedraw = true;
    } else if (e == EVENT_CLICK) {
        if (_currentItem == ITEM_EXIT) {
            // Exit menu
            // Storage::save(*_settings); // Save disabled to prevent freeze
            _exitRequest = true;
            return;
        } else if (_currentItem == ITEM_STATS) {
            _state = MENU_STATS_VIEW;
            _statsPage = 0;
        } else if (_currentItem == ITEM_CALIB) {
            _state = MENU_CALIB_SUBMENU;
            _calibSubItem = 0;
        } else if (_currentItem == ITEM_STOCK) {
            _state = MENU_STOCK_SELECT;
            _stockPage = 0;
        } else {
            _state = MENU_EDIT;
            // Initialize temp values
            if (_currentItem == ITEM_CUT_MODE) _tempCutMode = _settings->cutMode;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleStats(InputEvent e) {
    if (e == EVENT_CW) {
        _statsPage++;
        if (_statsPage > 4) _statsPage = 0;
        _needsRedraw = true;
    } else if (e == EVENT_CCW) {
        _statsPage--;
        if (_statsPage < 0) _statsPage = 4;
        _needsRedraw = true;
    } else if (e == EVENT_CLICK) {
        if (_statsPage == 4) {
            // Reset Project
            _stats->resetProject();
            _state = MENU_NAVIGATE;
        } else {
            // Exit stats
            _state = MENU_NAVIGATE;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleCalibSubmenu(InputEvent e) {
    if (e == EVENT_CW) {
        _calibSubItem++;
        if (_calibSubItem > 5) _calibSubItem = 0;
        _needsRedraw = true;
    } else if (e == EVENT_CCW) {
        _calibSubItem--;
        if (_calibSubItem < 0) _calibSubItem = 5;
        _needsRedraw = true;
    } else if (e == EVENT_CLICK) {
        if (_calibSubItem == 0) {
            // View/Edit Diameter
            _state = MENU_EDIT;
            _tempDia = _settings->wheelDiameter;
        } else if (_calibSubItem == 1) {
            // Edit Kerf
            _state = MENU_EDIT;
            _tempKerf = _settings->kerfMM;
        } else if (_calibSubItem == 2) {
            // Toggle Auto-Zero
            _settings->autoZeroEnabled = !_settings->autoZeroEnabled;
            Storage::save(*_settings);
        } else if (_calibSubItem == 3) {
            // Edit Auto-Zero Threshold
            _state = MENU_EDIT;
            _tempAZThresh = _settings->autoZeroThresholdMM;
        } else if (_calibSubItem == 4) {
            // Auto Calibration Wizard
            _state = MENU_AUTO_CALIB;
            _calibStep = 0;
        } else {
            // Back
            _state = MENU_NAVIGATE;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleAutoCalib(InputEvent e, EncoderSys* encoder) {
    if (_calibStep == 0) {
        // Step 0: Start (Click to Zero)
        if (e == EVENT_CLICK) {
            encoder->reset();
            _calibStep = 1;
            _needsRedraw = true;
        }
    } else if (_calibStep == 1) {
        // Step 1: Stop (Click to Capture)
        if (e == EVENT_CLICK) {
            _calibPulses = encoder->getRawCount();
            _calibRealLen = encoder->getDistanceMM();
            _calibStep = 2;
            _needsRedraw = true;
        }
    } else if (_calibStep == 2) {
        // Step 2: Input Actual Length
        if (e == EVENT_CW) { _calibRealLen += 0.5; _needsRedraw = true; }
        else if (e == EVENT_CCW) { _calibRealLen -= 0.5; _needsRedraw = true; }
        else if (e == EVENT_CLICK) { _calibStep = 3; _needsRedraw = true; }
    } else if (_calibStep == 3) {
        // Step 3: Confirm & Save
        if (e == EVENT_CLICK) {
            if (_calibPulses != 0) {
                float mmPerPulse = abs(_calibRealLen) / (float)abs(_calibPulses);
                float newCirc = mmPerPulse * PULSES_PER_REV;
                float newDia = newCirc / PI;
                
                _settings->wheelDiameter = newDia;
                Storage::save(*_settings);
                encoder->setWheelDiameter(newDia);
            }
            _state = MENU_CALIB_SUBMENU;
            _needsRedraw = true;
        }
    }
}

void MenuSys::handleStockSelect(InputEvent e) {
    if (_stockPage == 0) {
        // Page 0: Stock Type selection
        if (e == EVENT_CW) {
            _settings->stockType++;
            if (_settings->stockType > 2) _settings->stockType = 0;
        } else if (e == EVENT_CCW) {
            if (_settings->stockType == 0) _settings->stockType = 2;
            else _settings->stockType--;
        } else if (e == EVENT_CLICK) {
            _stockPage = 1;  // Move to size selection
            _settings->stockIdx = 0;  // Reset to first item
        }
        _needsRedraw = true;
    } else if (_stockPage == 1) {
        // Page 1: Stock Size selection
        uint8_t maxIdx = 0;
        if (_settings->isInch) {
            if (_settings->stockType == 0) maxIdx = STOCK_RECT_IN_COUNT - 1;
            else if (_settings->stockType == 1) maxIdx = STOCK_ANGLE_IN_COUNT - 1;
            else maxIdx = STOCK_CYL_IN_COUNT - 1;
        } else {
            if (_settings->stockType == 0) maxIdx = STOCK_RECT_MM_COUNT - 1;
            else if (_settings->stockType == 1) maxIdx = STOCK_ANGLE_MM_COUNT - 1;
            else maxIdx = STOCK_CYL_MM_COUNT - 1;
        }
        
        if (e == EVENT_CW) {
            _settings->stockIdx++;
            if (_settings->stockIdx > maxIdx) _settings->stockIdx = 0;
        } else if (e == EVENT_CCW) {
            if (_settings->stockIdx == 0) _settings->stockIdx = maxIdx;
            else _settings->stockIdx--;
        } else if (e == EVENT_CLICK) {
            if (_settings->stockType == 0) {  // Rectangular - needs face selection
                _stockPage = 2;
                _settings->faceIdx = 0;
            } else {
                // Angle/Cyl - done
                Storage::save(*_settings);
                _state = MENU_NAVIGATE;
            }
        }
        _needsRedraw = true;
    } else {
        // Page 2: Face selection (for rectangular only)
        if (e == EVENT_CW || e == EVENT_CCW) {
            _settings->faceIdx = (_settings->faceIdx == 0) ? 1 : 0;
        } else if (e == EVENT_CLICK) {
            Storage::save(*_settings);
            _state = MENU_NAVIGATE;
        }
    }
}

void MenuSys::handleEdit(InputEvent e) {
    if (_state == MENU_CALIB_SUBMENU) {
        // Editing calibration values
        if (_calibSubItem == 0) {
            // Edit Diameter
            if (e == EVENT_CW) _tempDia += 0.01;
            else if (e == EVENT_CCW) _tempDia -= 0.01;
            else if (e == EVENT_CLICK) {
                _settings->wheelDiameter = _tempDia;
                Storage::save(*_settings);
                _state = MENU_CALIB_SUBMENU;
            }
            _needsRedraw = true;
            _needsRedraw = true;
        } else if (_calibSubItem == 1) {
            // Edit Kerf
            if (e == EVENT_CW) _tempKerf += 0.1;
            else if (e == EVENT_CCW) _tempKerf -= 0.1;
            else if (e == EVENT_CLICK) {
                _settings->kerfMM = max(0.0f, _tempKerf);
                Storage::save(*_settings);
                _state = MENU_CALIB_SUBMENU;
            }
            _needsRedraw = true;
        } else if (_calibSubItem == 3) {
            // Edit Auto-Zero Threshold
            if (e == EVENT_CW) _tempAZThresh += 0.5;
            else if (e == EVENT_CCW) _tempAZThresh -= 0.5;
            else if (e == EVENT_CLICK) {
                _settings->autoZeroThresholdMM = constrain(_tempAZThresh, 2.0, 20.0);
                Storage::save(*_settings);
                _state = MENU_CALIB_SUBMENU;
            }
            _needsRedraw = true;
        }
    } else {
        // Main menu edits
        if (_currentItem == ITEM_UNITS) {
            _settings->isInch = !_settings->isInch;
            Storage::save(*_settings);
            _state = MENU_NAVIGATE;
            _needsRedraw = true;
        } else if (_currentItem == ITEM_DIRECTION) {
            _settings->reverseDirection = !_settings->reverseDirection;
            Storage::save(*_settings);
            _state = MENU_NAVIGATE;
            _needsRedraw = true;
        } else if (_currentItem == ITEM_CUT_MODE) {
            if (e == EVENT_CW || e == EVENT_CCW) {
                _tempCutMode = (_tempCutMode == 0) ? 1 : 0;  // Toggle 0° / 45°
                _needsRedraw = true;
            } else if (e == EVENT_CLICK) {
                _settings->cutMode = _tempCutMode;
                Storage::save(*_settings);
                _state = MENU_NAVIGATE;
                _needsRedraw = true;
            }
        }
    }
}

void MenuSys::render(DisplaySys* display) {
    String title = "";
    String value = "";
    
    // Stats rendering
    if (_state == MENU_STATS_VIEW) {
        if (_statsPage == 0) {
            title = "Proj: " + String(_stats->getProjectCuts()) + " cuts";
            value = String(_stats->getProjectLengthMeters(), 2) + " m";
        } else if (_statsPage == 1) {
            title = "Proj Waste";
            value = String(_stats->getProjectWasteMeters() * 1000, 1) + " mm";
        } else if (_statsPage == 2) {
            title = "Tot: " + String(_stats->getTotalCuts()) + " cuts";
            value = String(_stats->getTotalLengthMeters(), 2) + " m";
        } else if (_statsPage == 3) {
            title = "Tot Waste";
            value = String(_stats->getTotalWasteMeters() * 1000, 1) + " mm";
        } else {
            title = "Reset Project?";
            value = "[CLICK TO RST]";
        }
        display->showMenu(title.c_str(), value, false);
        return;
    }
    
    // Calib submenu rendering
    if (_state == MENU_CALIB_SUBMENU) {
        const char* items[] = {"Wheel Dia", "Blade Kerf", "Auto-Zero", "AZ Thresh", "Auto Calib", "Back"};
        title = String(_calibSubItem + 1) + ". " + items[_calibSubItem];
        
        if (_calibSubItem == 0) value = String(_settings->wheelDiameter, 2) + " mm";
        else if (_calibSubItem == 1) value = String(_settings->kerfMM, 1) + " mm";
        else if (_calibSubItem == 2) value = _settings->autoZeroEnabled ? "[ON]" : "[OFF]";
        else if (_calibSubItem == 3) value = String(_settings->autoZeroThresholdMM, 1) + " mm";
        else if (_calibSubItem == 4) value = "[START]";
        else value = "[CLICK]";
        
        display->showMenu(title.c_str(), value, _state == MENU_EDIT);
        return;
    }
    
    // Auto-calib wizard rendering
    if (_state == MENU_AUTO_CALIB) {
        if (_calibStep == 0) {
            title = "Step 1: Zero";
            value = "[CLICK TO START]";
        } else if (_calibStep == 1) {
            title = "Step 2: Cutting";
            value = "[CLICK TO STOP]";
        } else if (_calibStep == 2) {
            title = "Step 3: Actual";
            value = String(_calibRealLen, 1) + " mm";
        } else {
            float mmPerPulse = abs(_calibRealLen) / (float)abs(_calibPulses);
            float newDia = (mmPerPulse * PULSES_PER_REV) / PI;
            title = "NewD: " + String(newDia, 2);
            value = "[SAVE]";
        }
        display->showMenu(title.c_str(), value, _calibStep == 2);
        return;
    }
    
    // Stock selection rendering
    if (_state == MENU_STOCK_SELECT) {
        if (_stockPage == 0) {
            const char* types[] = {"Rect", "Angle", "Cyl"};
            title = "Stock Type";
            value = types[_settings->stockType];
        } else if (_stockPage == 1) {
            title = "Stock Size";
            if (_settings->isInch) {
                if (_settings->stockType == 0) value = STOCK_RECT_IN[_settings->stockIdx];
                else if (_settings->stockType == 1) value = STOCK_ANGLE_IN[_settings->stockIdx];
                else value = STOCK_CYL_IN[_settings->stockIdx];
            } else {
                if (_settings->stockType == 0) value = STOCK_RECT_MM[_settings->stockIdx];
                else if (_settings->stockType == 1) value = STOCK_ANGLE_MM[_settings->stockIdx];
                else value = STOCK_CYL_MM[_settings->stockIdx];
            }
        } else {
            title = "Cut Across";
            // Get dims for selected stock
            if (_settings->isInch) {
                float dim = STOCK_RECT_IN_DIMS[_settings->stockIdx][_settings->faceIdx];
                value = String(dim / 25.4, 2) + "\"";
            } else {
                uint8_t dim = STOCK_RECT_MM_DIMS[_settings->stockIdx][_settings->faceIdx];
                value = String(dim) + " mm";
            }
        }
        display->showMenu(title.c_str(), value, false);
        return;
    }
    
    // Main menu rendering
    const char* menuItems[] = {"Project Stats", "System Setup", "Blade Angle", "Material Profile", "Display Units", "Encoder Dir", "Return to Cut"};
    title = String(_currentItem + 1) + ". " + menuItems[_currentItem];
    
    if (_currentItem == ITEM_STATS) value = "View History >";
    else if (_currentItem == ITEM_CALIB) value = "Configure >";
    else if (_currentItem == ITEM_CUT_MODE) value = (_state == MENU_EDIT ? _tempCutMode : _settings->cutMode) == 0 ? "Straight (0 deg)" : "Miter (45 deg)";
    else if (_currentItem == ITEM_STOCK) value = "Select Size >";
    else if (_currentItem == ITEM_UNITS) value = _settings->isInch ? "Imperial (Inch)" : "Metric (mm)";
    else if (_currentItem == ITEM_DIRECTION) value = _settings->reverseDirection ? "Reversed" : "Normal";
    else if (_currentItem == ITEM_EXIT) value = "Save & Exit >";
    
    display->showMenu(title.c_str(), value, _state == MENU_EDIT && _currentItem == ITEM_CUT_MODE);
}
