#pragma once

#include "settings-manager/SettingsContainer.hpp"

#include <core/hash.hpp>
#include <eeprom-driver/EepromBase.hpp>

namespace settings
{

/// Handles saving non-static settings content to eeprom
/// @tparam SettingsCount number of settings
/// @tparam entryArray the array with size of SettingsCount - containing the setting entrys
/// @tparam MemoryType class with read/write access to any memory like EEPROM
template <size_t SettingsCount, const std::array<SettingsEntry, SettingsCount> &entryArray,
          class MemoryType>
class SettingsIO
{
public:
    SettingsIO(MemoryType &eeprom, SettingsContainer<SettingsCount, entryArray> &settings)
        : eeprom(eeprom),    //
          settings(settings) //
    {
    }
    virtual ~SettingsIO() = default;

    /// Loads settings from EEPROM. Blocking. Updates SettingsContainer with read values on success.
    /// Discards EEPROM content and writes defaults on failure.
    /// @return true on success, false otherwise
    virtual bool loadSettings()
    {
        eeprom.read(MemoryOffset, reinterpret_cast<uint8_t *>(&rawContent), sizeof(EepromContent));

        bool hasHeader = rawContent.magicString == Signature;
        if (!hasHeader)
        {
            // no valid header found, write default struct to EEPROM
            settings.resetAllToDefault();
            saveSettings();
            return false;
        }

        bool isCountEqual = rawContent.numberOfSettings == SettingsCount;
        bool areHashesHashEqual = rawContent.settingsHashesHash == settingsHashesHash;

        if (!isCountEqual)
        {
            // number of settings has been resized
        }

        // copy temporary settings to persistent instance
        bool saveRequired = false;
        for (const auto &settingEntry : settings.getAllSettings())
        {
            if (!settings.setValue(settingEntry.name,
                                   rawContent.settingsContainer.getValue(settingEntry.name)))
            {
                // read settings value is out of range, reset to default
                settings.setValue(settingEntry.name, settingEntry.defaultValue);
                rawContent.settingsContainer.setValue(settingEntry.name, settingEntry.defaultValue);
                saveRequired = true;
            }
        }
        if (saveRequired)
        {
            saveSettings();
        }
        return true;
    }

    /// Writes settings to EEPROM. Blocking
    virtual void saveSettings()
    {
        rawContent.magicString = Signature;
        rawContent.settingsHashesHash = settingsHashesHash;

        // copy persistent settings to temporary instance
        for (const auto &settingEntry : settings.getAllSettings())
        {
            const auto Value = settings.getValue(settingEntry.name);
            SafeAssert(rawContent.settingsContainer.setValue(settingEntry.name, Value));
        }
        rawContent.settingsValuesHash = hashSettingsValues(rawContent.settingsContainer);

        eeprom.write(MemoryOffset, reinterpret_cast<uint8_t *>(&rawContent), sizeof(EepromContent));
    }

    static constexpr size_t Signature = 0x0110CA6E;
    static constexpr uint16_t MemoryOffset = 0;
    struct EepromContent
    {
        // corruption unit test requires every member to be packed until the last one
        // but putting packed for the whole struct generates a warning
        // as optimisations to SettingsContainer may change its size
        // interfering with packing
        __attribute__((packed)) uint16_t numberOfSettings = SettingsCount;
        __attribute__((packed)) uint64_t settingsHashesHash = 0;
        __attribute__((packed)) uint64_t settingsValuesHash = 0;
        __attribute__((packed)) size_t magicString = Signature;
        SettingsContainer<SettingsCount, entryArray> settingsContainer;

        bool operator==(const EepromContent &other) const
        {
            return numberOfSettings == other.numberOfSettings &&
                   settingsHashesHash == other.settingsHashesHash &&
                   settingsValuesHash == other.settingsValuesHash &&
                   magicString == other.magicString && settingsContainer == other.settingsContainer;
        }

        bool operator!=(const EepromContent &other) const
        {
            return !((*this) == other);
        }
    };

    [[nodiscard]] static uint64_t
    hashSettingsValues(SettingsContainer<SettingsCount, entryArray> &container)
    {
        uint64_t hash = core::hash::HASH_SEED;
        for (const auto &settingEntry : container.getAllSettings())
        {
            SettingsValue_t value = container.getValue(settingEntry.name);
            const auto ptr = reinterpret_cast<const uint8_t *>(&value);
            hash = core::hash::fnvWithSeed(hash, ptr, ptr + sizeof(value));
        }
        return hash;
    }

private:
    MemoryType &eeprom;
    SettingsContainer<SettingsCount, entryArray> &settings;
    EepromContent rawContent;

    /// every setting has his own hash
    /// this function hashs over all setting hashes
    [[nodiscard]] static uint64_t hashSettingsHashes()
    {
        uint64_t hash = core::hash::HASH_SEED;
        for (const auto &settingEntry : entryArray)
        {
            hash = core::hash::fnvWithSeed(
                hash, reinterpret_cast<const uint8_t *>(std::begin(settingEntry.name)),
                reinterpret_cast<const uint8_t *>(std::end(settingEntry.name)));
        }
        return hash;
    }

    const uint64_t settingsHashesHash = hashSettingsHashes();
};

} // namespace settings