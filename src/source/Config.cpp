#include "../headers/Config.h"

// ============================================================================
// STOCK LIBRARY (Metric)
// ============================================================================
const char* STOCK_RECT_MM[] = {
    "20x20", "20x40", "25x25", "30x30", "35x35", "40x40", "50x50"
};
const uint8_t STOCK_RECT_MM_DIMS[][2] = {
    {20,20}, {20,40}, {25,25}, {30,30}, {35,35}, {40,40}, {50,50}
};
const uint8_t STOCK_RECT_MM_COUNT = 7;

const char* STOCK_ANGLE_MM[] = {
    "L20x20x3", "L25x25x3", "L30x30x3", "L40x40x4", "L50x50x5"
};
const uint8_t STOCK_ANGLE_MM_DIMS[][3] = {
    {20,20,3}, {25,25,3}, {30,30,3}, {40,40,4}, {50,50,5}
};
const uint8_t STOCK_ANGLE_MM_COUNT = 5;

const char* STOCK_CYL_MM[] = {
    "D16", "D20", "D25", "D30", "D40"
};
const uint8_t STOCK_CYL_MM_DIMS[] = {16, 20, 25, 30, 40};
const uint8_t STOCK_CYL_MM_COUNT = 5;

// ============================================================================
// STOCK LIBRARY (Imperial)
// ============================================================================
const char* STOCK_RECT_IN[] = {
    "1/2x1/2", "1x1", "1x2", "2x2"
};
const float STOCK_RECT_IN_DIMS[][2] = {
    {12.7,12.7}, {25.4,25.4}, {25.4,50.8}, {50.8,50.8}
};
const uint8_t STOCK_RECT_IN_COUNT = 4;

const char* STOCK_ANGLE_IN[] = {
    "L1x1x1/8", "L1.5x1.5x1/8", "L2x2x1/8"
};
const float STOCK_ANGLE_IN_DIMS[][3] = {
    {25.4,25.4,3.2}, {38.1,38.1,3.2}, {50.8,50.8,3.2}
};
const uint8_t STOCK_ANGLE_IN_COUNT = 3;

const char* STOCK_CYL_IN[] = {
    "D1/2", "D3/4", "D1", "D1.5"
};
const float STOCK_CYL_IN_DIMS[] = {12.7, 19.05, 25.4, 38.1};
const uint8_t STOCK_CYL_IN_COUNT = 4;
