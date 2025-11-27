# STM32 Rules: Examples & Rationale Reference
> **Companion to:** `STM32_RULES_STRICT.md`

---

## Rule 1: Strict Control Flow
**Rationale:** Simple control flow enables static analysis and prevents stack overflow.
### Examples
```cpp
// CORRECT: Bounded iteration
void processBuffer(uint8_t* data, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        processDataPoint(data[i]);
    }
}

// VIOLATION: Recursion
uint32_t factorial(uint32_t n) {
    return (n == 0) ? 1 : n * factorial(n - 1);  // FORBIDDEN
}
```

---

## Rule 2: Provable Loop Bounds
**Rationale:** Prevents infinite loops in real-time systems.
### Examples
```cpp
// CORRECT: Bounded with timeout
#define MAX_WAIT 10000
bool waitForFlag(volatile uint32_t* reg, uint32_t mask) {
    for (uint32_t i = 0; i < MAX_WAIT; i++) {
        if (*reg & mask) return true;
    }
    return false;
}

// VIOLATION: Unbounded wait
while (!(I2C1->SR1 & I2C_SR1_TXE)); // FORBIDDEN
```

---

## Rule 3: Zero Dynamic Allocation
**Rationale:** Prevents heap fragmentation and non-deterministic failures.
### Examples
```cpp
// CORRECT: Static allocation
class MotionController {
    static constexpr uint16_t BUF_SIZE = 256;
    uint8_t _buffer[BUF_SIZE]; 
};

// VIOLATION: Runtime allocation
void process(int size) {
    uint8_t* buf = new uint8_t[size]; // FORBIDDEN
}
```

---

## Rule 4: Function Complexity
**Rationale:** Ensures code is reviewable and debuggable.
### Strategy
- Break large functions into `init()`, `validate()`, `process()`, `cleanup()`.
- Use inline functions instead of deep nesting.

---

## Rule 5: Assertion Density
**Rationale:** Catches impossible states early.
### Examples
```cpp
// CORRECT: Defensive programming
void moveTo(int32_t target) {
    assert(_maxSpeed > 0 && "Not initialized");
    assert(abs(target - _pos) < LIMIT && "Unsafe move");
    // ...
}
```

---

## Rule 6: Minimal Scope
**Rationale:** Reduces shared state and race conditions.
### Examples
```cpp
// CORRECT: Local scope
void process() {
    for (int i=0; i<10; i++) {
        int temp = calculate(i); // Exists only here
    }
}

// VIOLATION: Global
int g_temp; // FORBIDDEN
```

---

## Rule 7: Full Validation
**Rationale:** Errors must be handled, not ignored.
### Examples
```cpp
// CORRECT: Check return & params
ErrorCode init(TIM_TypeDef* timer) {
    if (timer == nullptr) return ERR_NULL; // Validate param
    // ...
}

void main() {
    if (init(TIM2) != OK) errorHandler(); // Check return
}
```

---

## Rule 8: Restricted Preprocessor
**Rationale:** Macros hide bugs and confuse analysis.
### Examples
```cpp
// CORRECT: Inline function
inline bool isSet(uint32_t r, uint32_t b) { return (r & b); }

// VIOLATION: Complex macro
#define CHECK(x) if(x) return // FORBIDDEN (hides flow)
```

---

## Rule 9: Disciplined Pointers
**Rationale:** Pointers are the main source of crashes.
### Examples
```cpp
// CORRECT: Hardware Register
TIM2->CNT = 0;

// VIOLATION: Function Pointer
typedef void (*Callback)(void); // FORBIDDEN
```

---

## STM32 Addendum Examples

### ISR Safety
```cpp
// CORRECT: Critical Section
uint32_t readShared() {
    __disable_irq();
    uint32_t val = g_sharedVar;
    __enable_irq();
    return val;
}
```

### DMA Safety (H7)
```cpp
// CORRECT: Cache Maintenance
SCB_CleanDCache_by_Addr((uint32_t*)txBuf, len);
HAL_DMA_Start(&hdma, (uint32_t)txBuf, dst, len);
```
