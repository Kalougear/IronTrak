#pragma once

// Minimal Arduino API mock for native testing
#include <cstdint>
#include <cmath>

// Mock millis()
unsigned long millis();

// Mock pinMode/digitalWrite/digitalRead
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define HIGH 1
#define LOW 0

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
int digitalRead(uint8_t pin);

// Mock PI constant
#ifndef PI
#define PI 3.14159265358979323846f
#endif

// Mock delay
void delay(unsigned long ms);

// Mock abs (use std::abs)
using std::abs;
using std::isfinite;
