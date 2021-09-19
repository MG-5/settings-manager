#pragma once
#include "FreeRTOS.h"
#include <cmsis_os2.h>

#include "SettingsContainer.hpp"

#include <i2c-drivers/24lcxx.hpp>
#include <i2c-drivers/rtos_accessor.hpp>

#include <memory>

namespace settings
{
class SettingsIO
{

public:
    SettingsIO(i2c::RtosAccessor &i2cAccessor, Eeprom24LC64 &eeprom, SettingsContainer &settings);

    /// Non-blocking. Writes settings to eeprom
    void saveSettings();

    static void signalFromISR(bool error);

private:
    i2c::RtosAccessor &i2cBusAccessor;
    Eeprom24LC64 &eeprom;
    SettingsContainer &settings;
    static SettingsIO *instance;

    static constexpr size_t Signature = 0x0110CA6E;

    struct EepromContent
    {
        uint64_t fieldsHash;
        uint64_t settingsHash;
        size_t magicString = Signature;
        SettingsContainer settingsContainer{};
    };

    static constexpr uint16_t MemoryOffset = 0;
    EepromContent rawContent;
    uint64_t settingsFieldNamesHash;

    void write();
    uint64_t hashSettingsValues() const;

    void run();
    static void taskMain(void *param);
};

} // namespace settings