# STM32 EMBEDDED TESTING STANDARDS & INITIATOR

**Version:** 2.0 (Universal Template)  
**Last Updated:** 2025-11-27  
**Purpose:** Universal Testing Infrastructure Bootstrap for STM32 Embedded Projects

> [!IMPORTANT]
> This document serves as:
>
> 1. **Testing Initiator** - Bootstrap testing infrastructure for new STM32 projects
> 2. **AI Agent Cookbook** - Generate compliant tests via structured patterns
> 3. **Universal Standards** - Reusable across all STM32 embedded projects

---

## 0. Quick Start: New Project Setup (5 Minutes)

### Step 1: Create Directory Structure

```bash
cd {YOUR_PROJECT_ROOT}
mkdir -p test/native test/hardware/automated test/hardware/manual test/hardware/stress test/mocks
```

### Step 2: Configure PlatformIO

Add to `platformio.ini`:

```ini
[env:native]
platform = native
build_flags =
    -D NATIVE_BUILD
    -I test/mocks
    -I include
    -std=c++17
build_src_filter =
    +<*>
    -<main.cpp>
    -<Hardware/>  # Exclude hardware-specific code
test_filter = native/*

[env:your_board]
platform = ststm32
board = {your_stm32_board}  # e.g., nucleo_f446re, blackpill_f411ce
framework = arduino
test_port = COM6  # Update to your serial port
test_speed = 115200
test_filter = hardware/*
```

### Step 3: Install Unity Framework

Unity is included with PlatformIO by default. No additional installation needed.

### Step 4: Create Your First Test

Copy boilerplate from Section 11.2 and customize names.

### Step 5: Run Tests

```bash
# Native tests (x86, fast)
pio test -e native -f "native/test_your_module" -v

# Hardware tests (on-target)
pio test -e your_board -f "hardware/automated/test_your_module" -v
```

---

## 1. Testing Philosophy & Architecture

### 1.1 Core Principles

**Hardware-First Testing Strategy**  
Based on ISO 26262 standards and the V-Model approach:

```
┌─────────────────────────────────────────────────────────┐
│  L1: Native Tests (SIL - Software-in-the-Loop)          │
│      ├─ Pure algorithms (math, validation, conversion)  │
│      ├─ No hardware dependencies                        │
│      └─ Fast execution (~ms per test)                   │
├─────────────────────────────────────────────────────────┤
│  L2: Hardware Automated Tests (HIL - Hardware-in-Loop)  │
│      ├─ HAL integration                                 │
│      ├─ Real peripherals (timers, GPIO, UART)          │
│      └─ Pass/fail verification                          │
├─────────────────────────────────────────────────────────┤
│  L3: Manual & Stress Tests                              │
│      ├─ Interactive user validation                     │
│      ├─ Long-duration reliability testing               │
│      └─ Physical observation required                   │
└─────────────────────────────────────────────────────────┘
```

### 1.2 Test-Driven Development (TDD) Mandate

> [!CAUTION] > **Best Practice:** Write a test (or verification step) BEFORE fixing bugs or adding features. No exceptions.

**Workflow:**

1. **RED:** Write failing test that defines expected behavior
2. **GREEN:** Write minimum code to make test pass
3. **REFACTOR:** Optimize while maintaining test compliance
4. **VERIFY:** Ensure embedded constraints compliance

### 1.3 Framework Stack

| Component        | Technology             | Purpose                                  |
| :--------------- | :--------------------- | :--------------------------------------- |
| **Unit Testing** | Unity (ThrowTheSwitch) | C-native, lightweight, embedded-friendly |
| **Mocking**      | Custom HAL Mocks       | Hardware abstraction for native tests    |
| **Build System** | PlatformIO             | Test environment orchestration           |
| **Hardware**     | STM32 (Any variant)    | Target platform for HIL tests            |

---

## 2. Test Taxonomy & Directory Structure

### 2.1 Test Categories Template

