# STM32 HAL Essentials (Universal)
> **Scope:** STM32Cube HAL  
> **Focus:** Robustness, Performance, and Non-Blocking I/O

## 1. DMA Circular Buffers (The "Zero CPU" Approach)
Essential for high-speed UART/SPI/ADC data streams.

### 1.1 Concept
DMA fills a ring buffer in the background. CPU processes data in "chunks" triggered by Half-Transfer (HT) and Transfer-Complete (TC) interrupts.

### 1.2 UART Receive Example
```cpp
#define RX_BUF_SIZE 256
uint8_t rxBuffer[RX_BUF_SIZE];

// Init
HAL_UART_Receive_DMA(&huart1, rxBuffer, RX_BUF_SIZE);

// Callbacks
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
    processData(&rxBuffer[0], RX_BUF_SIZE/2);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    processData(&rxBuffer[RX_BUF_SIZE/2], RX_BUF_SIZE/2);
}
```

---

## 2. UART Idle Line Detection
Process variable-length packets efficiently without fixed size.

### 2.1 Pattern
1.  Start DMA Circular Receive.
2.  Enable Idle Line Interrupt (`__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE)`).
3.  In ISR:
    - Check Idle Flag.
    - Calculate data length: `Received = BufferSize - __HAL_DMA_GET_COUNTER(hdma)`.
    - Process new data.
    - Clear Idle Flag.

---

## 3. Critical Sections & Atomicity
Protect shared data between ISRs and Main Loop.

### 3.1 Basic (Global Disable)
Fastest, but halts *everything*.
```cpp
uint32_t readAtomic() {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    uint32_t val = volatileVar;
    if (!primask) __enable_irq();
    return val;
}
```

### 3.2 Advanced (Basepri - Cortex-M3/4/7)
Only mask interrupts below a certain priority. Allows high-priority safety ISRs to keep running.
```cpp
// Mask interrupts with priority >= 0x50
__set_BASEPRI(0x50);
// Critical Code
__set_BASEPRI(0);
```

---

## 4. Error Handling Best Practices
Don't let the default `Error_Handler` just infinite loop.

### 4.1 Robust Handler
```cpp
void Error_Handler(void) {
    __disable_irq();
    
    // 1. Safe State: Turn off motors/heaters
    HAL_GPIO_WritePin(SAFE_PIN, 0);
    
    // 2. Log Error (if possible) to Flash/EEPROM
    logError(lastErrorCode);
    
    // 3. Auto-Reset (Watchdog will trigger, or manual)
    NVIC_SystemReset();
}
```

### 4.2 HAL Error Checks
Always check HAL returns.
```cpp
if (HAL_SPI_Transmit(&hspi1, data, len, 100) != HAL_OK) {
    // Handle Timeout/Error
    HAL_SPI_Abort(&hspi1);
    resetSPI();
}
```

---

## 5. Low Power Strategies
### 5.1 Sleep Mode
CPU stops, Peripherals run. Wakes on any interrupt.
```cpp
// In main loop idle
HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
```

### 5.2 Stop Mode
Clocks stopped. SRAM retained. Wakes on EXTI (GPIO, RTC).
```cpp
HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
// Re-config clocks after wake-up!
SystemClock_Config();
```
