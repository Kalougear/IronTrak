#include "headers/EncoderSys.h"

EncoderSys::EncoderSys() {
    _wheelDiameter = DEFAULT_WHEEL_DIA_MM;
    _offsetMM = 0.0f;
    _reverseDirection = false;
    recalculateCalibration();
    
#if defined(STM32F4xx)
    _timer = nullptr;
    _overflowCount = 0;
    _lastTimerCount = 0;
#else
    _encoder = nullptr;
#endif
}

void EncoderSys::init() {
#if defined(STM32F4xx)
    // 1. Configure Pins FIRST (Enable Pull-ups)
    pinMode(PB6, INPUT_PULLUP);
    pinMode(PB7, INPUT_PULLUP);


    // 2. Initialize Hardware Timer
    // GEMINI.md Rule 4.2: ZERO dynamic allocation - use static instance
    static HardwareTimer timerInstance(TIM4);
    _timer = &timerInstance;
    
    
    // 3. EXPLICITLY Force GPIO to Alternate Function Mode (AF2 for TIM4)
    // pinMode sets them to Input, which disconnects the timer. We must reconnect it.
    // GEMINI.md: Explicit struct initialization to satisfy -Wextra
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = 0,
        .Mode = 0,
        .Pull = 0,
        .Speed = 0,
        .Alternate = 0
    };
    
    // Enable GPIOB Clock
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // Configure PB6 & PB7 as Alternate Function (TIM4)
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // Keep pull-ups enabled!
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    // HardwareTimer doesn't expose encoder mode directly, so we configure via HAL
    // Stop the timer first
    _timer->pause();
    
    // Get the HAL timer handle (returns pointer)
    TIM_HandleTypeDef *halTimer = _timer->getHandle();
    
    // Configure encoder mode (both TI1 and TI2 on both edges)
    TIM_Encoder_InitTypeDef encoderConfig;
    encoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;  // Count on both edges of both channels
    encoderConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    encoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    encoderConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    encoderConfig.IC1Filter = 0x0F; // Add some filtering (0-15)
    encoderConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    encoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    encoderConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    encoderConfig.IC2Filter = 0x0F; // Add some filtering
    
    
    // GEMINI.md Rule 3.1: Return value checking with explicit recovery
    if (HAL_TIM_Encoder_Init(halTimer, &encoderConfig) != HAL_OK) {
        #if defined(USE_SERIAL_DEBUG)
        Serial1.println("ERROR: Encoder init failed!");
        #endif
        // Recovery: Halt system - watchdog will reset
        while(1);
    }
    
    
    // Start the encoder!
    HAL_TIM_Encoder_Start(halTimer, TIM_CHANNEL_ALL);
    
    // Set max count (16-bit timer)
    _timer->setOverflow(65535);
    _timer->setCount(0);
    _timer->resume();
    
    // Start encoder mode
    HAL_TIM_Encoder_Start(halTimer, TIM_CHANNEL_ALL);
    
    // Initialize state
    _timer->setCount(0);
    _lastTimerCount = 0;
    _overflowCount = 0;
#else
    // AVR Software Interrupt Implementation
    // GEMINI.md Rule 4.2: ZERO dynamic allocation - use static instance
    static Encoder encoderInstance(PIN_ENCODER_A, PIN_ENCODER_B);
    _encoder = &encoderInstance;

#endif
    
    reset();
}

void EncoderSys::update() {
#if defined(STM32F4xx)
    // Handle 16-bit Timer Overflow/Underflow
    // This must be called frequently (at least once every few seconds)
    if (_timer == nullptr) return;
    
    uint16_t currentTimerCount = _timer->getCount();
    int32_t diff = currentTimerCount - _lastTimerCount;
    
    // Detect wrap-around
    // If diff is very negative (e.g. 0 - 65535 = -65535), we wrapped forward
    // If diff is very positive (e.g. 65535 - 0 = 65535), we wrapped backward
    // Threshold is half range (32768)
    
    if (diff < -32768) {
        _overflowCount++;
    } else if (diff > 32768) {
        _overflowCount--;
    }
    
    _lastTimerCount = currentTimerCount;
#endif
}

void EncoderSys::reset() {
#if defined(STM32F4xx)
    if (_timer != nullptr) {
        _timer->setCount(0);
        _lastTimerCount = 0;
        _overflowCount = 0;
    }
#else
    if (_encoder != nullptr) {
        _encoder->write(0);
    }
#endif
    _lastCount = 0;
    _offsetMM = 0.0f;
}

long EncoderSys::getRawCount() {
#if defined(STM32F4xx)
    if (_timer == nullptr) return 0;
    
    // Update overflow tracking first to ensure consistency
    update();
    
    // Calculate total count: (Overflows * 65536) + CurrentCount
    return (_overflowCount * 65536) + _timer->getCount();
#else
    if (_encoder == nullptr) return 0;
    return _encoder->read();
#endif
}

float EncoderSys::getDistanceMM() {
    long raw = getRawCount();
    // Apply direction reversal if enabled
    float direction = _reverseDirection ? -1.0f : 1.0f;
    return (raw * _mmPerPulse * direction) - _offsetMM;
}

void EncoderSys::setWheelDiameter(float diameterMM) {
    // GEMINI.md Rule 4.2: Assertion 1 - Range validation
    if (diameterMM < MIN_WHEEL_DIA_MM || diameterMM > MAX_WHEEL_DIA_MM) {
        // Recovery: Clamp to safe default instead of crashing
        diameterMM = DEFAULT_WHEEL_DIA_MM;
    }
    
    // GEMINI.md Rule 4.2: Assertion 2 - Verify not NaN/Inf
    if (!isfinite(diameterMM)) {
        diameterMM = DEFAULT_WHEEL_DIA_MM;
    }
    
    _wheelDiameter = diameterMM;
    recalculateCalibration();
}


float EncoderSys::getWheelDiameter() {
    return _wheelDiameter;
}

void EncoderSys::setOffset(float offsetMM) {
    _offsetMM = offsetMM;
}

void EncoderSys::setReverseDirection(bool reverse) {
    _reverseDirection = reverse;
}

void EncoderSys::recalculateCalibration() {
    float circumference = _wheelDiameter * PI;
    _mmPerPulse = circumference / PULSES_PER_REV;
}
