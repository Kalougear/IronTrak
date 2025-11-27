// HardwareTimer mock for native testing
#pragma once
#include <cstdint>

class HardwareTimer {
public:
    HardwareTimer(void* timer) { (void)timer; }
    
    void pause() {}
    void resume() {}
    void setOverflow(uint32_t val) { (void)val; }
    void setCount(uint32_t val) { _count = val; }
    uint32_t getCount() { return _count; }
    void* getHandle() { return nullptr; }
    
private:
    uint32_t _count = 0;
};

// STM32 HAL type mocks
typedef struct {
    void* Instance;
} TIM_HandleTypeDef;

typedef struct {
    uint32_t EncoderMode;
    uint32_t IC1Polarity;
    uint32_t IC1Selection;
    uint32_t IC1Prescaler;
    uint32_t IC1Filter;
    uint32_t IC2Polarity;
    uint32_t IC2Selection;
    uint32_t IC2Prescaler;
    uint32_t IC2Filter;
} TIM_Encoder_InitTypeDef;

typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
    uint32_t Alternate;
} GPIO_InitTypeDef;

// HAL constants
#define TIM_ENCODERMODE_TI12 0
#define TIM_ICPOLARITY_RISING 0
#define TIM_ICSELECTION_DIRECTTI 0
#define TIM_ICPSC_DIV1 0
#define TIM_CHANNEL_ALL 0
#define GPIO_MODE_AF_PP 0
#define GPIO_PULLUP 0
#define GPIO_SPEED_FREQ_HIGH 0
#define GPIO_AF2_TIM4 0
#define GPIO_PIN_6 (1<<6)
#define GPIO_PIN_7 (1<<7)
#define HAL_OK 0

// Mock GPIO port
struct GPIO_TypeDef { int dummy; };
extern GPIO_TypeDef GPIOB_MOCK;
#define GPIOB (&GPIOB_MOCK)

// Mock HAL functions
inline int HAL_TIM_Encoder_Init(TIM_HandleTypeDef* htim, TIM_Encoder_InitTypeDef* config) {
    (void)htim; (void)config;
    return HAL_OK;
}

inline void HAL_TIM_Encoder_Start(TIM_HandleTypeDef* htim, uint32_t channel) {
    (void)htim; (void)channel;
}

inline void HAL_GPIO_Init(GPIO_TypeDef* port, GPIO_InitTypeDef* init) {
    (void)port; (void)init;
}

// Mock clock enable (no-op)
#define __HAL_RCC_GPIOB_CLK_ENABLE()

// Mock pin defines
#define PB6 6
#define PB7 7

// Mock TIM4 pointer
extern void* TIM4;
