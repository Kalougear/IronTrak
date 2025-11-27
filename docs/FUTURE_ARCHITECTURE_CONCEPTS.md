# FUTURE ARCHITECTURE CONCEPTS

> **Status:** PROPOSAL ONLY — NOT IMPLEMENTED  
> **Created:** 2025-11-27  
> **Purpose:** Preserve architectural analysis for split-system design (Desktop Companion + Embedded LCD)  
> **Current State:** IronTrak operates as monolithic embedded system with all features on LCD

---

## 1. ARCHITECTURAL VISION

### 1.1 Core Principle

**Desktop = Planning Brain | LCD = Shop Floor Eyes & Hands**

The 20×4 LCD with rotary encoder interface is optimized for **execution**, not **data entry**. Complex planning tasks (weight calculations, cut list optimization, job management) should be offloaded to a desktop companion application where keyboard/mouse input and graphical displays excel.

### 1.2 Design Philosophy Alignment

This split architecture aligns with GEMINI.md principles:

- **Mechanical Sympathy:** Operator keeps eyes on workpiece, not menu navigation
- **Reliability First:** Simpler embedded logic = fewer failure modes
- **No Over-Engineering:** LCD does only what it must (measure, zero, log)
- **Production Ready:** Desktop app handles user-facing complexity

---

## 2. FEATURE SEGREGATION MATRIX

### 2.1 PLANNING DOMAIN (Desktop App)

**Characteristics:** Heavy computation, complex input, pre-work preparation

| Feature                         | Current Location | Proposed Location       | Migration Rationale                                                  |
| ------------------------------- | ---------------- | ----------------------- | -------------------------------------------------------------------- |
| **Weight Calculator**           | N/A (planned)    | Desktop Only            | Multi-field input (profile, material, length) slow on rotary encoder |
| **Cut List Optimizer**          | N/A (planned)    | Desktop Only            | Requires editing, reordering, complex bin-packing algorithm          |
| **Job Database**                | N/A              | Desktop Only            | String entry (project names, client info) impractical on LCD         |
| **Material Library Management** | N/A              | Desktop Only            | Adding custom profiles/densities requires extensive data entry       |
| **Historical Analysis**         | Partial (Stats)  | Desktop Only            | Charts, graphs, CSV export require graphical display                 |
| **Calibration Wizard**          | LCD (MenuSys)    | Desktop (Initial Setup) | One-time setup with precise numeric input better via keyboard        |
| **Settings Backup/Restore**     | N/A              | Desktop Only            | File I/O, version management not critical on device                  |
| **Hourly Rate Management**      | LCD (MenuSys)    | Desktop Only            | Job costing belongs in planning phase                                |

**Implementation:** Python/PyQt desktop app, Electron web app, or browser-based tool

**Data Flow:** Desktop → USB Serial (JSON) → STM32 EEPROM/Flash

---

### 2.2 EXECUTION DOMAIN (LCD Device)

**Characteristics:** Real-time feedback, tactile control, mission-critical reliability

| Feature                   | Current Status     | Priority     | GEMINI.md Alignment                          |
| ------------------------- | ------------------ | ------------ | -------------------------------------------- |
| **Live DRO (Big Number)** | ✅ Implemented     | **CRITICAL** | Core identity — real-time measurement        |
| **Zero/Reset**            | ✅ Implemented     | **CRITICAL** | Instant operator response (safety)           |
| **Cut Registration**      | ✅ Implemented     | **CRITICAL** | One-click logging, stats sync to PC later    |
| **Target Display**        | ✅ Implemented     | **HIGH**     | Visual feedback for angle cuts               |
| **Auto-Zero**             | ✅ Implemented     | **HIGH**     | Hands-free workflow (reduces operator error) |
| **Job Selection**         | ❌ Not Implemented | **MEDIUM**   | Load pre-planned cut sequence from PC        |
| **Unit Toggle**           | ✅ Implemented     | **MEDIUM**   | Quick mm ↔ inch swap (settings menu)         |
| **Reverse Direction**     | ✅ Implemented     | **LOW**      | Physical mount orientation fix               |
| **Angle Sensor Mode**     | ✅ Implemented     | **LOW**      | Auto vs. Manual angle selection              |

