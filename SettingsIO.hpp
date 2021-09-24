#pragma once

#include "SettingsContainer.hpp"
#include <i2c-drivers/24lcxx.hpp>

namespace settings
{
class SettingsIO
{

public:
    SettingsIO(Eeprom24LC64 &eeprom, SettingsContainer &settings);

    void init();

    /// Read and verify settings from eeprom, returns true if valid
    bool loadSettings(bool shouldNotify = true);

    /// Non-blocking. Writes settings to eeprom
    void saveSettings();

private:
    Eeprom24LC64 &eeprom;
    SettingsContainer &settings;

    static constexpr size_t Signature = 0x0110CA6E;

    struct EepromContent
    {
        uint64_t settingsNamesHash;
        uint64_t settingsValuesHash;
        size_t magicString = Signature;
        SettingsContainer settingsContainer{};
    };

    static constexpr uint16_t MemoryOffset = 0;
    EepromContent rawContent;

    uint64_t hashSettingsNames() const;
    uint64_t hashSettingsValues() const;

    const uint64_t settingsNamesHash = hashSettingsNames();
};

} // namespace settings