```
test/
├── native/                      # L1: Native Tests (x86/x64, no hardware)
│   ├── test_{algorithm_name}/   # Math/logic testing
│   ├── test_{validation_name}/  # Input validation
│   └── test_{conversion_name}/  # Unit conversions
│
├── hardware/
│   ├── automated/               # L2: Pass/Fail HW Tests
│   │   ├── test_{peripheral}/   # Peripheral drivers
│   │   ├── test_{sensor}/       # Sensor interfaces
│   │   ├── test_{actuator}/     # Actuator control
│   │   └── test_{protocol}/     # Communication protocols
│   │
│   ├── manual/                  # L3: Interactive Tests
│   │   ├── test_{interactive}/  # User-driven validation
│   │   └── test_{calibration}/  # Calibration procedures
│   │
│   └── stress/                  # L3: Endurance Tests
│       ├── test_{reliability}/  # Long-duration tests
│       └── test_{limits}/       # Boundary testing
│
├── mocks/                       # HAL mocks for native tests
│   ├── Arduino.h                # Arduino API stubs
│   ├── stm32{family}_hal_mock.cpp  # STM32 HAL stubs
│   └── {peripheral}_mock.cpp   # Custom peripheral mocks
│
└── legacy/                      # Deprecated tests (optional)
```

### 2.2 Test Selection Rules

| Condition                       | Use Native Test | Use HW Automated | Use Manual Test |
| :------------------------------ | :-------------- | :--------------- | :-------------- |
| Pure algorithm (math, logic)    | ✅ Yes          | ❌ No            | ❌ No           |
| Requires real timers/interrupts | ❌ No           | ✅ Yes           | ❌ No           |
| Requires physical observation   | ❌ No           | ❌ No            | ✅ Yes          |
| Long duration (>10s)            | ❌ No           | ❌ No            | ✅ Yes (stress) |

---

## 3. Naming Conventions & Style

### 3.1 File Naming

**Pattern:** `{test_type}_{module_name}_test.cpp`

```
native/test_math_helpers/test_native.cpp                    ✅ CORRECT
hardware/automated/test_motor_driver/motor_automated_test.cpp ✅ CORRECT
hardware/manual/test_sensor_calib/sensor_manual_test.cpp      ✅ CORRECT
hardware/stress/test_comm_protocol/protocol_stress_test.cpp   ✅ CORRECT
```

### 3.2 Function Naming

**Pattern:** `test_{module}_{behavior}_{condition}`

```cpp
void test_pid_calculation_basic(void);              // ✅ Basic success case
void test_adc_reading_zero_voltage(void);           // ✅ Edge case
void test_validation_invalid_range(void);           // ✅ Failure case
void test_timer_init(void);                         // ✅ Initialization
void test_filter_roundtrip_precision(void);         // ✅ Precision check
```

**Anti-Patterns:**

```cpp
void test1(void);                  // ❌ Non-descriptive
void testMotor(void);              // ❌ Wrong case (use snake_case)
void test_motor_stuff(void);       // ❌ Vague behavior
```

### 3.3 Test Organization

**Required Structure:**

```cpp
#include <unity.h>
#include "ModuleUnderTest.h"  // System Under Test (SUT)

// Test fixture functions (MANDATORY)
void setUp(void) {
    // Pre-test initialization (runs before EACH test)
}

void tearDown(void) {
    // Post-test cleanup (runs after EACH test)
}

// Test functions
void test_feature_success_case(void) {
    // Arrange: Setup test data
    // Act: Execute function under test
    // Assert: Verify expected behavior
}

// Main function (Native) or setup/loop (Hardware)
int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_feature_success_case);
    UNITY_END();
    return 0;
}
```

---

## 4. Native Tests (L1): Pure Algorithm Testing

### 4.1 When to Use Native Tests

- ✅ Math calculations (filters, conversions, control algorithms)
- ✅ Configuration validation logic
- ✅ Unit conversions (metric ↔ imperial, degrees ↔ radians)
- ✅ State machine logic (without HAL dependencies)
- ❌ Anything requiring real timers, interrupts, or peripherals

### 4.2 PlatformIO Configuration Template

```ini
[env:native]
platform = native
build_flags =
    -D NATIVE_BUILD                # Flag to exclude hardware code
    -I test/mocks                  # Include mock headers
    -I include                     # Project headers
    -std=c++17                     # Modern C++ (or c++14)
build_src_filter =
    +<*>
    -<main.cpp>                    # Exclude main application
    -<Hardware/>                   # Exclude hardware-specific code
test_filter = native/*             # Only run native tests
```

### 4.3 Boilerplate Template: Native Test

**File:** `test/native/test_{module_name}/test_native.cpp`