**Implementation:** Maintain current embedded C++ architecture (Arduino framework, PlatformIO)

**Data Flow:** STM32 → USB Serial (JSON) → Desktop (log sync, stats aggregation)

---

## 3. DATA SYNCHRONIZATION PROTOCOL

### 3.1 Job Upload (PC → LCD)

**Transport:** USB Serial (JSON over UART at 115200 baud)

**Payload Example:**

```json
{
  "command": "LOAD_JOB",
  "job_id": 123,
  "name": "Shelf Brackets",
  "stock": "40x40x3mm Angle (Mild Steel)",
  "cuts": [
    {
      "sequence": 1,
      "length_mm": 450.0,
      "angle_deg": 0,
      "quantity": 4,
      "notes": "Vertical supports"
    },
    {
      "sequence": 2,
      "length_mm": 300.0,
      "angle_deg": 45,
      "quantity": 8,
      "notes": "Diagonal braces"
    }
  ],
  "total_cuts": 12
}
```

**LCD Response:**

```json
{
  "status": "OK",
  "job_id": 123,
  "loaded": true
}
```

**Error Handling:**

- Validate job_id (uint16_t range)
- Validate cut count ≤ MAX_JOB_CUTS (e.g., 50)
- Reject if memory allocation would exceed static buffer

---

### 3.2 Statistics Sync (LCD → PC)

**Trigger:** On demand (user clicks "Sync to PC" in desktop app) or automatic (end of shift)

**Payload Example:**

```json
{
  "command": "STATS_REPORT",
  "job_id": 123,
  "cuts_completed": [
    {
      "sequence": 1,
      "target_mm": 450.0,
      "actual_mm": 450.2,
      "timestamp_ms": 14523600,
      "angle_deg": 0
    },
    {
      "sequence": 1,
      "target_mm": 450.0,
      "actual_mm": 449.8,
      "timestamp_ms": 14523620,
      "angle_deg": 0
    }
  ],
  "session_stats": {
    "total_cuts": 47,
    "total_length_mm": 18345.6,
    "runtime_seconds": 8640,
    "start_timestamp": 14515000,
    "end_timestamp": 14523640
  }
}
```

**Desktop Processing:**

- Update job completion status (e.g., "Cut 8/12")
- Analyze cut accuracy (target vs. actual, show histogram)
- Generate waste report
- Update material inventory (if tracked)
- Export to CSV for invoicing

---

### 3.3 Calibration Transfer (PC → LCD)

**Use Case:** Initial setup or recalibration via desktop wizard

**Payload Example:**

```json
{
  "command": "SET_CALIBRATION",
  "wheel_diameter_mm": 62.85,
  "pulses_per_rev": 4096,
  "kerf_mm": 1.5,
  "reverse_direction": false
}
```

**LCD Response:**

```json
{
  "status": "OK",
  "values_applied": true,
  "saved_to_eeprom": true
}
```

---

## 4. PROPOSED LCD MENU STRUCTURE (SIMPLIFIED)

### 4.1 Current Menu (Complex, Monolithic)

```
Main Menu
├── Statistics (view only)
│   ├── Cuts: 47
│   ├── Length: 18.3m
│   └── Runtime: 2.4h
├── Settings
│   ├── Units (Metric/Imperial)
│   ├── Hourly Rate (numeric entry) ← REMOVE
│   ├── Auto Zero
│   │   ├── Enable/Disable
│   │   └── Threshold (numeric entry) ← SIMPLIFY
│   └── Advanced
│       ├── Reverse Direction
│       └── Angle Sensor (Auto/Manual)
└── Calibration ← MOVE TO PC
    ├── Auto Wizard (multi-step)
    └── Manual (numeric entry)
```

**Issues:**

- Hourly rate entry is slow (rotary encoder)
- Calibration wizard requires precision input
- Auto-zero threshold adjustment is rare (set-and-forget)

---

### 4.2 Proposed Menu (Execution-Focused)

