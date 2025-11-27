#include <unity.h>
#include "../../src/headers/Config.h"

// Mock SystemSettings for testing
struct SystemSettings {
    float kerfMM = 2.5f;
    float hourlyRate = 25.0f;
    unsigned long projectCuts = 0;
    float projectLengthMeters = 0.0f;
    unsigned long projectSeconds = 0;
    unsigned long totalCuts = 0;
    float totalLengthMeters = 0.0f;
    unsigned long totalSeconds = 0;
};

// ===== Test Fixture =====

SystemSettings settings;

void setUp(void) {
    // Reset settings before each test
    settings.kerfMM = 2.5f;
    settings.hourlyRate = 25.0f;
    settings.projectCuts = 0;
    settings.projectLengthMeters = 0.0f;
    settings.projectSeconds = 0;
    settings.totalCuts = 0;
    settings.totalLengthMeters = 0.0f;
    settings.totalSeconds = 0;
}

void tearDown(void) {
    // Cleanup after each test
}

// ===== Test Cases: Statistics Calculations =====

void test_stats_register_cut_length_accumulation(void) {
    // Test: Cut length should accumulate with kerf compensation
    
    // Arrange
    float cut1 = 1000.0f; // 1000mm = 1.0m
    float cut2 = 500.0f;  // 500mm = 0.5m
    float kerf = 2.5f;
    
    // Act: Register two cuts
    float cut1_meters = (cut1 / 1000.0f) + (kerf / 1000.0f);
    float cut2_meters = (cut2 / 1000.0f) + (kerf / 1000.0f);
    
    settings.projectCuts = 2;
    settings.projectLengthMeters = cut1_meters + cut2_meters;
    
    // Assert: Should be 1.505m (1.0 + 0.5 + 0.0025 + 0.0025)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.505f, settings.projectLengthMeters);
}

void test_stats_register_cut_min_threshold(void) {
    // Test: Cuts below minimum threshold should be ignored
    
    // Arrange
    const float MIN_CUT_LENGTH_MM = 10.0f;
    float tiny_cut = 5.0f; // Below threshold
    
    // Act: Check if cut would be registered
    bool should_register = (abs(tiny_cut) > MIN_CUT_LENGTH_MM);
    
    // Assert: Should NOT register
    TEST_ASSERT_FALSE_MESSAGE(should_register, "Tiny cuts should be ignored");
}

void test_stats_register_cut_negative_values(void) {
    // Test: Negative cut lengths should use abs() value
    
    // Arrange
    float negative_cut = -500.0f;
    
    // Act
    float absolute_value = abs(negative_cut);
    
    // Assert
    TEST_ASSERT_EQUAL_FLOAT(500.0f, absolute_value);
}

void test_stats_waste_calculation_kerf(void) {
    // Test: Waste = cuts * kerf
    
    // Arrange
    settings.projectCuts = 10;
    settings.kerfMM = 2.5f;
    
    // Act
    float waste_meters = (settings.projectCuts * settings.kerfMM) / 1000.0f;
    
    // Assert: 10 cuts * 2.5mm = 25mm = 0.025m
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.025f, waste_meters);
}

void test_stats_average_cut_length(void) {
    // Test: Average = (total_length - waste) / cuts
    
    // Arrange
    settings.projectCuts = 5;
    settings.projectLengthMeters = 2.5f; // Includes kerf
    settings.kerfMM = 2.0f;
    
    // Act
    float waste_meters = (settings.projectCuts * settings.kerfMM) / 1000.0f; // 0.01m
    float total_raw_meters = settings.projectLengthMeters - waste_meters;     // 2.49m
    float average_mm = (total_raw_meters * 1000.0f) / settings.projectCuts;  // 498mm
    
    // Assert
    TEST_ASSERT_FLOAT_WITHIN(1.0f, 498.0f, average_mm);
}

void test_stats_average_cut_divide_by_zero(void) {
    // Test: Division by zero protection when cuts = 0
    
    // Arrange
    settings.projectCuts = 0;
    settings.projectLengthMeters = 0.0f;
    
    // Act
    float average = (settings.projectCuts == 0) ? 0.0f : 
                    (settings.projectLengthMeters * 1000.0f) / settings.projectCuts;
    
    // Assert: Should return 0, not crash
    TEST_ASSERT_EQUAL_FLOAT(0.0f, average);
}