```cpp
#include <unity.h>
#include "{YourNamespace}/{ModuleName}.h"  // Module under test

// ===== Test Fixture =====

void setUp(void) {
    // Initialize test data (runs before EACH test)
}

void tearDown(void) {
    // Cleanup (runs after EACH test)
}

// ===== Test Cases =====

// Test naming: test_{module}_{behavior}_{condition}
void test_{module}_calculation_positive_input(void) {
    // Arrange: Prepare input data
    int32_t input_value = 100;

    // Act: Execute function under test
    int32_t result = yourFunction(input_value);

    // Assert: Verify expected result
    TEST_ASSERT_EQUAL_INT32(expected_value, result);
}

void test_{module}_calculation_negative_input(void) {
    int32_t input_value = -100;

    int32_t result = yourFunction(input_value);

    TEST_ASSERT_EQUAL_INT32(expected_value, result);
}

void test_{module}_calculation_overflow_protection(void) {
    // Edge case: Test overflow behavior
    int32_t input_value = INT32_MAX;

    int32_t result = yourFunction(input_value);

    // Verify saturation or safe handling
    TEST_ASSERT_TRUE(result <= INT32_MAX);
}

// ===== Test Runner =====

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Happy path tests first
    RUN_TEST(test_{module}_calculation_positive_input);
    RUN_TEST(test_{module}_calculation_negative_input);

    // Edge cases
    RUN_TEST(test_{module}_calculation_overflow_protection);

    UNITY_END();
    return 0;
}
```

**Execution:**

```bash
pio test -e native -f "native/test_{module_name}" -v
```

### 4.4 Common Unity Assertions

```cpp
// Integer assertions
TEST_ASSERT_EQUAL_INT32(expected, actual);
TEST_ASSERT_INT32_WITHIN(delta, expected, actual);  // |expected - actual| <= delta

// Floating-point assertions (use tolerance!)
TEST_ASSERT_FLOAT_WITHIN(0.001f, expected, actual);  // Within ±0.001

// Boolean assertions
TEST_ASSERT_TRUE(condition);
TEST_ASSERT_FALSE(condition);

// Pointer/NULL assertions
TEST_ASSERT_NULL(pointer);
TEST_ASSERT_NOT_NULL(pointer);

// String assertions
TEST_ASSERT_EQUAL_STRING("expected", actual_str);

// Custom messages
TEST_ASSERT_TRUE_MESSAGE(condition, "Custom failure message");
```

---

## 5. Hardware Automated Tests (L2): HIL Testing

### 5.1 When to Use Hardware Automated Tests

- ✅ Peripheral initialization (timers, GPIO, UART, SPI, I2C)
- ✅ Motor/actuator control (enable/disable, position tracking)
- ✅ Sensor reading (ADC, encoder, IMU)
- ✅ EEPROM/Flash read/write operations
- ✅ Communication protocol verification
- ❌ Tests requiring user input (use manual tests instead)

### 5.2 PlatformIO Configuration Template

```ini
[env:{your_board_name}]
platform = ststm32
board = {your_board}              # e.g., nucleo_f446re, blackpill_f411ce
framework = arduino               # or stm32cube
upload_protocol = stlink
debug_tool = stlink
monitor_speed = 115200
monitor_filters = direct
test_port = COM6                  # ⚠️ Update to your serial port
test_speed = 115200
test_filter = hardware/*          # Include all hardware tests
build_flags =
    -DUSE_HAL_DRIVER
    -DSTM32{FAMILY}xx             # e.g., STM32F4xx, STM32H7xx
    -O0                           # No optimization for debugging
    -g3                           # Full debug symbols
    -I include
    -Wl,-u,_printf_float          # Enable float printf
```

### 5.3 Boilerplate Template: Hardware Automated Test

**File:** `test/hardware/automated/test_{module}/test_automated.cpp`

