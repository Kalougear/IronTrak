# STM32 Safety-Critical Rules (Token Optimized)
> **Version:** 1.0 | **Target:** STM32 Series | **Source:** NASA Power of 10 + MISRA C

## 1. Strict Control Flow
- **NO** `goto`, `setjmp`, `longjmp`.
- **NO** recursion (direct or indirect).
- **EXCEPTION:** Application `while(1)` main loop is permitted.

## 2. Provable Loop Bounds
- Every loop (`for`, `while`, `do-while`) **MUST** have a statically verifiable upper bound.
- Loop counters must be finite and monotonically approaching termination.
- **EXCEPTION:** Application `while(1)` only.

## 3. Zero Dynamic Allocation
- **NO** `new`, `malloc`, `calloc`, `realloc`, `delete`, `free` after initialization.
- All memory allocation **MUST** be static, compile-time determined, or stack-based.

## 4. Function Complexity Limits
- **Maximum 60 lines** per function.
- **Maximum cyclomatic complexity of 10**.

## 5. Assertion Density
- **Minimum 2 assertions** per non-trivial function (>10 lines).
- Assertions must trigger explicit recovery (no silent failures).

## 6. Minimal Scope
- Declare variables in **smallest scope** possible.
- **NO global variables** except justified hardware singletons or `volatile` ISR flags.

## 7. Full Validation
- **Caller:** Must check return values of ALL non-`void` functions.
- **Callee:** Must validate ALL pointer (`nullptr`) and numeric parameters.

## 8. Restricted Preprocessor
- **PERMITTED:** `#include`, simple `#define`, guards.
- **FORBIDDEN:** Token pasting (`##`), variadic (`...`), recursive macros, hiding control flow.

## 9. Disciplined Pointers
- **Max 1 level** of dereference.
- **NO** function pointers.
- **NO** pointer arithmetic (except bounded array indexing).
- **EXCEPTION:** Hardware register pointers permitted.

## 10. Strict Compilation
- **Flags:** `-Wall -Wextra -Wpedantic -Werror`.
- **Zero** static analysis defects allowed.

---

## STM32 Addendum: ISR & Hardware Safety

### 1. Volatile Discipline
- **MANDATORY** `volatile` for all variables shared between ISR and Main Loop.
- **MANDATORY** `volatile` for all hardware register pointers.

### 2. Atomic Access
- **MANDATORY** Critical sections (`__disable_irq`) when accessing multi-byte shared data (e.g., `uint32_t` on 8-bit, or struct access).

### 3. ISR Constraints
- Execution time **< 10µs**.
- **NO** floating-point math.
- **NO** blocking calls (`HAL_Delay`).

### 4. DMA Safety (Cortex-M7/H7)
- **MANDATORY** Cache maintenance (Clean/Invalidate) OR use of MPU-protected non-cacheable memory regions for DMA buffers.