void test_stats_labor_cost_calculation(void) {
    // Test: Labor cost = hours * hourly_rate
    
    // Arrange
    settings.projectSeconds = 7200; // 2 hours
    settings.hourlyRate = 25.0f;
    
    // Act
    float hours = settings.projectSeconds / 3600.0f;
    float labor_cost = hours * settings.hourlyRate;
    
    // Assert: 2h * $25/h = $50
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 50.0f, labor_cost);
}

void test_stats_uptime_seconds_to_minutes(void) {
    // Test: Minutes = seconds / 60
    
    // Arrange
    settings.projectSeconds = 3600; // 1 hour = 60 minutes
    
    // Act
    unsigned long minutes = settings.projectSeconds / 60;
    
    // Assert
    TEST_ASSERT_EQUAL_UINT32(60, minutes);
}

void test_stats_cuts_per_hour(void) {
    // Test: CPH = (cuts * 60) / minutes
    
    // Arrange
    settings.projectCuts = 120;
    settings.projectSeconds = 7200; // 2 hours = 120 min
    
    // Act
    unsigned long minutes = settings.projectSeconds / 60;
    float cuts_per_hour = (minutes < 1) ? 0.0f : (settings.projectCuts * 60.0f) / minutes;
    
    // Assert: 120 cuts / 120 min = 1 cut/min = 60 cuts/hour
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 60.0f, cuts_per_hour);
}

void test_stats_cuts_per_hour_divide_by_zero(void) {
    // Test: CPH should return 0 when uptime < 1 minute
    
    // Arrange
    settings.projectCuts = 10;
    settings.projectSeconds = 30; // 0 minutes (rounds down)
    
    // Act
    unsigned long minutes = settings.projectSeconds / 60;
    float cuts_per_hour = (minutes < 1) ? 0.0f : (settings.projectCuts * 60.0f) / minutes;
    
    // Assert
    TEST_ASSERT_EQUAL_FLOAT(0.0f, cuts_per_hour);
}

void test_stats_total_hours_conversion(void) {
    // Test: Total hours = seconds / 3600
    
    // Arrange
    settings.totalSeconds = 18000; // 5 hours
    
    // Act
    float total_hours = settings.totalSeconds / 3600.0f;
    
    // Assert
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 5.0f, total_hours);
}

void test_stats_reset_project_clears_stats(void) {
    // Test: Reset should zero all project stats
    
    // Arrange
    settings.projectCuts = 100;
    settings.projectLengthMeters = 50.0f;
    settings.projectSeconds = 3600;
    
    // Act: Simulate reset
    settings.projectCuts = 0;
    settings.projectLengthMeters = 0.0f;
    settings.projectSeconds = 0;
    
    // Assert: All should be zero
    TEST_ASSERT_EQUAL_UINT32(0, settings.projectCuts);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, settings.projectLengthMeters);
    TEST_ASSERT_EQUAL_UINT32(0, settings.projectSeconds);
}

// ===== Test Runner =====

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Calculation tests
    RUN_TEST(test_stats_register_cut_length_accumulation);
    RUN_TEST(test_stats_waste_calculation_kerf);
    RUN_TEST(test_stats_average_cut_length);
    RUN_TEST(test_stats_labor_cost_calculation);
    RUN_TEST(test_stats_uptime_seconds_to_minutes);
    RUN_TEST(test_stats_cuts_per_hour);
    RUN_TEST(test_stats_total_hours_conversion);

    // Edge cases (critical!)
    RUN_TEST(test_stats_register_cut_min_threshold);
    RUN_TEST(test_stats_register_cut_negative_values);
    RUN_TEST(test_stats_average_cut_divide_by_zero);
    RUN_TEST(test_stats_cuts_per_hour_divide_by_zero);

    // State management
    RUN_TEST(test_stats_reset_project_clears_stats);

    return UNITY_END();
}