```cpp
#include <Arduino.h>
#include <unity.h>
#include "Hardware/{ModuleName}.h"

// Include implementations (REQUIRED for on-target testing)
#include "../../src/Hardware/{ModuleName}.cpp"
// Add other dependencies as needed

// Debug serial port (MANDATORY for test output)
const uint8_t k_debug_tx = {YOUR_TX_PIN};  // e.g., PD8
const uint8_t k_debug_rx = {YOUR_RX_PIN};  // e.g., PD9
HardwareSerial SerialDebug(k_debug_rx, k_debug_tx);

// System Under Test
{ModuleType} module_instance;

// ===== Test Cases =====

void test_{module}_initialization(void) {
    SerialDebug.println("\n[TEST] {Module} Initialization");

    bool init_result = module_instance.begin();

    TEST_ASSERT_TRUE_MESSAGE(init_result, "{Module} init failed");
    SerialDebug.println("✅ {Module} initialized");
}

void test_{module}_enable_disable(void) {
    SerialDebug.println("\n[TEST] {Module} Enable/Disable");

    module_instance.enable();
    bool is_enabled = module_instance.isEnabled();
    TEST_ASSERT_TRUE_MESSAGE(is_enabled, "Should be enabled");

    module_instance.disable();
    is_enabled = module_instance.isEnabled();
    TEST_ASSERT_FALSE_MESSAGE(is_enabled, "Should be disabled");

    SerialDebug.println("✅ Enable/disable OK");
}

// ===== Arduino Framework Entry Points =====

void setup() {
    // Initialize debug serial FIRST
    SerialDebug.begin(115200);
    delay(2000);  // Wait for serial monitor connection

    // Print test header (repeat for visibility)
    for(int i = 0; i < 5; i++) {
        SerialDebug.println(">>> {MODULE} AUTOMATED TEST <<<");
        delay(100);
    }

    SerialDebug.println("\n========================================");
    SerialDebug.println("  {MODULE} AUTOMATED TEST");
    SerialDebug.println("========================================\n");

    // Run Unity test framework
    UNITY_BEGIN();
    RUN_TEST(test_{module}_initialization);
    RUN_TEST(test_{module}_enable_disable);
    int result = UNITY_END();

    // Print final result
    SerialDebug.println();
    SerialDebug.println(result == 0 ? "✅ ALL TESTS PASSED" : "❌ TESTS FAILED");
    SerialDebug.print("Result: ");
    SerialDebug.println(result == 0 ? "PASS" : "FAIL");
}

void loop() {
    // Empty loop (tests run once in setup)
}
```

**Execution:**

```bash
pio test -e {your_board} -f "hardware/automated/test_{module}" -v
```

### 5.4 Hardware Test Best Practices

> [!WARNING] > **STM32 Embedded Constraints (See Section 12):**
>
> - ISR execution time < 10µs
> - NO floating-point math in ISR
> - NO blocking calls (HAL_Delay in ISR)
> - MANDATORY `volatile` for ISR-shared variables
> - MANDATORY critical sections for multi-byte data

```cpp
// ✅ CORRECT: Hardware test with timing verification
void test_{module}_operation_with_timeout(void) {
    module_instance.startOperation();

    uint32_t start = millis();
    constexpr uint32_t k_timeout_ms = 5000;  // 5 second timeout

    // Wait for completion with timeout
    while(module_instance.isRunning()) {
        if (millis() - start > k_timeout_ms) {
            TEST_FAIL_MESSAGE("Test timeout");
            break;
        }
        delay(10);  // Yield to other tasks
    }

    TEST_ASSERT_TRUE(module_instance.isComplete());
}

// ❌ INCORRECT: Blocking without timeout
void test_{module}_bad(void) {
    module_instance.startOperation();
    while(module_instance.isRunning());  // ❌ Infinite loop if operation fails
}
```

---

## 6. Manual & Stress Tests (L3): Interactive Validation

### 6.1 Manual Test Template

**File:** `test/hardware/manual/test_{module}/test_manual.cpp`

```cpp
#include <Arduino.h>
#include "Hardware/{ModuleName}.h"

const uint8_t k_debug_tx = {YOUR_TX_PIN};
const uint8_t k_debug_rx = {YOUR_RX_PIN};
HardwareSerial SerialDebug(k_debug_rx, k_debug_tx);

{ModuleType} module_instance;

void setup() {
    SerialDebug.begin(115200);
    delay(1000);

    SerialDebug.println("\n========================================");
    SerialDebug.println("  {MODULE} MANUAL TEST");
    SerialDebug.println("========================================\n");

    if (!module_instance.begin()) {
        SerialDebug.println("❌ Init failed!");
        while(1);
    }

    SerialDebug.println("COMMANDS:");
    SerialDebug.println("  f = forward/start");
    SerialDebug.println("  b = backward/stop");
    SerialDebug.println("  r = reset\n");
    SerialDebug.println("Waiting for input...");
}

void loop() {
    // Handle serial commands
    if (SerialDebug.available() > 0) {
        char cmd = SerialDebug.read();

        SerialDebug.print("\n[RX: '");
        SerialDebug.print(cmd);
        SerialDebug.print("'] ");

        switch(cmd) {
            case 'f':
            case 'F':
                SerialDebug.println("→ FORWARD/START");
                module_instance.start();
                break;

            case 'b':
            case 'B':
                SerialDebug.println("← BACKWARD/STOP");
                module_instance.stop();
                break;

            case 'r':
            case 'R':
                SerialDebug.println("↻ RESET");
                module_instance.reset();
                break;

            default:
                SerialDebug.println("Unknown command");
        }
    }

    // Status display every 200ms
    static uint32_t last_update = 0;
    if (millis() - last_update >= 200) {
        last_update = millis();

        int32_t value = module_instance.getValue();
        SerialDebug.print("Value: ");
        SerialDebug.println(value);
    }
}
```

