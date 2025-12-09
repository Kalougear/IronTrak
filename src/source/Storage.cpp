#include "../headers/Storage.h"

// Use FlashStorage_STM32 library - the recommended solution for STM32 Flash storage
// This library properly handles Flash page erase and wear leveling
#include <FlashStorage_STM32.h>

#if defined(STM32F4xx)
#include <IWatchdog.h>
#endif

// Define Magic Number to check initialization
#define STORAGE_MAGIC 0x42
#define STORAGE_OFFSET 4 // Leave space for Magic + Version

void Storage::init()
{
    // FlashStorage_STM32 uses EEPROM-like API
    // No explicit begin() needed - library auto-initializes
    Serial1.println("Storage::init() - FlashStorage_STM32 ready");
}

void Storage::load(SystemSettings *settings)
{
    uint8_t magic = EEPROM.read(0);
    Serial1.print("Storage::load() Magic=0x");
    Serial1.println(magic, HEX);
    
    if (magic != STORAGE_MAGIC)
    {
        // First run or corrupted - Keep defaults
        Serial1.println("Storage: No valid magic, using defaults");
        return;
    }

    EEPROM.get(STORAGE_OFFSET, *settings);
    Serial1.print("Storage: Loaded OK. Dir=");
    Serial1.println(settings->reverseDirection);
}

void Storage::save(const SystemSettings &settings)
{
    Serial1.println("Storage::save() Starting...");
    
    // Kick watchdog before potentially slow Flash operation
    #if defined(STM32F4xx)
    IWatchdog.reload();
    #endif

    // Write Magic if not present
    if (EEPROM.read(0) != STORAGE_MAGIC)
    {
        EEPROM.write(0, STORAGE_MAGIC);
        Serial1.println("Storage: Magic written");
    }

    // Write settings struct to Flash
    EEPROM.put(STORAGE_OFFSET, settings);
    
    Serial1.print("Storage: Data written. Dir=");
    Serial1.println(settings.reverseDirection);

    // *** CRITICAL: Commit changes to Flash ***
    // FlashStorage_STM32 requires explicit commit() call
    EEPROM.commit();
    Serial1.println("Storage: commit() called");

    // Kick watchdog after Flash operation
    #if defined(STM32F4xx)
    IWatchdog.reload();
    #endif

    Serial1.println("Storage::save() Done");
}
