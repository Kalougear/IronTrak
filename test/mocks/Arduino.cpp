// Arduino API mock implementation for native testing
#include "Arduino.h"
#include <chrono>

static auto start_time = std::chrono::steady_clock::now();

unsigned long millis() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
    return static_cast<unsigned long>(duration.count());
}

void pinMode(uint8_t pin, uint8_t mode) {
    // Mock: No-op
    (void)pin;
    (void)mode;
}

void digitalWrite(uint8_t pin, uint8_t value) {
    // Mock: No-op
    (void)pin;
    (void)value;
}

int digitalRead(uint8_t pin) {
    // Mock: Always return HIGH
    (void)pin;
    return HIGH;
}

void delay(unsigned long ms) {
    // Mock: No-op (tests should not delay)
    (void)ms;
}