### 6.2 Stress Test Template

```cpp
void test_rapid_cycles_1000x(void) {
    SerialDebug.println("\n[STRESS] 1000x Rapid Enable/Disable");

    constexpr uint32_t k_cycles = 1000;
    uint32_t errors = 0;

    for(uint32_t i = 0; i < k_cycles; i++) {
        module_instance.enable();
        if (!module_instance.isEnabled()) errors++;

        module_instance.disable();
        if (module_instance.isEnabled()) errors++;
    }

    SerialDebug.print("Completed ");
    SerialDebug.print(k_cycles);
    SerialDebug.println(" cycles");

    TEST_ASSERT_EQUAL_MESSAGE(0, errors, "Errors detected during stress");
    SerialDebug.println("✅ Stress test OK");
}
```

---

## 7. Mocking Strategy for Native Tests

### 7.1 Mock Directory Structure

```
test/mocks/
├── Arduino.h                # Arduino API stubs (Serial, millis, etc.)
├── stm32{family}_hal_mock.cpp  # STM32 HAL function stubs
├── {Peripheral}_mock.cpp    # Custom peripheral mocks
└── {Protocol}_mock.cpp      # Communication protocol mocks
```

### 7.2 Mock Implementation Example

**File:** `test/mocks/{Module}_mock.cpp`

```cpp
#include "{Module}.h"
#include <stdio.h>
#include <vector>

// Mock data storage for verification
struct {Module}Call {
    int param1;
    float param2;
};
std::vector<{Module}Call> call_history;

// Helper function to clear history between tests
void clearHistory() {
    call_history.clear();
}

// Mock implementations (no-op or logging only)
{Module}::{Module}() {
    printf("{Module}: Mock constructor\n");
}

bool {Module}::begin() {
    printf("{Module}: Mock begin()\n");
    return true;
}

void {Module}::setValue(int param1, float param2) {
    printf("{Module}: setValue(%d, %.2f)\n", param1, param2);
    call_history.push_back({param1, param2});
}
```

### 7.3 Using Mocks in Tests

```cpp
#include <unity.h>
#include "{Module}.h"  // Uses mock implementation

extern std::vector<{Module}Call> call_history;
extern void clearHistory();

void setUp(void) {
    clearHistory();  // Clear mock state before each test
}

void test_{module}_calls_setValue(void) {
    {Module} instance;
    instance.begin();

    instance.setValue(42, 3.14f);

    // Verify mock captured the call
    TEST_ASSERT_EQUAL(1, call_history.size());
    TEST_ASSERT_EQUAL(42, call_history[0].param1);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.14f, call_history[0].param2);
}
```

---

## 8. STM32 Embedded Constraints (Universal Best Practices)

### 8.1 Absolute PROHIBITIONS

> [!CAUTION] > **FORTRESS MODE - ZERO TOLERANCE:**
>
> These rules apply to ALL safety-critical embedded systems and are based on MISRA C, CERT C, and industry standards.

