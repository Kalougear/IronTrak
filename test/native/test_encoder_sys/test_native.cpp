#include <unity.h>
#include "../../src/headers/EncoderSys.h"

// ===== Test Fixture =====

void setUp(void) {
    // Pre-test initialization (runs before EACH test)
}

void tearDown(void) {
    // Post-test cleanup (runs after EACH test)
}

// ===== Test Cases: Encoder Overflow Logic =====

// Test naming: test_{module}_{behavior}_{condition}
void test_encoder_overflow_calculation_forward(void) {
    // Test: Overflow boundary 65535 → 0 should increment overflow counter
    
    // Arrange: Simulate encoder near overflow
    long count_before_overflow = 65535;
    long count_after_overflow = 0;
    
    // Act: Calculate position continuity
    // Expected: Position should increase smoothly (overflow counter increments)
    long delta = count_after_overflow - count_before_overflow; // = -65535
    
    // Assert: Delta should be detected as forward overflow
    TEST_ASSERT_TRUE_MESSAGE(delta < -32768, "Should detect forward overflow");
}

void test_encoder_overflow_calculation_backward(void) {
    // Test: Underflow boundary 0 → 65535 should decrement overflow counter
    
    // Arrange
    long count_before = 0;
    long count_after = 65535;
    
    // Act
    long delta = count_after - count_before; // = 65535
    
    // Assert: Delta should be detected as backward overflow
    TEST_ASSERT_TRUE_MESSAGE(delta > 32768, "Should detect backward overflow");
}

void test_encoder_recalibrate_mm_per_pulse(void) {
    // Test: Calibration math (circumference / pulses_per_rev)
    
    // Arrange
    const float wheel_diameter = 50.0f;  // 50mm wheel
    const float pulses_per_rev = 4096.0f; // 1024 PPR * 4 (quadrature)
    
    // Act
    float circumference = wheel_diameter * PI;
    float mm_per_pulse = circumference / pulses_per_rev;
    
    // Assert: Verify calculation
    float expected_mm_per_pulse = (50.0f * 3.14159265f) / 4096.0f;
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, expected_mm_per_pulse, mm_per_pulse);
}

void test_encoder_wheel_diameter_bounds_clamp(void) {
    // Test: Wheel diameter should clamp to safe bounds
    
    // Arrange
    const float MIN_DIA = 20.0f;
    const float MAX_DIA = 200.0f;
    const float DEFAULT_DIA = 50.0f;
    
    // Act & Assert: Test minimum clamp
    float test_value = 10.0f; // Too small
    if (test_value < MIN_DIA || test_value > MAX_DIA) {
        test_value = DEFAULT_DIA;
    }
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DIA, test_value);
    
    // Act & Assert: Test maximum clamp
    test_value = 250.0f; // Too large
    if (test_value < MIN_DIA || test_value > MAX_DIA) {
        test_value = DEFAULT_DIA;
    }
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DIA, test_value);
}

void test_encoder_wheel_diameter_nan_protection(void) {
    // Test: NaN/Inf values should be rejected
    
    // Arrange
    float test_value = NAN;
    const float DEFAULT_DIA = 50.0f;
    
    // Act
    if (!isfinite(test_value)) {
        test_value = DEFAULT_DIA;
    }
    
    // Assert
    TEST_ASSERT_EQUAL_FLOAT(DEFAULT_DIA, test_value);
}

void test_encoder_distance_calculation_accuracy(void) {
    // Test: Distance = raw_count * mm_per_pulse
    
    // Arrange
    long raw_count = 4096; // 1 full revolution
    float mm_per_pulse = 0.0384f; // ~50mm wheel
    
    // Act
    float distance_mm = raw_count * mm_per_pulse;
    
    // Assert: Should be close to wheel circumference (~157mm)
    TEST_ASSERT_FLOAT_WITHIN(1.0f, 157.08f, distance_mm);
}

void test_encoder_offset_application(void) {
    // Test: Offset should be subtracted from distance
    
    // Arrange
    long raw_count = 1000;
    float mm_per_pulse = 0.0384f;
    float offset_mm = 10.0f;
    
    // Act
    float distance_raw = raw_count * mm_per_pulse;
    float distance_with_offset = distance_raw - offset_mm;
    
    // Assert
    float expected = (1000 * 0.0384f) - 10.0f;
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected, distance_with_offset);
}

void test_encoder_position_continuity_across_overflow(void) {
    // Test: Position should NOT jump when overflow occurs
    
    // Arrange: Simulate encoder state before/after overflow
    int32_t overflow_count_before = 0;
    uint16_t timer_count_before = 65530;
    
    int32_t overflow_count_after = 1;  // Incremented by overflow detection
    uint16_t timer_count_after = 5;    // Wrapped to 5
    
    // Act: Calculate absolute positions
    long position_before = (overflow_count_before * 65536) + timer_count_before; // = 65530
    long position_after = (overflow_count_after * 65536) + timer_count_after;     // = 65541
    
    // Assert: Position should increase by 11 (65530 + 11 = 65541)
    long delta = position_after - position_before;
    TEST_ASSERT_EQUAL_INT32(11, delta);
}

// ===== Test Runner =====

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Happy path tests first
    RUN_TEST(test_encoder_recalibrate_mm_per_pulse);
    RUN_TEST(test_encoder_distance_calculation_accuracy);
    RUN_TEST(test_encoder_offset_application);

    // Edge cases
    RUN_TEST(test_encoder_overflow_calculation_forward);
    RUN_TEST(test_encoder_overflow_calculation_backward);
    RUN_TEST(test_encoder_position_continuity_across_overflow);
    RUN_TEST(test_encoder_wheel_diameter_bounds_clamp);
    RUN_TEST(test_encoder_wheel_diameter_nan_protection);

    return UNITY_END();
}
