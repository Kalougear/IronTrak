# STM32 Timer Cookbook (Universal)
> **Scope:** General Purpose Timers (TIM2-TIM5, TIM15-17) & Advanced (TIM1, TIM8)  
> **Focus:** Practical Configuration Recipes

## 1. PWM Generation
Control motors, LEDs, or generate analog voltages.

### 1.1 Configuration
- **Mode:** PWM Generation CHx
- **Prescaler (PSC):** Sets the "tick" speed.
- **Auto-Reload (ARR):** Sets the PWM Frequency.
- **Capture/Compare (CCR):** Sets the Duty Cycle.

$$Frequency = \frac{F_{clk}}{(PSC+1)(ARR+1)}$$
$$Duty\% = \frac{CCR}{ARR+1} \times 100$$

### 1.2 HAL Code
```cpp
// Start PWM on Channel 1
HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

// Change Duty Cycle (e.g., 50% of ARR=1000 -> 500)
__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 500);
```

---

## 2. Input Capture (Frequency/Pulse Width)
Measure external signals.

### 2.1 Configuration
- **Mode:** Input Capture Direct Mode
- **Channel:** CH1 (Rising Edge), CH2 (Falling Edge) - Linked for PWM Input
- **Slave Mode:** Reset Mode (Trigger Source: TI1FP1) -> Resets counter on edge

### 2.2 HAL Code (Interrupt)
```cpp
// Start Capture
HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

// ISR Callback
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        uint32_t period = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        // Warning: Division in ISR. Ensure Period > 0.
        // For Rule 11 (<10us), ensure this is the ONLY heavy op.
        if (period > 0) {
            uint32_t frequency = TimerClock / period;
        }
    }
}
```

---

## 3. Quadrature Encoder Interface
Read rotary encoders with hardware acceleration.

### 3.1 Configuration
- **Mode:** Encoder Mode TI1 and TI2 (x4 resolution)
- **Filter:** 0x0 - 0xF (Essential for noise immunity)
- **ARR:** 0xFFFFFFFF (32-bit) or 0xFFFF (16-bit)

### 3.2 HAL Code
```cpp
HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

// Read Position (Direction handles automatically)
int32_t position = (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
```

---

## 4. One-Pulse Mode (OPM)
Generate a single precise pulse after a trigger (delay + width).

### 4.1 Configuration
- **Mode:** One Pulse Mode
- **Trigger:** External Pin or Internal TRGO
- **CCR1:** Delay before pulse starts
- **ARR:** Total duration (Delay + Pulse Width) - *Wait, check specific timer mode*
    - *Correction:* In OPM PWM 2:
    - **CCR1:** Defines delay length.
    - **ARR:** Defines total length (pulse ends at ARR).

### 4.2 HAL Code
```cpp
HAL_TIM_OnePulse_Init(&htim1, TIM_OPMODE_SINGLE);
HAL_TIM_OnePulse_Start(&htim1, TIM_CHANNEL_1);
```

---

## 5. High-Frequency Interrupts (Timebase)
Run control loops (PID, Motion).

### 5.1 Configuration
- **Timer:** Basic Timers (TIM6, TIM7) preferred.
- **Prescaler:** Set to 1us or 1ms tick.
- **ARR:** Set for desired Hz.

### 5.2 HAL Code
```cpp
HAL_TIM_Base_Start_IT(&htim6);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) {
        // Run Control Loop (Keep it short! <10us)
    }
}
```