```cpp
// ❌ FORBIDDEN: Dynamic memory allocation
void test_bad_malloc(void) {
    int* data = (int*)malloc(100 * sizeof(int));  // ❌ NEVER in embedded
    free(data);                                    // ❌ NEVER
}

// ❌ FORBIDDEN: Recursion (stack overflow risk)
int factorial(int n) {
    return (n == 0) ? 1 : n * factorial(n - 1);   // ❌ NO RECURSION
}

// ❌ FORBIDDEN: Unbounded loops (hang risk)
void test_bad_loop(void) {
    while(peripheral.isRunning()) {  // ❌ No timeout
        // Infinite loop risk
    }
}

// ❌ FORBIDDEN: Magic numbers (maintainability)
void test_bad_magic(void) {
    motor.setSpeed(1234);  // ❌ What is 1234?
}

// ❌ FORBIDDEN: Implicit auto (readability)
void test_bad_auto(void) {
    auto value = peripheral.getValue();  // ❌ Type unclear
}

// ❌ FORBIDDEN: Floating-point in ISR (timing)
void EXTI_IRQHandler(void) {
    float result = sensorValue * 3.14f;  // ❌ NO FP MATH IN ISR
}
```

### 8.2 MANDATORY Practices

```cpp
// ✅ CORRECT: Static allocation only
void test_good_static(void) {
    static int32_t buffer[100];  // ✅ Static allocation (compile-time)
    // Or stack allocation for small arrays
    int32_t small_buf[10];       // ✅ Stack allocation
}

// ✅ CORRECT: Named constants (self-documenting code)
static constexpr uint32_t k_motor_speed_hz = 1000;
static constexpr float k_adc_voltage_ref = 3.3f;

void test_good_constants(void) {
    motor.setSpeed(k_motor_speed_hz);  // ✅ Clear intent
}

// ✅ CORRECT: Bounded loops with timeout
void test_good_timeout(void) {
    uint32_t start = millis();
    constexpr uint32_t k_timeout_ms = 5000;

    while(peripheral.isRunning()) {
        if (millis() - start > k_timeout_ms) {
            TEST_FAIL_MESSAGE("Timeout");
            break;
        }
        delay(10);
    }
}

// ✅ CORRECT: Explicit types (readability)
void test_good_typing(void) {
    int32_t value = peripheral.getValue();      // ✅ Explicit
    float temperature = sensor.readTemp();      // ✅ Explicit
}

// ✅ CORRECT: Volatile for ISR-shared data
static volatile uint32_t g_interrupt_count = 0;

void EXTI_IRQHandler(void) {
    g_interrupt_count++;  // ✅ Only integer ops in ISR
}

void test_isr_safety(void) {
    // Access volatile variable safely
    uint32_t count_snapshot = g_interrupt_count;  // Atomic read on 32-bit MCU
}
```

### 8.3 Test Complexity Limits

**Industry Standards (MISRA, CERT, Safety-Critical):**

- ✅ Max 60 lines per function (including test functions)
- ✅ Max cyclomatic complexity of 10
- ✅ Max 1 level of pointer dereference
- ✅ NO function pointers, NO pointer arithmetic

```cpp
// ✅ CORRECT: Simple, focused test (<60 lines)
void test_{module}_simple_operation(void) {
    module.enable();
    bool is_enabled = module.isEnabled();
    TEST_ASSERT_TRUE(is_enabled);
}

// ❌ INCORRECT: Overly complex test (>60 lines, high complexity)
void test_everything_at_once(void) {
    // 100+ lines of initialization
    // 20+ conditional branches
    // ❌ VIOLATION: Split into multiple tests
}
```

### 8.4 ISR Discipline

**Critical Rules:**

- ⏱️ Execution time **< 10µs** (prefer < 5µs)
- 🚫 **NO** dynamic allocation
- 🚫 **NO** floating-point math
- 🚫 **NO** blocking calls (`HAL_Delay`, `printf`, etc.)
- ✅ **MANDATORY** `volatile` for shared variables
- ✅ **MANDATORY** Critical sections for multi-byte data

```cpp
// ✅ CORRECT: Fast, integer-only ISR
static volatile uint32_t g_pulse_count = 0;

void TIM2_IRQHandler(void) {
    if (LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        LL_TIM_ClearFlag_UPDATE(TIM2);
        g_pulse_count++;  // Fast, atomic operation
    }
}

// ❌ INCORRECT: Slow, complex ISR
void BAD_IRQHandler(void) {
    float result = computePID();          // ❌ Floating-point
    Serial.println("Interrupt!");         // ❌ Blocking I/O
    processComplexLogic();                // ❌ Too slow
}
```

---

## 9. Edge Case & Failure Mode Testing

### 9.1 The "Unhappy Path" Priority

> [!TIP] > **Best Practice:** Prioritize failure modes over success scenarios. Test what CAN go wrong, not just what SHOULD work.

