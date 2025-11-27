# IronTrak API Reference

**Complete class and method documentation for developers**

---

## 📋 Table of Contents

- [EncoderSys](#encodersys)
- [DisplaySys](#displaysys)
- [UserInput](#userinput)
- [MenuSys](#menusys)
- [StatsSys](#statssys)
- [AngleSensor](#anglesensor)
- [Storage](#storage)
- [Helper Functions](#helper-functions)

---

## EncoderSys

**File:** `src/source/EncoderSys.cpp`, `src/headers/EncoderSys.h`

**Purpose:** Hardware encoder interface for precision position tracking

### Public Methods

#### `void init()`
Initialize TIM4 in encoder mode for quadrature capture.

**Parameters:** None  
**Returns:** void  
**Usage:**
```cpp
EncoderSys encoderSys;
encoderSys.init();  // Call once in setup()
```

**Details:**
- Configures TIM4 in `ENCODER_MODE_TI12` (both edges, both channels)
- Sets counter range to 16-bit (0-65535)
- Enables hardware quadrature decoding (4x resolution)

---

#### `void update()`
Update internal state (currently no-op, reserved for future filtering).

**Parameters:** None  
**Returns:** void  
**Usage:**
```cpp
void loop() {
    encoderSys.update();  // Call every loop iteration
}
```

---

#### `float getDistanceMM()`
Get current position in millimeters.

**Parameters:** None  
**Returns:** `float` - Distance in mm (can be negative)  
**Usage:**
```cpp
float position = encoderSys.getDistanceMM();
Serial.print("Position: ");
Serial.print(position);
Serial.println(" mm");
```

**Formula:**
```
distance = (rawCount × wheelDiameter × π / pulsesPerRev + offset) × direction
```

---

#### `void reset()`
Zero the encoder position (sets offset to cancel current reading).

**Parameters:** None  
**Returns:** void  
**Usage:**
```cpp
// Zero measurement after button click
encoderSys.reset();
```

---

#### `void setOffset(float offsetMM)`
Set position offset (for angle compensation).

**Parameters:**
- `offsetMM` - Offset value in mm

**Returns:** void  
**Usage:**
```cpp
// Apply angle offset after cut registration
float angleOffset = faceHeight * tan(angleRadians);
encoderSys.setOffset(angleOffset);
```

---

#### `void setWheelDiameter(float diameterMM)`
Set encoder wheel diameter for calibration.

**Parameters:**
- `diameterMM` - Wheel diameter in mm (typically 50.0)

**Returns:** void  
**Usage:**
```cpp
settings.wheelDiameter = 52.3;  // From calibration
encoderSys.setWheelDiameter(settings.wheelDiameter);
```

---

#### `long getRawCount()`
Get raw TIM4 counter value (for diagnostics/calibration).

**Parameters:** None  
**Returns:** `long` - Raw pulse count  
**Usage:**
```cpp
long pulses = encoderSys.getRawCount();
// Use for calibration calculations
```

---

## DisplaySys

**File:** `src/source/DisplaySys.cpp`, `src/headers/DisplaySys.h`

**Purpose:** 20x4 LCD management with flicker-free rendering

### Public Methods

#### `void init()`
Initialize I2C LCD and custom characters.

**Parameters:** None  
**Returns:** void  
**Usage:**
```cpp
DisplaySys displaySys;
displaySys.init();  // Must be first I2C device initialized
```

**Side Effects:**
- Calls `Wire.begin()` to start I2C bus
- Creates 8 custom characters (icons + big number segments)
- Clears display

---

#### `void update()`
Refresh display (currently no-op, reserved for animations).

**Parameters:** None  
**Returns:** void  

---

#### `void clear()`
Force full display clear and cache reset.

**Parameters:** None  
**Returns:** void  
**Usage:**
```cpp
// Before switching display modes
displaySys.clear();
displaySys.showMenu(...);
```

---

#### `void showIdle(params...)`
Display main measurement screen.

**Parameters:**
- `float currentMM` - Current position
- `float targetMM` - Target dimension (0 if no target)
- `uint8_t cutMode` - Angle (0-90)
- `uint8_t stockType` - 0=Rect, 1=Angle, 2=Cylinder
- `const char* stockStr` - Stock label (e.g., "20x40")
- `uint8_t faceVal` - Face dimension (mm)
- `bool isInch` - Unit system
- `bool reverseDir` - Direction indicator

**Returns:** void  
**Usage:**
```cpp
float current = encoderSys.getDistanceMM();
displaySys.showIdle(current, 100.0, 45, 0, "20x40", 20, false, false);
```

**Layout:**
```
┌────────────────────┐
│  123.4 CM       ↑  │  Big number + direction arrow
│                    │
│ 45° 20x40 F:20mm   │  Angle, stock, face
│ ◄──────●───────►   │  Target progress bar
└────────────────────┘
```

---

#### `void showMenu(const char* title, String value, bool isEditMode)`
Display single-line menu item.

**Parameters:**
- `title` - Menu item name
- `value` - Current value
- `isEditMode` - true = highlight for editing

**Returns:** void  
**Usage:**
```cpp
displaySys.showMenu("Wheel Diameter", "50.0 mm", true);
```

---

#### `void showMenu4(String l0, String l1, String l2, String l3)`
Display full 4-line custom menu.

**Parameters:**
- `l0`, `l1`, `l2`, `l3` - Line contents

**Returns:** void  
**Usage:**
```cpp
displaySys.showMenu4(
    "Calibration Wizard",
    "Move fence 100mm",
    "Press CLICK when",
    "done measuring"
);
```

---

#### `void showHiddenInfo(...)`
Display diagnostic screen (kerf, diameter, flags).

**Usage:**
```cpp
// Triggered by 10s button hold
displaySys.showHiddenInfo(settings.kerfMM, settings.wheelDiameter, 
                          settings.reverseDirection, settings.autoZeroEnabled);
```

---

## UserInput

**File:** `src/source/UserInput.cpp`, `src/headers/UserInput.h`

**Purpose:** Debounced rotary encoder and button event system

### Public Methods

#### `void init()`
Initialize GPIO pins for KY-040 encoder.

**Parameters:** None  
**Returns:** void  

---

#### `void isrTick()`
**CALL FROM ISR ONLY** - 1kHz timer callback for debouncing.

**Parameters:** None  
**Returns:** void  
**Usage:**
```cpp
void Timer1_Callback() {
    userInput.isrTick();  // Called by TIM3 at 1kHz
}
```

---

#### `InputEvent getEvent()`
Poll for user input events (non-blocking).

**Parameters:** None  
**Returns:** `InputEvent` enum  
**Usage:**
```cpp
void loop() {
    InputEvent event = userInput.getEvent();
    
    switch (event) {
        case EVENT_CLICK:
            // Single button press
            break;
        case EVENT_LONG_PRESS:
            // 500ms hold
            break;
        case EVENT_CW:
            // Clockwise rotation
            break;
        case EVENT_CCW:
            // Counter-clockwise
 break;
    }
}
```

**Event Types:**
```cpp
enum InputEvent {
    EVENT_NONE,             // No activity
    EVENT_CW,               // Rotate clockwise
    EVENT_CCW,              // Rotate counter-clockwise
    EVENT_CLICK,            // Button click
    EVENT_LONG_PRESS,       // 500ms hold
    EVENT_SUPER_LONG_PRESS, // 10s hold
    EVENT_NEXT,             // Semantic: increase/down
    EVENT_PREV              // Semantic: decrease/up
};
```

---

#### `InputEvent toSemanticEvent(InputEvent raw)`
Convert hardware events to UI-semantic events.

**Parameters:**
- `raw` - Hardware event (CW/CCW)

**Returns:** `InputEvent` - Semantic event (NEXT/PREV)  
**Usage:**
```cpp
InputEvent event = userInput.getEvent();
InputEvent semantic = toSemanticEvent(event);
// CCW → NEXT (increase value in menu)
// CW → PREV (decrease value)
```

---

## MenuSys

**File:** `src/source/MenuSys.cpp`, `src/headers/MenuSys.h`

**Purpose:** Hierarchical menu navigation and settings management

### Public Methods

#### `void init(SystemSettings* settings, StatsSys* stats, AngleSensor* sensor)`
Initialize menu system with pointers to global data.

**Parameters:**
- `settings` - Pointer to SystemSettings struct
- `stats` - Pointer to StatsSys instance
- `sensor` - Pointer to AngleSensor instance

**Returns:** void  
**Usage:**
```cpp
menuSys.init(&settings, &statsSys, &angleSensor);
```

---

#### `bool update(InputEvent event, DisplaySys* display, EncoderSys* encoder)`
Process user input and update menu state.

**Parameters:**
- `event` - Input event from UserInput
- `display` - Pointer to DisplaySys for rendering
- `encoder` - Pointer to EncoderSys (for calibration)

**Returns:** `bool` - true = stay in menu, false = exit to idle  
**Usage:**
```cpp
case STATE_MENU:
    if (!menuSys.update(semanticEvent, &displaySys, &encoderSys)) {
        currentState = STATE_IDLE;  // User exited menu
    }
    break;
```

---

## StatsSys

**File:** `src/source/StatsSys.cpp`, `src/headers/StatsSys.h`

**Purpose:** Cut statistics and time tracking

### Public Methods

#### `void init(SystemSettings* settings)`
Initialize stats system.

**Parameters:**
- `settings` - Pointer to SystemSettings (for reading stats)

**Returns:** void  

---

#### `void update()`
Update time tracking (call every loop).

**Parameters:** None  
**Returns:** void  

---

#### `void registerCut(float lengthMM)`
Record a cut event.

**Parameters:**
- `lengthMM` - Length of material cut

**Returns:** void  
**Usage:**
```cpp
// On button click
float currentPos = encoderSys.getDistanceMM();
statsSys.registerCut(currentPos);
```

**Side Effects:**
- Increments `settings.totalCuts` and `settings.projectCuts`
- Adds to `settings.totalLengthMeters` and `settings.projectLengthMeters`

---

#### `void resetProject()`
Clear project statistics (keep lifetime stats).

**Parameters:** None  
**Returns:** void  

---

## AngleSensor

**File:** `src/source/AngleSensor.cpp`, `src/headers/AngleSensor.h`

**Purpose:** AS5600 magnetic angle sensor interface

### Public Methods

#### `bool init()`
Detect and initialize AS5600 sensor.

**Parameters:** None  
**Returns:** `bool` - true if sensor found, false otherwise  
**Usage:**
```cpp
if (settings.useAngleSensor) {
    if (!angleSensor.init()) {
        settings.useAngleSensor = false;  // Sensor not present
    }
}
```

---

#### `float getAngleDegrees()`
Read current angle in degrees.

**Parameters:** None  
**Returns:** `float` - Angle (0.0 - 360.0)  
**Usage:**
```cpp
if (settings.useAngleSensor) {
    float angle = angleSensor.getAngleDegrees();
    uint8_t newMode = (uint8_t)(angle + 0.5);  // Round to integer
    settings.cutMode = newMode;
}
```

---

#### `uint16_t getRawAngle()`
Read raw 12-bit sensor value.

**Parameters:** None  
**Returns:** `uint16_t` - Raw value (0-4095)  

---

## Storage

**File:** `src/headers/Storage.h`

**Purpose:** SystemSettings struct definition

### SystemSettings Struct

```cpp
struct SystemSettings {
    // Calibration
    float wheelDiameter;        // Encoder wheel diameter (mm)
    bool isInch;                // false=Metric, true=Imperial
    bool reverseDirection;      // Encoder direction flag
    
    // Statistics
    unsigned long totalCuts;
    float totalLengthMeters;
    unsigned long projectCuts;
    float projectLengthMeters;
    unsigned long totalSeconds;
    unsigned long projectSeconds;
    
    // Cutting Parameters
    float kerfMM;               // Blade thickness
    bool autoZeroEnabled;
    float autoZeroThresholdMM;
    uint8_t cutMode;            // Angle (0-90)
    uint8_t stockType;          // 0=Rect, 1=Angle, 2=Cyl
    uint8_t stockIdx;           // Index into stock library
    uint8_t faceIdx;            // Face selection (0 or 1)
    
    // Angle Sensor
    uint8_t lastAngle;          // Previously set angle
    bool useAngleSensor;
    
    // Economic
    float hourlyRate;           // $/hour
};
```

---

## Helper Functions

**File:** `src/main.cpp`

### `float getTargetMM()`
Calculate target dimension based on stock and angle.

**Parameters:** None (uses global `settings`)  
**Returns:** `float` - Target in mm, or 0 if no target  

**Usage:**
```cpp
float target = getTargetMM();
displaySys.showIdle(current, target, ...);
```

---

### `const char* getStockString()`
Get display string for current stock selection.

**Parameters:** None  
**Returns:** `const char*` - Stock label (e.g., "20x40")  

---

### `uint8_t getFaceValue()`
Get dimension of selected face (rectangular stock only).

**Parameters:** None  
**Returns:** `uint8_t` - Face dimension in mm  

---

**Last Updated:** 2025-11-25  
**Firmware Version:** v1.0.0
