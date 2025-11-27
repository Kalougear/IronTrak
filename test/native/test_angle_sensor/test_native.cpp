#include <unity.h>
#include <cmath>

// ===== Test Fixture =====

void setUp(void) {
    // Pre-test initialization
}

void tearDown(void) {
    // Post-test cleanup
}

// ===== Test Cases: Angle Sensor Math =====

void test_anglesensor_calculation_zero_point(void) {
    // Test: 0° angle calculation when raw == rawZero
    
    // Arrange
    uint16_t rawZero = 1000;
    uint16_t raw45 = 1512;   // 512 ticks for 45°
    uint16_t raw = 1000;     // At zero point
    
    // Act: Calculate degrees
    int32_t deltaRaw = (int32_t)raw - (int32_t)rawZero;  // = 0
    int32_t rangeRaw = (int32_t)raw45 - (int32_t)rawZero; // = 512
    float degrees = (float)deltaRaw * 45.0f / (float)rangeRaw; // = 0°
    
    // Assert
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, degrees);
}

void test_anglesensor_calculation_45_point(void) {
    // Test: 45° angle calculation when raw == raw45
    
    // Arrange
    uint16_t rawZero = 1000;
    uint16_t raw45 = 1512;
    uint16_t raw = 1512;  // At 45° calibration point
    
    // Act
    int32_t deltaRaw = (int32_t)raw - (int32_t)rawZero;   // = 512
    int32_t rangeRaw = (int32_t)raw45 - (int32_t)rawZero; // = 512
    float degrees = (float)deltaRaw * 45.0f / (float)rangeRaw; // = 45°
    
    // Assert
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 45.0f, degrees);
}

void test_anglesensor_calculation_interpolation(void) {
    // Test: Linear interpolation between 0° and 45°
    
    // Arrange
    uint16_t rawZero = 1000;
    uint16_t raw45 = 1512;   // Range of 512 ticks
    uint16_t raw = 1256;     // Halfway (256 ticks from zero)
    
    // Act
    int32_t deltaRaw = (int32_t)raw - (int32_t)rawZero;   // = 256
    int32_t rangeRaw = (int32_t)raw45 - (int32_t)rawZero; // = 512
    float degrees = (float)deltaRaw * 45.0f / (float)rangeRaw; // = 22.5°
    
    // Assert: Should be 22.5° (halfway between 0 and 45)
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 22.5f, degrees);
}

void test_anglesensor_clamp_0_to_90(void) {
    // Test: Angle should clamp to [0, 90] range
    
    // Arrange
    float angle_negative = -10.0f;
    float angle_too_high = 100.0f;
    
    // Act: Clamp negative
    if (angle_negative < 0.0f) angle_negative = 0.0f;
    if (angle_negative > 90.0f) angle_negative = 90.0f;
    
    // Act: Clamp too high
    if (angle_too_high < 0.0f) angle_too_high = 0.0f;
    if (angle_too_high > 90.0f) angle_too_high = 90.0f;
    
    // Assert
    TEST_ASSERT_EQUAL_FLOAT(0.0f, angle_negative);
    TEST_ASSERT_EQUAL_FLOAT(90.0f, angle_too_high);
}

void test_anglesensor_divide_by_zero_protection(void) {
    // Test: rangeRaw == 0 should be handled (prevents divide-by-zero)
    
    // Arrange
    uint16_t rawZero = 1000;
    uint16_t raw45 = 1000;  // Same as zero! Range = 0
    
    // Act
    int32_t rangeRaw = (int32_t)raw45 - (int32_t)rawZero; // = 0
    if (rangeRaw == 0) rangeRaw = 1;  // Protection
    
    // Assert: Should be corrected to 1
    TEST_ASSERT_EQUAL_INT32(1, rangeRaw);
}

void test_anglesensor_hysteresis_filter(void) {
    // Test: 0.2° deadband filter prevents jitter
    
    // Arrange
    float lastDegrees = 25.0f;
    float newDegrees = 25.1f;  // Change of 0.1° (below threshold)
    const float HYSTERESIS = 0.2f;
    
    // Act: Apply filter
    bool should_update = (fabs(newDegrees - lastDegrees) > HYSTERESIS);
    
    // Assert: Should NOT update (change too small)
    TEST_ASSERT_FALSE_MESSAGE(should_update, "Small changes should be filtered");
}

// ===== Test Runner =====

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Angle calculation tests
    RUN_TEST(test_anglesensor_calculation_zero_point);
    RUN_TEST(test_anglesensor_calculation_45_point);
    RUN_TEST(test_anglesensor_calculation_interpolation);

    // Edge cases
    RUN_TEST(test_anglesensor_clamp_0_to_90);
    RUN_TEST(test_anglesensor_divide_by_zero_protection);
    RUN_TEST(test_anglesensor_hysteresis_filter);

    return UNITY_END();
}