**Mandatory Edge Cases:**

```cpp
// ✅ Zero input
void test_{module}_calculation_zero_input(void) {
    int32_t result = calculateValue(0);
    TEST_ASSERT_EQUAL_INT32(0, result);
}

// ✅ Negative input
void test_{module}_calculation_negative_input(void) {
    int32_t result = calculateValue(-100);
    TEST_ASSERT_EQUAL_INT32(-expected, result);
}

// ✅ Maximum values (boundary testing)
void test_{module}_calculation_max_input(void) {
    int32_t result = calculateValue(INT32_MAX);
    TEST_ASSERT_TRUE(result <= INT32_MAX);  // No overflow
}

// ✅ Invalid configuration
void test_{module}_validation_invalid_config(void) {
    ValidationResult result = validateConfig({.invalid = 0});

    TEST_ASSERT_FALSE(result.valid);
    TEST_ASSERT_NOT_NULL(result.error_message);
    TEST_ASSERT_EQUAL_STRING("Expected error", result.error_message);
}

// ✅ Timeout handling
void test_{module}_timeout_recovery(void) {
    module.startOperation();

    uint32_t start = millis();
    bool completed = false;

    while(millis() - start < 1000) {  // 1s timeout
        if (module.isComplete()) {
            completed = true;
            break;
        }
        delay(10);
    }

    if (!completed) {
        module.abort();  // Test graceful abort
        TEST_ASSERT_TRUE(module.isIdle());
    }
}
```

### 9.2 Boundary Value Analysis

Test at boundaries of valid ranges:

```cpp
void test_{module}_minimal_valid_input(void) {
    // Just above minimum threshold
    int32_t result = calculateValue(MIN_THRESHOLD + 1);
    TEST_ASSERT_TRUE(result > 0);
}

void test_{module}_just_below_threshold(void) {
    // Just below minimum (should fail or return error)
    int32_t result = calculateValue(MIN_THRESHOLD - 1);
    TEST_ASSERT_EQUAL_INT32(ERROR_CODE, result);
}
```

---

## 10. Running Tests

### 10.1 Command Reference

```bash
# Run ALL native tests (x86, fast)
pio test -e native -f "native/*"

# Run ALL hardware automated tests
pio test -e {your_board} -f "hardware/automated/*"

# Run SPECIFIC test by name
pio test -e {your_board} -f "hardware/automated/test_{module}" -v

# Run manual test (interactive)
pio test -e {your_board} -f "hardware/manual/test_{module}" -v

# Run stress test
pio test -e {your_board} -f "hardware/stress/test_{module}" -v

# Restore main firmware after testing
pio run -e {your_board} -t upload
```

### 10.2 Expected Output Format

All tests MUST output results via serial monitor:

```
>>> {MODULE} AUTOMATED TEST <<<
========================================
  {MODULE} AUTOMATED TEST
========================================

[TEST] {Module} Initialization
✅ {Module} initialized

[TEST] {Module} Enable/Disable
✅ Enable/disable OK

✅ ALL TESTS PASSED
Result: PASS
```

---

## 11. AI Agent Quick Reference

### 11.1 Checklist: Creating a New Test

- [ ] Determine test type (Native / HW Automated / Manual / Stress)
- [ ] Create directory: `test/{type}/test_{module}/`
- [ ] Create test file: `{module}_{type}_test.cpp`
- [ ] Include Unity framework: `#include <unity.h>`
- [ ] Implement `setUp()` and `tearDown()` fixtures
- [ ] Name tests: `test_{module}_{behavior}_{condition}`
- [ ] Use AAA pattern: Arrange → Act → Assert
- [ ] Test edge cases and failure modes FIRST
- [ ] Verify embedded constraints compliance (Section 8)
- [ ] Add test execution command to documentation

### 11.2 Copy-Paste Boilerplates

**Native Test (Minimal):**

```cpp
#include <unity.h>
#include "{Module}.h"

void setUp(void) {}
void tearDown(void) {}

void test_{module}_success_case(void) {
    // Arrange
    int32_t input = 100;

    // Act
    int32_t result = functionUnderTest(input);

    // Assert
    TEST_ASSERT_EQUAL_INT32(expected, result);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_{module}_success_case);
    UNITY_END();
    return 0;
}
```

**Hardware Test (Minimal):**

