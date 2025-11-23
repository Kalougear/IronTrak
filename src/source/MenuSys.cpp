#include "headers/MenuSys.h"

void MenuSys::init(SystemSettings* settings, StatsSys* stats) {
    _settings = settings;
    _stats = stats;
    _state = MENU_NAVIGATE;
    _currentItem = 0;
    _scrollOffset = 0;
    _statsPage = 0;
    _statsScrollOffset = 0;
    _statsSubItem = 0;
    _calibSubItem = 0;
    _calibScrollOffset = 0;
    _calibStep = 0;
    _stockPage = 0;
    _needsRedraw = true;
    _exitRequest = false;
    _lastActivityTime = millis();
}

bool MenuSys::update(InputEvent e, DisplaySys* display, EncoderSys* encoder) {
    if (e != EVENT_NONE) {
        _lastActivityTime = millis();
    } else if (millis() - _lastActivityTime > 15000) {
        _exitRequest = true;
        return false;
    }
    
    if (_state == MENU_NAVIGATE) {
        handleNavigation(e);
    } else if (_state == MENU_STATS_SELECT) {
        handleStatsSelect(e);
    } else if (_state == MENU_STATS_PROJECT) {
        handleStatsProject(e);
    } else if (_state == MENU_STATS_GLOBAL) {
        handleStatsGlobal(e);
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
    
    return !_exitRequest;
}

void MenuSys::handleNavigation(InputEvent e) {
    if (e == EVENT_NEXT) {
        _currentItem++;
        if (_currentItem >= ITEM_COUNT) _currentItem = 0;
        _needsRedraw = true;
    } else if (e == EVENT_PREV) {
        _currentItem--;
        if (_currentItem < 0) _currentItem = ITEM_COUNT - 1;
        _needsRedraw = true;
    }
    
    if (_currentItem < _scrollOffset) _scrollOffset = _currentItem;
    else if (_currentItem >= _scrollOffset + 3) _scrollOffset = _currentItem - 2;
    
    if (e == EVENT_CLICK) {
        if (_currentItem == ITEM_EXIT) {
            _exitRequest = true;
        } else if (_currentItem == ITEM_STATS) {
            _state = MENU_STATS_SELECT;
            _statsSubItem = 0;
            _statsScrollOffset = 0;
        } else if (_currentItem == ITEM_CALIB) {
            _state = MENU_CALIB_SUBMENU;
            _calibSubItem = 0;
            _calibScrollOffset = 0;
        } else if (_currentItem == ITEM_STOCK) {
            _state = MENU_STOCK_SELECT;
            _stockPage = 0;
        } else if (_currentItem == ITEM_CUT_MODE) {
            _state = MENU_EDIT;
            _tempCutMode = _settings->cutMode;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleStatsSelect(InputEvent e) {
    // 0: Project Stats, 1: Global Stats, 2: Hourly Rate, 3: Back
    if (e == EVENT_NEXT) {
        _statsSubItem++;
        if (_statsSubItem > 3) _statsSubItem = 0;
        _needsRedraw = true;
    } else if (e == EVENT_PREV) {
        _statsSubItem--;
        if (_statsSubItem < 0) _statsSubItem = 3;
        _needsRedraw = true;
    }
    
    if (_statsSubItem < _statsScrollOffset) _statsScrollOffset = _statsSubItem;
    else if (_statsSubItem >= _statsScrollOffset + 3) _statsScrollOffset = _statsSubItem - 2;
    
    if (e == EVENT_CLICK) {
        if (_statsSubItem == 0) {
            _state = MENU_STATS_PROJECT;
            _statsPage = 0;
            _statsScrollOffset = 0;
        } else if (_statsSubItem == 1) {
            _state = MENU_STATS_GLOBAL;
            _statsPage = 0;
            _statsScrollOffset = 0;
        } else if (_statsSubItem == 2) {
            _state = MENU_EDIT;
            _tempRate = _settings->hourlyRate;
        } else if (_statsSubItem == 3) {
            _state = MENU_NAVIGATE;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleStatsProject(InputEvent e) {
    // 0: Cuts, 1: Len, 2: Waste, 3: Time, 4: Cost, 5: Reset, 6: Back
    if (e == EVENT_NEXT) {
        _statsPage++;
        if (_statsPage > 6) _statsPage = 0;
        _needsRedraw = true;
    } else if (e == EVENT_PREV) {
        _statsPage--;
        if (_statsPage < 0) _statsPage = 6;
        _needsRedraw = true;
    }
    
    if (_statsPage < _statsScrollOffset) _statsScrollOffset = _statsPage;
    else if (_statsPage >= _statsScrollOffset + 3) _statsScrollOffset = _statsPage - 2;
    
    if (e == EVENT_CLICK) {
        if (_statsPage == 5) { // Reset
            _stats->resetProject();
            _state = MENU_STATS_SELECT;
        } else if (_statsPage == 6) { // Back
            _state = MENU_STATS_SELECT;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleStatsGlobal(InputEvent e) {
    // 0: Cuts, 1: Len, 2: Waste, 3: Time, 4: Back
    if (e == EVENT_NEXT) {
        _statsPage++;
        if (_statsPage > 4) _statsPage = 0;
        _needsRedraw = true;
    } else if (e == EVENT_PREV) {
        _statsPage--;
        if (_statsPage < 0) _statsPage = 4;
        _needsRedraw = true;
    }
    
    if (_statsPage < _statsScrollOffset) _statsScrollOffset = _statsPage;
    else if (_statsPage >= _statsScrollOffset + 3) _statsScrollOffset = _statsPage - 2;
    
    if (e == EVENT_CLICK) {
        if (_statsPage == 4) { // Back
            _state = MENU_STATS_SELECT;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleCalibSubmenu(InputEvent e) {
    if (e == EVENT_NEXT) {
        _calibSubItem++;
        if (_calibSubItem > 7) _calibSubItem = 0;
        _needsRedraw = true;
    } else if (e == EVENT_PREV) {
        _calibSubItem--;
        if (_calibSubItem < 0) _calibSubItem = 7;
        _needsRedraw = true;
    }
    
    if (_calibSubItem < _calibScrollOffset) _calibScrollOffset = _calibSubItem;
    else if (_calibSubItem >= _calibScrollOffset + 3) _calibScrollOffset = _calibSubItem - 2;
    
    if (e == EVENT_CLICK) {
       if (_calibSubItem == 0) {
            _state = MENU_EDIT;
            _tempDia = _settings->wheelDiameter;
        } else if (_calibSubItem == 1) {
            _state = MENU_EDIT;
            _tempKerf = _settings->kerfMM;
        } else if (_calibSubItem == 2) {
            _settings->autoZeroEnabled = !_settings->autoZeroEnabled;
        } else if (_calibSubItem == 3) {
            _state = MENU_EDIT;
            _tempAZThresh = _settings->autoZeroThresholdMM;
        } else if (_calibSubItem == 4) {
            _settings->isInch = !_settings->isInch;
        } else if (_calibSubItem == 5) {
            _settings->reverseDirection = !_settings->reverseDirection;
        } else if (_calibSubItem == 6) {
            _state = MENU_AUTO_CALIB;
            _calibStep = 0;
        } else if (_calibSubItem == 7) {
            _state = MENU_NAVIGATE;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleAutoCalib(InputEvent e, EncoderSys* encoder) {
    if (_calibStep == 0) {
        if (e == EVENT_CLICK) {
            encoder->reset();
            _calibStep = 1;
            _needsRedraw = true;
        }
    } else if (_calibStep == 1) {
        if (e == EVENT_CLICK) {
            _calibPulses = encoder->getRawCount();
            _calibRealLen = encoder->getDistanceMM();
            _calibStep = 2;
            _needsRedraw = true;
        }
    } else if (_calibStep == 2) {
        if (e == EVENT_NEXT) { _calibRealLen += 0.5; _needsRedraw = true; }
        else if (e == EVENT_PREV) { _calibRealLen -= 0.5; _needsRedraw = true; }
        else if (e == EVENT_CLICK) { _calibStep = 3; _needsRedraw = true; }
    } else if (_calibStep == 3) {
        if (e == EVENT_CLICK) {
            if (_calibPulses != 0) {
                float mmPerPulse = abs(_calibRealLen) / (float)abs(_calibPulses);
                float newCirc = mmPerPulse * PULSES_PER_REV;
                float newDia = newCirc / PI;
                
                _settings->wheelDiameter = newDia;
                encoder->setWheelDiameter(newDia);
            }
            _state = MENU_CALIB_SUBMENU;
            _needsRedraw = true;
        }
    }
}

void MenuSys::handleStockSelect(InputEvent e) {
    if (_stockPage == 0) {
        if (e == EVENT_NEXT) {
            _settings->stockType++;
            if (_settings->stockType > 2) _settings->stockType = 0;
        } else if (e == EVENT_PREV) {
            if (_settings->stockType == 0) _settings->stockType = 2;
            else _settings->stockType--;
        } else if (e == EVENT_CLICK) {
            _stockPage = 1;
            _settings->stockIdx = 0;
        }
        _needsRedraw = true;
    } else if (_stockPage == 1) {
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
        
        if (e == EVENT_NEXT) {
            _settings->stockIdx++;
            if (_settings->stockIdx > maxIdx) _settings->stockIdx = 0;
        } else if (e == EVENT_PREV) {
            if (_settings->stockIdx == 0) _settings->stockIdx = maxIdx;
            else _settings->stockIdx--;
        } else if (e == EVENT_CLICK) {
            _state = MENU_NAVIGATE;
        }
        _needsRedraw = true;
    }
}

void MenuSys::handleEdit(InputEvent e) {
    if (_currentItem == ITEM_CUT_MODE && _state == MENU_EDIT) {
        if (e == EVENT_NEXT) {
            if (_tempCutMode < 45) _tempCutMode++;
            _needsRedraw = true;
        } else if (e == EVENT_PREV) {
            if (_tempCutMode > 0) _tempCutMode--;
            _needsRedraw = true;
        } else if (e == EVENT_CLICK) {
            _settings->cutMode = _tempCutMode;
            _state = MENU_NAVIGATE;
            _needsRedraw = true;
        }
    }
    else if (_state == MENU_EDIT && _state != MENU_CALIB_SUBMENU && _statsSubItem == 2) {
        // Editing Hourly Rate
        if (e == EVENT_NEXT) _tempRate += 1.0;
        else if (e == EVENT_PREV) _tempRate -= 1.0;
        else if (e == EVENT_CLICK) {
            _settings->hourlyRate = max(0.0f, _tempRate);
            _state = MENU_STATS_SELECT;
            _needsRedraw = true;
        }
        _needsRedraw = true;
    }
    else if (_calibSubItem >= 0 && _calibSubItem <= 3 && _state == MENU_EDIT) {
        if (_calibSubItem == 0) {
            if (e == EVENT_NEXT) _tempDia += 0.01;
            else if (e == EVENT_PREV) _tempDia -= 0.01;
            else if (e == EVENT_CLICK) {
                _settings->wheelDiameter = _tempDia;
                _state = MENU_CALIB_SUBMENU;
            }
            _needsRedraw = true;
        } else if (_calibSubItem == 1) {
            if (e == EVENT_NEXT) _tempKerf += 0.1;
            else if (e == EVENT_PREV) _tempKerf -= 0.1;
            else if (e == EVENT_CLICK) {
                _settings->kerfMM = max(0.0f, _tempKerf);
                _state = MENU_CALIB_SUBMENU;
            }
            _needsRedraw = true;
        } else if (_calibSubItem == 3) {
            if (e == EVENT_NEXT) _tempAZThresh += 0.5;
            else if (e == EVENT_PREV) _tempAZThresh -= 0.5;
            else if (e == EVENT_CLICK) {
                _settings->autoZeroThresholdMM = constrain(_tempAZThresh, 2.0, 20.0);
                _state = MENU_CALIB_SUBMENU;
            }
            _needsRedraw = true;
        }
    }
}

void MenuSys::render(DisplaySys* display) {
    auto header = [](String s) -> String {
        s = " " + s + " ";
        int len = s.length();
        if (len >= 20) return s.substring(0, 20);
        int padLeft = (20 - len) / 2;
        String res = "";
        for (int i = 0; i < padLeft; i++) res += "=";
        res += s;
        while (res.length() < 20) res += "=";
        return res;
    };
    
    auto center = [](String s) -> String {
        int len = s.length();
        if (len >= 20) return s.substring(0, 20);
        int pad = (20 - len) / 2;
        String res = "";
        for (int i = 0; i < pad; i++) res += " ";
        res += s;
        while (res.length() < 20) res += " ";
        return res;
    };

    if (_state == MENU_AUTO_CALIB) {
        String l0, l1, l2, l3;
        if (_calibStep == 0) {
            l0 = header("AUTO CALIB");
            l1 = center("STEP 1: ZERO");
            l2 = "";
            l3 = center("CLICK TO START");
        } else if (_calibStep == 1) {
            l0 = header("MEASURE");
            l1 = center("CUT KNOWN LENGTH");
            l2 = "CURRENT: " + String(_stats->getProjectLengthMeters() * 1000, 1) + " MM";
            l3 = center("CLICK WHEN DONE");
        } else if (_calibStep == 2) {
            l0 = header("ENTER ACTUAL");
            l1 = "MEASURED: " + String(_calibRealLen, 1) + " MM";
            l2 = center("TURN TO ADJUST");
            l3 = center("CLICK TO SAVE");
        } else {
            float mmPerPulse = abs(_calibRealLen) / (float)abs(_calibPulses);
            float newDia = (mmPerPulse * PULSES_PER_REV) / PI;
            l0 = header("RESULT");
            l1 = "NEW DIAMETER:";
            l2 = "  " + String(newDia, 3) + " MM";
            l3 = center("CLICK TO SAVE");
        }
        display->showMenu4(l0, l1, l2, l3);
        return;
    }
    
    if (_state == MENU_STOCK_SELECT) {
        String l0, l1, l2, l3;
        if (_stockPage == 0) {
            const char* types[] = {"RECTANGULAR", "ANGLE IRON", "CYLINDRICAL"};
            const char icons[] = {1, 2, 3};
            l0 = header("STOCK TYPE");
            auto fmt = [&](int i) -> String {
                String s = (i == _settings->stockType) ? "> " : "  ";
                s += String(icons[i]) + " " + String(types[i]);
                return s;
            };
            l1 = fmt(0); l2 = fmt(1); l3 = fmt(2);
        } else if (_stockPage == 1) {
            l0 = header("STOCK SIZE");
            int count = 0;
            if (_settings->isInch) {
                if (_settings->stockType == 0) count = STOCK_RECT_IN_COUNT;
                else if (_settings->stockType == 1) count = STOCK_ANGLE_IN_COUNT;
                else count = STOCK_CYL_IN_COUNT;
            } else {
                if (_settings->stockType == 0) count = STOCK_RECT_MM_COUNT;
                else if (_settings->stockType == 1) count = STOCK_ANGLE_MM_COUNT;
                else count = STOCK_CYL_MM_COUNT;
            }
            auto getStr = [&](int idx) -> String {
                if (idx < 0 || idx >= count) return "";
                String s = "";
                if (_settings->isInch) {
                    if (_settings->stockType == 0) s = STOCK_RECT_IN[idx];
                    else if (_settings->stockType == 1) s = STOCK_ANGLE_IN[idx];
                    else s = STOCK_CYL_IN[idx];
                } else {
                    if (_settings->stockType == 0) s = STOCK_RECT_MM[idx];
                    else if (_settings->stockType == 1) s = STOCK_ANGLE_MM[idx];
                    else s = STOCK_CYL_MM[idx];
                }
                char icon = (_settings->stockType == 0) ? 1 : (_settings->stockType == 1 ? 2 : 3);
                return String(icon) + " " + s;
            };
            int current = _settings->stockIdx;
            int startIdx = current - 1;
            if (startIdx < 0) startIdx = 0;
            if (startIdx > count - 3) startIdx = count - 3;
            if (startIdx < 0) startIdx = 0;
            auto fmtLine = [&](int row) -> String {
                int idx = startIdx + row;
                if (idx >= count) return "";
                String s = (idx == current) ? "> " : "  ";
                s += getStr(idx);
                return s;
            };
            l1 = fmtLine(0); l2 = fmtLine(1); l3 = fmtLine(2);
        } else {
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
    
    if (_state == MENU_NAVIGATE || (_state == MENU_EDIT && _currentItem == ITEM_CUT_MODE)) {
        l0 = header("MAIN MENU");
        currentItem = _currentItem;
        scrollOffset = _scrollOffset;
        itemCount = ITEM_COUNT;
    } else if (_state == MENU_STATS_SELECT || (_state == MENU_EDIT && _statsSubItem == 2)) {
        l0 = header("STATISTICS");
        currentItem = _statsSubItem;
        scrollOffset = _statsScrollOffset;
        itemCount = 4;
    } else if (_state == MENU_STATS_PROJECT) {
        l0 = header("PROJECT STATS");
        currentItem = _statsPage;
        scrollOffset = _statsScrollOffset;
        itemCount = 7;
    } else if (_state == MENU_STATS_GLOBAL) {
        l0 = header("GLOBAL STATS");
        currentItem = _statsPage;
        scrollOffset = _statsScrollOffset;
        itemCount = 5;
    } else if (_state == MENU_CALIB_SUBMENU || (_state == MENU_EDIT && _calibSubItem >= 0)) {
        l0 = header("SYSTEM SETUP");
        currentItem = _calibSubItem;
        scrollOffset = _calibScrollOffset;
        itemCount = 8;
    }

    auto renderItem = [&](int idx) -> String {
        if (idx >= itemCount) return "";
        bool selected = (idx == currentItem);
        String s = selected ? "> " : "  ";
        
        if (_state == MENU_NAVIGATE || (_state == MENU_EDIT && _currentItem == ITEM_CUT_MODE)) {
            const char* items[] = {"STOCK PROFILE", "CUT ANGLE", "STATISTICS", "SYSTEM SETUP", "EXIT MENU"};
            const char icons[] = {1, 6, 5, 4, 7};
            
            s += String(icons[idx]) + " " + items[idx];
            
            if (idx == ITEM_CUT_MODE) {
                bool editing = (_state == MENU_EDIT && selected);
                uint8_t mode = editing ? _tempCutMode : _settings->cutMode;
                s = selected ? "> " : "  ";
                s += String(icons[idx]) + " ANGLE: ";
                if (editing) s += "\x7E";
                s += String(mode) + "\xDF";
            }
        } 
        else if (_state == MENU_STATS_SELECT || (_state == MENU_EDIT && _statsSubItem == 2)) {
            bool editing = (_state == MENU_EDIT && selected);
            if (idx == 0) s += "PROJECT STATS";
            else if (idx == 1) s += "GLOBAL STATS";
            else if (idx == 2) {
                s += "RATE: $";
                if (editing) s = "> \x7E RATE: $" + String(_tempRate, 2);
                else s += String(_settings->hourlyRate, 2) + "/HR";
            }
            else if (idx == 3) s += "BACK";
        }
        else if (_state == MENU_STATS_PROJECT) {
            if (idx == 0) s += "CUTS: " + String(_stats->getProjectCuts());
            else if (idx == 1) s += "LEN: " + String(_stats->getProjectLengthMeters(), 1) + " M";
            else if (idx == 2) s += "WASTE: " + String(_stats->getProjectWasteMeters(), 2) + " M";
            else if (idx == 3) {
                unsigned long mins = _stats->getUptimeMinutes();
                s += "TIME: " + String(mins / 60) + "H " + String(mins % 60) + "M";
            }
            else if (idx == 4) s += "COST: $" + String(_stats->getLaborCost(), 2);
            else if (idx == 5) s += "[ RESET PROJECT ]";
            else if (idx == 6) s += "BACK";
        }
        else if (_state == MENU_STATS_GLOBAL) {
            if (idx == 0) s += "TOT CUTS: " + String(_stats->getTotalCuts());
            else if (idx == 1) s += "TOT LEN: " + String(_stats->getTotalLengthMeters(), 1) + " M";
            else if (idx == 2) s += "TOT WASTE: " + String(_stats->getTotalWasteMeters(), 1) + " M";
            else if (idx == 3) s += "TOT TIME: " + String((int)_stats->getTotalHours()) + " H";
            else if (idx == 4) s += "BACK";
        }
        else if (_state == MENU_CALIB_SUBMENU || _state == MENU_EDIT) {
            bool editing = (_state == MENU_EDIT && selected);
            
            if (idx == 0) {
                s += "WHEEL: " + String(editing ? _tempDia : _settings->wheelDiameter, 2) + " MM";
                if (editing) s = "> \x7E WHEEL: " + String(_tempDia, 2);
            }
            else if (idx == 1) {
                s += "KERF: " + String(editing ? _tempKerf : _settings->kerfMM, 1) + " MM";
                if (editing) s = "> \x7E KERF: " + String(_tempKerf, 1);
            }
            else if (idx == 2) s += "AUTO-ZERO: " + String(_settings->autoZeroEnabled ? "ON" : "OFF");
            else if (idx == 3) {
                s += "AZ THRESH: " + String(editing ? _tempAZThresh : _settings->autoZeroThresholdMM, 1);
                if (editing) s = "> \x7E AZ THR: " + String(_tempAZThresh, 1);
            }
            else if (idx == 4) s += "UNITS: " + String(_settings->isInch ? "IMPERIAL" : "METRIC");
            else if (idx == 5) s += "DIR: " + String(_settings->reverseDirection ? "REV" : "FWD");
            else if (idx == 6) s += "CALIB WIZARD";
            else if (idx == 7) s += "BACK";
        }
        return s;
    };

    l1 = renderItem(scrollOffset);
    l2 = renderItem(scrollOffset + 1);
    l3 = renderItem(scrollOffset + 2);
    
    display->showMenu4(l0, l1, l2, l3);
}
