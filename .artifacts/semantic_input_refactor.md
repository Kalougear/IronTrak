# Semantic Input Layer - Implementation Plan

## Objective
Eliminate encoder direction inconsistencies by creating a semantic abstraction layer that maps hardware rotation events to UI-level semantic actions.

## Problem Statement
Currently, every menu handler independently interprets EVENT_CW and EVENT_CCW, leading to:
- Inconsistent behavior across different screens
- Repeated bugs when adding new menu items
- No central place to define "what direction means what"

## Solution Architecture

### 1. Add Semantic Events to UserInput.h
```cpp
enum InputEvent {
    EVENT_NONE,
    EVENT_CW,           // Raw hardware: Clockwise rotation
    EVENT_CCW,          // Raw hardware: Counter-clockwise rotation
    EVENT_CLICK,
    EVENT_LONG_PRESS,
    // Semantic UI events (use these in menu handlers)
    EVENT_NEXT,         // UI: Move forward/down in list, increase value
    EVENT_PREV          // UI: Move backward/up in list, decrease value
};
```

### 2. Add Translation Function to UserInput Class
Add a static method that converts raw hardware events to semantic events:
```cpp
static InputEvent toSemantic(InputEvent raw) {
    // Current convention: CCW = Next/Increase, CW = Prev/Decrease
    if (raw == EVENT_CCW) return EVENT_NEXT;
    if (raw == EVENT_CW) return EVENT_PREV;
    return raw;  // CLICK, LONG_PRESS, NONE pass through unchanged
}
```

### 3. Update MenuSys.cpp to Use Semantic Events
Replace all instances of:
- `EVENT_CCW` → `EVENT_NEXT`
- `EVENT_CW` → `EVENT_PREV`

Affected handlers:
- `handleNavigation()` - Main menu scrolling
- `handleStats()` - Stats page navigation
- `handleCalibSubmenu()` - Calibration menu scrolling
- `handleAutoCalib()` - Calibration wizard value adjustment
- `handleStockSelect()` - Stock type/size/face selection
- `handleEdit()` - All edit modes (blade angle, wheel diameter, kerf, threshold)

### 4. Update main.cpp
Convert raw input to semantic before passing to menu:
```cpp
InputEvent rawEvent = input.poll();
InputEvent semanticEvent = UserInput::toSemantic(rawEvent);
menu.update(semanticEvent, &display, &encoder);
```

## Benefits
✅ **Single source of truth** for rotation direction semantics  
✅ **Consistent behavior** across entire UI  
✅ **Easy to change** direction preference globally if needed  
✅ **Self-documenting** code - EVENT_NEXT is clearer than EVENT_CCW  
✅ **FORTRESS MODE compliant** - reliability through consistency  

## Testing Checklist
After implementation, verify:
- [ ] Main menu scrolling direction is correct
- [ ] Blade angle editing direction is correct
- [ ] Stats page navigation direction is correct
- [ ] Calibration submenu scrolling direction is correct
- [ ] Calibration value editing (wheel, kerf, threshold) direction is correct
- [ ] Stock selection navigation direction is correct
- [ ] All values stay within bounds (no wrap-around bugs)

## Rollback Plan
If issues arise, the change is easily reversible:
1. Remove EVENT_NEXT/EVENT_PREV from enum
2. Revert toSemantic() addition
3. Restore original EVENT_CW/EVENT_CCW usage in handlers

## Estimated Impact
- **Files Modified:** 3 (UserInput.h, UserInput.cpp optional, MenuSys.cpp, main.cpp)
- **Lines Changed:** ~50 lines
- **Risk Level:** LOW (pure refactor, no logic changes)
- **Time Required:** 15-20 minutes