```
Main Menu
├── Jobs 📋
│   ├── Current: "Shelf Brackets" (view only)
│   ├── Progress: Cut 3/12
│   └── [Sync to PC] (upload stats)
├── Stats 📊 (view only)
│   ├── Session Cuts: 47
│   ├── Session Length: 18.3m
│   └── Session Runtime: 2.4h
├── Quick Settings ⚙️
│   ├── Units: [MM] / IN (toggle)
│   ├── Reverse: [OFF] / ON (toggle)
│   └── Auto-Zero: [ON] / OFF (toggle)
└── Advanced 🔧
    └── Wheel Diameter: 62.85mm (view only, PC-managed)
```

**Key Changes:**

- **Removed:** Hourly rate (moved to desktop job costing)
- **Removed:** Complex calibration wizard (PC handles via USB upload)
- **Removed:** Numeric entry fields (except emergency override)
- **Added:** Job progress display ("Cut 3/12")
- **Added:** Manual sync trigger (if auto-sync disabled)
- **Simplified:** All settings are toggles, not numeric spinners

---

## 5. MEMORY IMPACT ANALYSIS

### 5.1 Current Monolithic System

| Component               | Flash (KB) | RAM (Bytes) | Notes                                  |
| ----------------------- | ---------- | ----------- | -------------------------------------- |
| Core measurement logic  | ~50        | ~2048       | Encoder, display, state machine        |
| Menu system             | ~25        | ~1024       | Navigation, settings, calibration      |
| Statistics tracking     | ~10        | ~512        | Cut count, length, time                |
| Stock profile libraries | ~5         | 0           | `PROGMEM` arrays (angles, tubes, etc.) |
| **Current Total**       | **~90 KB** | **~3.6 KB** | Leaves ~360KB Flash, ~124KB RAM free   |

---

### 5.2 Split-System Embedded (LCD Only)

| Component                     | Flash (KB) | RAM (Bytes) | Delta                     |
| ----------------------------- | ---------- | ----------- | ------------------------- |
| Core measurement logic        | ~50        | ~2048       | (unchanged)               |
| Simplified menu               | ~15        | ~512        | **-10 KB, -512 bytes**    |
| Statistics tracking           | ~10        | ~512        | (unchanged)               |
| **Job management** (new)      | ~5         | ~1024       | **+5 KB, +1024 bytes**    |
| **USB serial protocol** (new) | ~8         | ~512        | **+8 KB, +512 bytes**     |
| Stock profiles                | 0          | 0           | **-5 KB** (moved to PC)   |
| **Split Total**               | **~88 KB** | **~4.6 KB** | Net: -2KB Flash, +1KB RAM |

**Conclusion:** Split architecture is **memory-neutral** while significantly improving UX.

---

### 5.3 Desktop Companion App

**Zero impact on embedded resources** (runs on PC)

**Estimated Desktop App Size:**

- Python + PyQt: ~50MB installed (includes interpreter)
- Electron + React: ~150MB (Chromium overhead)
- Web app (local server): ~5MB (Python Flask + vanilla JS)

**Recommended Stack:** Python + PyQt for native performance and small footprint

---

## 6. IMPLEMENTATION ROADMAP (FUTURE)

### 6.1 Phase 1: Protocol Foundation (Week 1-2)

**Embedded Side:**

1. Add USB serial command parser (`ProtocolSys.cpp/.h`)
2. Define message schemas (job upload, stats sync, calibration)
3. Implement `LOAD_JOB` command (store in static struct)
4. Add `SYNC_STATS` command (serialize to JSON)
5. Implement acknowledgment/error responses

**Desktop Side:**

1. Create Python project structure (`irontrak_companion/`)
2. Build USB serial communication layer (`SerialInterface.py`)
3. Implement job editor UI (PyQt table widget)
4. Add "Send to Device" button (serialize to JSON, send via USB)
5. Add "Sync Stats" button (request + deserialize response)

**Test Plan:**

- Loopback test (desktop → device → desktop echo)
- Job upload with 5 cuts, verify LCD displays "Job: Test | Cut 0/5"
- Register 2 cuts on LCD, sync to desktop, verify count matches

---

### 6.2 Phase 2: Desktop Features (Week 3-4)

**Desktop App Additions:**

