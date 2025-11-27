# STM32 Embedded Math & Physics Guide
> **Scope:** Universal STM32 Series (F0-H7)  
> **Focus:** Performance, Precision, and Hardware Acceleration

## 1. The Hierarchy of Math Optimization

When implementing control loops or signal processing on STM32, choose the right tool for the job:

| Method | Best For | Pros | Cons |
| :--- | :--- | :--- | :--- |
| **Integer / Fixed-Point** | Simple control logic, PID, Cortex-M0/M3 | Zero FPU overhead, deterministic | Manual scaling, limited range |
| **Hardware FPU** | General physics, Cortex-M4F/M7 | Easy to read, wide dynamic range | Context switch overhead in RTOS |
| **CORDIC (Hardware)** | Trig/Hyperbolic on G4/H7 | Ultra-fast sin/cos/atan2 | Fixed-point input only, limited ops |
| **CMSIS-DSP** | FFT, Filters, Matrix Ops | Highly optimized SIMD/Assembly | Library overhead, learning curve |

---

## 2. Fixed-Point Arithmetic (The "Integer" Way)
Essential for low-end MCUs or ISRs where FPU context save is too costly.

### 2.1 Concept
Represent fractional numbers as integers by scaling them up by a factor of $2^Q$.
- **Q15:** 1 sign bit, 15 fractional bits (Range: -1 to ~1)
- **Q31:** 1 sign bit, 31 fractional bits

### 2.2 Implementation Pattern
```cpp
// Example: Q16.16 Fixed Point (16 integer bits, 16 fractional bits)
#define Q16_SHIFT 16
#define FLOAT_TO_FIXED(f) ((int32_t)((f) * (1 << Q16_SHIFT)))
#define FIXED_TO_FLOAT(x) ((float)(x) / (1 << Q16_SHIFT))

int32_t a = FLOAT_TO_FIXED(3.14159);
int32_t b = FLOAT_TO_FIXED(2.0);

// Addition: Direct
int32_t sum = a + b;

// Multiplication: Result is Q32.32, shift back to Q16.16
int32_t prod = (int32_t)(((int64_t)a * b) >> Q16_SHIFT);
```

---

## 3. Hardware FPU Usage
Most modern STM32s (F3, F4, G4, F7, H7) have a Single Precision FPU (Cortex-M4F).

### 3.1 Enabling FPU
Ensure your compiler flags are set correctly (usually automatic in IDEs):
- **GCC:** `-mfloat-abi=hard -mfpu=fpv4-sp-d16`
- **Code:** `SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));` (SystemInit usually handles this)

### 3.2 Best Practices
1.  **Use `float`, not `double`:** Standard `double` is 64-bit. The FPU is 32-bit. Using `double` triggers slow software emulation.
    ```cpp
    float x = 3.14f; // CORRECT: 'f' suffix forces 32-bit float
    float y = 3.14;  // WRONG: Literal is double, forces conversion
    ```
2.  **Fast Math Functions:** Use `sinf()`, `cosf()`, `sqrtf()` instead of `sin()`, `cos()`, `sqrt()`.

---

## 4. CORDIC Acceleration (G4, H7)
A dedicated co-processor for rotation and trigonometric functions.

### 4.1 Capabilities
- Sine, Cosine, Sinh, Cosh
- Arctangent (Atan2), Modulus
- Square Root

### 4.2 HAL Usage Example
```cpp
CORDIC_ConfigTypeDef sCordicConfig;
sCordicConfig.Function = CORDIC_FUNCTION_SINE;
sCordicConfig.Precision = CORDIC_PRECISION_6CYCLES;
sCordicConfig.Scale = CORDIC_SCALE_0;

HAL_CORDIC_Configure(&hcordic, &sCordicConfig);

int32_t input_q31 = 0x40000000; // +0.5 in Q31
int32_t output_q31;

HAL_CORDIC_Calculate(&hcordic, &input_q31, &output_q31, 1, 100);
```

---

## 5. CMSIS-DSP Library
ARM's optimized Digital Signal Processing library.

### 5.1 Common Functions
- **PID Controller:** `arm_pid_f32`
- **FFT:** `arm_rfft_fast_f32`
- **Matrix:** `arm_mat_mult_f32`
- **Filters:** `arm_biquad_cascade_df1_f32`

### 5.2 Example: Vector Dot Product
```cpp
#include "arm_math.h"

float32_t vecA[4] = {1.0f, 2.0f, 3.0f, 4.0f};
float32_t vecB[4] = {1.0f, 0.0f, 1.0f, 0.0f};
float32_t result;

arm_dot_prod_f32(vecA, vecB, 4, &result);
```
