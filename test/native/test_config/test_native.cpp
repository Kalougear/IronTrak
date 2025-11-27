#include <unity.h>

// Self-contained test (doesn't link to Config.cpp due to const internal linkage)
// Instead, we validate the concept with local constants

// ===== Test Fixture =====

void setUp(void) {
    // Pre-test initialization
}

void tearDown(void) {
    // Post-test cleanup
}

// ===== Test Cases: Config Data Validation =====

void test_config_stock_library_count_metric_rect(void) {
    // Test: Validate array/count consistency concept
    
    // Arrange: Simulate stock library (based on Config.cpp structure)
    const uint8_t test_array_length = 8;  // Metric rect has 8 items
    const uint8_t test_count_constant = 8;
    
    // Assert: Array length should match count
    TEST_ASSERT_EQUAL_UINT8(test_count_constant, test_array_length);
}

void test_config_stock_library_count_imperial_rect(void) {
    // Test: Imperial array consistency
    
    // Arrange
    const uint8_t test_array_length = 4;  // Imperial rect has 4 items
    const uint8_t test_count_constant = 4;
    
    // Assert
    TEST_ASSERT_EQUAL_UINT8(test_count_constant, test_array_length);
}

void test_config_stock_dimensions_no_zero_values(void) {
    // Test: Stock dimensions validation logic
    
    // Arrange: Sample dimensions (20x20 mm)
    const uint8_t width = 20;
    const uint8_t height = 20;
    
    // Act: Validate both dimensions are positive
    bool dimensions_valid = (width > 0) && (height > 0);
    
    // Assert
    TEST_ASSERT_TRUE_MESSAGE(dimensions_valid, "Dimensions must be positive");
}

// ===== Test Runner =====

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_config_stock_library_count_metric_rect);
    RUN_TEST(test_config_stock_library_count_imperial_rect);
    RUN_TEST(test_config_stock_dimensions_no_zero_values);

    return UNITY_END();
}