1. **Weight Calculator:**

   - Dropdown: Profile type (angle, tube, bar)
   - Dropdown: Standard size or custom dimensions
   - Dropdown: Material (mild steel, aluminum, etc.)
   - Input: Length (mm or inches)
   - Output: Weight in kg/lbs

2. **Cut List Optimizer:**

   - Table: Add/edit/delete cuts (length, quantity)
   - Input: Stock length (default 6000mm)
   - Button: "Optimize" (run greedy bin-packing)
   - Output: Layout plan with waste percentage

3. **Job Manager:**
   - Create new job (name, client, notes)
   - Add cuts from optimizer
   - Save to SQLite database
   - Export to device or print cut sheet

**Test Plan:**

- Validate weight calculation accuracy (compare to online calculators)
- Verify cut list optimizer minimizes waste (compare to manual layout)
- Send 10-cut job to device, execute all cuts, sync back, verify accuracy

---

### 6.3 Phase 3: Advanced Integration (Week 5-6)

**Features:**

1. **Auto-sync on USB connection:**

   - Desktop app detects device insertion (via USB VID/PID)
   - Auto-loads pending job if queued
   - Auto-syncs stats on device removal (end of shift)

2. **Historical analysis:**

   - Charts: Cuts per day, total length over time
   - Reports: Material usage by job, waste trends
   - Export: CSV for invoicing, PDF job summaries

3. **Material inventory (optional):**
   - Track stock on hand (e.g., "3× 6m angle bars in inventory")
   - Deduct consumed material on job completion
   - Alert when stock low

**Test Plan:**

- Run 5 jobs over simulated workweek, verify charts render correctly
- Export job summary PDF, verify all data present
- Test inventory deduction (start with 10 bars, complete job, verify 8 remaining)

---

## 7. MIGRATION STRATEGY

### 7.1 Backward Compatibility

**Requirement:** Existing IronTrak units must function **without** desktop app.

**Implementation:**

- Embedded firmware supports **both** standalone and connected modes
- If no job loaded, LCD operates in current "freeform" mode (measure, zero, log)
- If job loaded, LCD shows progress ("Cut 3/12")
- Desktop app is **optional enhancement**, not requirement

---

### 7.2 Gradual Rollout

**Step 1: Add Protocol (No UX Change)**

- Implement USB serial parser on embedded side
- Desktop app can query stats, but LCD UX unchanged
- User continues using device as before

**Step 2: Add Job Display**

- If job uploaded, show progress bar
- If no job, fallback to current display
- User can opt-in to job mode when ready

**Step 3: Simplify Menu**

- Remove hourly rate, calibration wizard
- Add "only editable via PC" message
- User transitions to desktop app for these tasks

**Step 4: Full Transition**

- Desktop app becomes primary configuration tool
- LCD becomes pure execution interface
- User fully adopts split workflow

---

## 8. TECHNICAL CONSTRAINTS (GEMINI.md COMPLIANCE)

### 8.1 Embedded Side (STM32F411CE)

**Rule Compliance:**

- ✅ **Zero dynamic allocation:** Job struct is static array (`JobCut[MAX_CUTS]`)
- ✅ **Bounded loops:** JSON parser has max token count limit
- ✅ **No blocking I/O:** USB serial uses interrupt-driven DMA (if available)
- ✅ **ISR discipline:** Protocol parsing runs in main loop, not ISR

**Memory Safety:**

```cpp
// GEMINI.md Rule 4.2: Static allocation only
#define MAX_JOB_CUTS 50
struct JobProfile {
    uint16_t job_id;
    char name[32];             // Fixed-size string
    uint8_t total_cuts;
    uint8_t cuts_completed;
    JobCut cuts[MAX_JOB_CUTS]; // Static array, no malloc
};

static JobProfile activeJob;  // Global static, not heap
```

**Validation:**

```cpp
bool loadJob(const char* json) {
    // GEMINI.md Rule 3.1: Parameter validation
    if (json == nullptr) return false;

    // Parse and validate cut count
    uint8_t cutCount = parseJobCutCount(json);
    if (cutCount == 0 || cutCount > MAX_JOB_CUTS) {
        return false;  // Explicit failure
    }

    // Load into static struct (no allocation)
    activeJob.total_cuts = cutCount;
    // ... parse remaining fields ...

    return true;
}
```

