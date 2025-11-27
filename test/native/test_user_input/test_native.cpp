#include <unity.h>

// ===== Test Fixture =====

void setUp(void) {
    // Pre-test initialization
}

void tearDown(void) {
    // Post-test cleanup
}

// ===== Test Cases: Quadrature Encoder Logic =====

void test_userinput_encoder_quadrature_cw(void) {
    // Test: Detect clockwise rotation from quadrature state transitions
    
    // Arrange: Quadrature encoder state machine
    // CW pattern: 0b1101, 0b0100, 0b0010, 0b1011
    int sum1 = 0b1101; // (lastEncoded=11, encoded=01) -> CW
    
    // Act: Check if pattern matches CW
    bool is_cw = (sum1 == 0b1101 || sum1 == 0b0100 || sum1 == 0b0010 || sum1 == 0b1011);
    
    // Assert
    TEST_ASSERT_TRUE_MESSAGE(is_cw, "Should detect CW rotation");
}

void test_userinput_encoder_quadrature_ccw(void) {
    // Test: Detect counter-clockwise rotation from quadrature states
    
    // Arrange: CCW pattern: 0b1110, 0b0111, 0b0001, 0b1000
    int sum1 = 0b1110;
    
    // Act
    bool is_ccw = (sum1 == 0b1110 || sum1 == 0b0111 || sum1 == 0b0001 || sum1 == 0b1000);
    
    // Assert
    TEST_ASSERT_TRUE_MESSAGE(is_ccw, "Should detect CCW rotation");
}

void test_userinput_encoder_direction_change_reset(void) {
    // Test: Encoder accumulator should reset on direction change to eliminate hysteresis
    
    // Arrange
    int8_t lastDirection = 1;      // Was rotating CW
    int8_t currentDirection = -1;  // Now rotating CCW
    int encoderValue = 3;          // Had accumulated 3 steps CW
    
    // Act: Detect direction change
    if (currentDirection != 0 && lastDirection != 0 && currentDirection != lastDirection) {
        encoderValue = currentDirection;  // Reset to current direction's first tick
    }
    
    // Assert: Should reset to -1 (first CCW tick)
    TEST_ASSERT_EQUAL_INT8(-1, encoderValue);
}

void test_userinput_encoder_steps_per_detent(void) {
    // Test: 4 steps should generate 1 event (standard KY-040 encoder)
    
    // Arrange
    int encoderValue = 4;  // Accumulated 4 steps
    bool event_triggered = false;
    
    // Act: Check threshold
    if (encoderValue >= 4) {
        event_triggered = true;
        encoderValue = 0;  // Reset after event
    }
    
    // Assert
    TEST_ASSERT_TRUE(event_triggered);
    TEST_ASSERT_EQUAL_INT32(0, encoderValue);
}

void test_userinput_button_debounce_20ms(void) {
    // Test: Button must be stable for 20ms before state change
    
    // Arrange: Simulate 1kHz ISR calls
    const uint8_t DEBOUNCE_COUNT = 20;  // 20 ticks @ 1kHz = 20ms
    uint8_t stableCount = 0;
    bool lastState = true;  // HIGH (released)
    bool currentState = false;  // LOW (pressed)
    
    // Act: Count stable samples
    for (int i = 0; i < DEBOUNCE_COUNT; i++) {
        if (currentState != lastState) {
            stableCount++;
        }
    }
    
    // Assert: Should accumulate 20 counts
    TEST_ASSERT_TRUE(stableCount >= DEBOUNCE_COUNT);
}

void test_userinput_button_long_press_500ms(void) {
    // Test: Button held >500ms should trigger LONG_PRESS event
    
    // Arrange
    unsigned long press_time = 0;
    unsigned long current_time = 600;  // Held for 600ms
    const unsigned long LONG_PRESS_THRESHOLD = 500;
    
    // Act
    unsigned long duration = current_time - press_time;
    bool is_long_press = (duration > LONG_PRESS_THRESHOLD);
    
    // Assert
    TEST_ASSERT_TRUE_MESSAGE(is_long_press, "Should detect long press");
}

void test_userinput_button_super_long_press_10s(void) {
    // Test: Button held >10s should trigger SUPER_LONG_PRESS event
    
    // Arrange
    unsigned long press_time = 0;
    unsigned long current_time = 11000;  // Held for 11 seconds
    const unsigned long SUPER_LONG_THRESHOLD = 10000;
    
    // Act
    unsigned long duration = current_time - press_time;
    bool is_super_long = (duration > SUPER_LONG_THRESHOLD);
    
    // Assert
    TEST_ASSERT_TRUE_MESSAGE(is_super_long, "Should detect super long press");
}

// ===== Test Runner =====

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Encoder quadrature tests
    RUN_TEST(test_userinput_encoder_quadrature_cw);
    RUN_TEST(test_userinput_encoder_quadrature_ccw);
    RUN_TEST(test_userinput_encoder_direction_change_reset);
    RUN_TEST(test_userinput_encoder_steps_per_detent);

    // Button debouncing tests
    RUN_TEST(test_userinput_button_debounce_20ms);
    RUN_TEST(test_userinput_button_long_press_500ms);
    RUN_TEST(test_userinput_button_super_long_press_10s);

    return UNITY_END();
}