```cpp
#include <Arduino.h>
#include <unity.h>
#include "Hardware/{Module}.h"

const uint8_t k_debug_tx = {TX_PIN};
const uint8_t k_debug_rx = {RX_PIN};
HardwareSerial SerialDebug(k_debug_rx, k_debug_tx);

void test_{module}_init(void) {
    bool result = module.begin();
    TEST_ASSERT_TRUE_MESSAGE(result, "Init failed");
}

void setup() {
    SerialDebug.begin(115200);
    delay(2000);

    UNITY_BEGIN();
    RUN_TEST(test_{module}_init);
    int result = UNITY_END();

    SerialDebug.println(result == 0 ? "✅ PASS" : "❌ FAIL");
}

void loop() {}
```

### 11.3 Common Pitfalls & Solutions

| Problem                          | Solution                                       |
| :------------------------------- | :--------------------------------------------- |
| Test hangs indefinitely          | Add timeout with `millis()` check              |
| Float comparison fails           | Use `TEST_ASSERT_FLOAT_WITHIN(tolerance, …)`   |
| Mock not used in native test     | Verify `-I test/mocks` in `platformio.ini`     |
| Serial output not visible        | Ensure `-v` flag in `pio test` command         |
| Dynamic allocation violation     | Replace `malloc`/`new` with static arrays      |
| Hardware test fails occasionally | Add initialization delay (e.g., `delay(1000)`) |

---

## 12. Project-Specific Customization

### 12.1 Adapting This Template

When using this template in a new project:

1. **Replace Placeholders:**

   - `{PROJECT_NAME}` → Your project name
   - `{MODULE}` → Your module name (e.g., `MotorDriver`, `ADCInterface`)
   - `{your_board}` → Your STM32 board identifier
   - `{TX_PIN}`, `{RX_PIN}` → Your debug UART pins

2. **Update File Paths:**

   - Adjust `build_src_filter` in platformio.ini
   - Update include paths to match your directory structure

3. **Customize Test Categories:**

   - Add/remove test categories based on your needs
   - Modify test selection rules (Section 2.2)

4. **Project-Specific Rules:**
   - If you have a `GEMINI.md` or coding standards document, reference it
   - Add project-specific constraints to Section 8

### 12.2 Example: PrimePrecision-ELS Project

This template is used in the **PrimePrecision-ELS** project with the following customizations:

- **Board:** `devebox_h743vitx` (STM32H743VIT6)
- **Debug UART:** PD8 (TX), PD9 (RX)
- **Modules:** `StepperTimer`, `EncoderTimer`, `DisplayComm`, `MotionControl`
- **Coding Standards:** [GEMINI.md](file:///C:/Users/SKTech/Desktop/project_files/projects/ONGOING/PrimePrecision-ELS/software/GEMINI.md) FORTRESS MODE
- **Test Categories:** Native (5), HW Automated (6), Manual (3), Stress (4)

See [project-specific TESTING_COMMANDS.md](file:///C:/Users/SKTech/Desktop/project_files/projects/ONGOING/PrimePrecision-ELS/software/test/TESTING_COMMANDS.md) for execution examples.

---

## 13. References & Resources

### 13.1 Testing Frameworks

- **Unity Framework:** [ThrowTheSwitch Unity](https://github.com/ThrowTheSwitch/Unity)
- **PlatformIO Testing:** [Unit Testing Guide](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)

### 13.2 Embedded Standards

- **MISRA C:** Safety-critical C coding standard
- **CERT C:** Secure coding practices
- **ISO 26262:** Functional safety for automotive systems
- **DO-178C:** Software safety for avionics

### 13.3 Testing Methodologies

- **V-Model:** Systems engineering verification methodology
- **TDD (Test-Driven Development):** RED-GREEN-REFACTOR cycle
- **SIL/HIL:** Software/Hardware-in-the-Loop testing

---

## 14. Version History

| Version | Date       | Changes                              |
| :------ | :--------- | :----------------------------------- |
| 2.0     | 2025-11-27 | Universal template, project-agnostic |
| 1.0     | 2025-11-27 | Initial project-specific version     |

---

**END OF DOCUMENT**

> [!NOTE]
> This document is optimized for:
>
> 1. **AI agent consumption** - Structured context for test generation
> 2. **Multi-project reuse** - Generic patterns and templates
> 3. **Quick bootstrapping** - 5-minute setup guide
> 4. **Best practices enforcement** - STM32 embedded constraints codified