---

### 8.2 Desktop Side

**No constraints** — standard desktop development practices apply.

**Recommended Libraries:**

- **PyQt6** or **PySide6** (GUI framework)
- **pyserial** (USB serial communication)
- **sqlite3** (job database, built-in to Python)
- **matplotlib** (charting for analytics)
- **reportlab** (PDF export)

---

## 9. BENEFITS SUMMARY

### 9.1 For the Operator

| Benefit                 | Current (Monolithic)            | Future (Split)          |
| ----------------------- | ------------------------------- | ----------------------- |
| **Job Setup Time**      | 5-10 min (rotary encoder entry) | 30 sec (load from PC)   |
| **Cut List Planning**   | Manual paper notes              | Automated optimizer     |
| **Weight Estimation**   | External calculator/chart       | Instant in-app lookup   |
| **Historical Analysis** | None                            | Charts, reports, trends |
| **Error Recovery**      | Re-enter all data               | Reload job from PC      |

---

### 9.2 For the Codebase

| Benefit                | Impact                                                    |
| ---------------------- | --------------------------------------------------------- |
| **Reduced Complexity** | Simpler menu logic, fewer edge cases                      |
| **Testability**        | Desktop UI testable via unit tests (PyTest)               |
| **Maintainability**    | Separation of concerns (planning vs. execution)           |
| **Extensibility**      | New calculators added to desktop without firmware updates |
| **Reliability**        | Fewer lines of embedded code = fewer failure modes        |

---

### 9.3 For Scalability

**Multi-Device Support:**

- One desktop app controls multiple IronTrak units (via USB hub)
- Centralized job database for shop with 3-5 bandsaws
- Fleet management dashboard (which saw is running which job)

**Cloud Sync (Optional Future):**

- Desktop app uploads job history to cloud
- Access reports from mobile device
- Multi-location shops share material library

---

## 10. RISKS & MITIGATIONS

### 10.1 USB Connectivity Dependency

**Risk:** If USB cable disconnects mid-job, is data lost?

**Mitigation:**

- Embedded device stores active job in **EEPROM** on upload
- Stats buffered locally, synced opportunistically
- Device continues operating in standalone mode if disconnected
- Desktop app prompts to retry sync on reconnect

---

### 10.2 Desktop App Adoption Barrier

**Risk:** User doesn't want to run desktop app, prefers pure embedded.

**Mitigation:**

- **Maintain backward compatibility:** Device works without PC
- **Gradual onboarding:** Desktop app optional at first
- **Clear value prop:** Show time savings (5 min → 30 sec job setup)

---

### 10.3 Protocol Version Mismatch

**Risk:** Desktop app v2.0 sends commands incompatible with firmware v1.0.

**Mitigation:**

- Include `protocol_version` field in all JSON messages
- Desktop app queries firmware version on connect
- Display warning if incompatible ("Update firmware to v1.5+")
- Maintain backward compatibility for 2 major versions

---

## 11. DECISION CRITERIA (IMPLEMENTATION TRIGGER)

**Implement split architecture when:**

1. ✅ User confirms need for cut list optimizer or weight calculator
2. ✅ User willing to use desktop app alongside embedded device
3. ✅ Current embedded features are **stable and tested** (no regressions)
4. ✅ USB serial protocol specification is finalized and documented
5. ❌ **DO NOT IMPLEMENT** if user prefers pure standalone embedded system

**Current Status:** PROPOSAL ONLY — awaiting user decision.

---

## 12. REFERENCES

- **GEMINI.md** — System instructions for IronTrak development
- **STM32_RULES_STRICT.md** — Embedded coding standards (NASA Power of 10 + MISRA-C)
- **STM32_FIRMWARE_ARCH_PATTERNS.md** — Architectural patterns for STM32 systems
- **TESTING_STANDARDS.md** — Testing protocols for hardware-in-loop verification

---

**End of Document**

> **Next Steps:** IF user approves split architecture, create `docs/USB_SERIAL_PROTOCOL_SPEC.md` with formal message schemas and state machine diagrams.
