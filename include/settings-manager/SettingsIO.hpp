#pragma once

#include "settings-manager/SettingsContainer.hpp"

#include <core/hash.hpp>
#include <eeprom-driver/EepromBase.hpp>

#include <cstring>
#include <memory>
#include <vector>

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

    using memoryEntry = typename SettingsContainer<SettingsCount, entryArray>::memoryEntry;

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
        bool areValuesValid =
            rawContent.settingsValuesHash == hashSettingsValues(rawContent.settingsContainer);
        bool saveRequired = false;

        if (!isCountEqual)
        {
            saveRequired = true;
            if (rawContent.numberOfSettings > SettingsCount)
            {
                // old eeprom structure contained more settings than the current now
                // read the missing content afterwards
                std::unique_ptr<memoryEntry[]> oldEepromAdditionalContent(
                    new memoryEntry[rawContent.numberOfSettings]);

                constexpr size_t BytesInRawContent = sizeof(memoryEntry) * SettingsCount;
                const size_t BytesToAfterwardsRead =
                    (rawContent.numberOfSettings - SettingsCount) * sizeof(memoryEntry);

                std::memcpy(oldEepromAdditionalContent.get(),
                            rawContent.settingsContainer.getContainerArray().data(),
                            BytesInRawContent);

                eeprom.read(MemoryOffset + sizeof(EepromContent),
                            reinterpret_cast<uint8_t *>(oldEepromAdditionalContent.get()) +
                                BytesInRawContent,
                            BytesToAfterwardsRead);

                migrateSettings(std::move(oldEepromAdditionalContent), rawContent.numberOfSettings);
            }
            else
                migrateSettings();
        }
        else if (!areHashesHashEqual && areValuesValid)
        {
            // something changed in the hashes, that can be the order of settings
            // or one or more setting was replace by another one resp.
            migrateSettings();
            saveRequired = true;
        }
        else if (!areValuesValid)
        {
            // values are corrupt, reset EEPROM
            settings.resetAllToDefault();
            saveSettings();
            return false;
        }
        else
        {
            // copy temporary settings to persistent instance
            for (const auto &settingEntry : settings.getAllSettings())
            {
                if (!settings.setValue(settingEntry.name,
                                       rawContent.settingsContainer.getValue(settingEntry.name)))
                {
                    // read settings value is out of range, reset to default
                    settings.setValue(settingEntry.name, settingEntry.defaultValue);
                    rawContent.settingsContainer.setValue(settingEntry.name,
                                                          settingEntry.defaultValue);
                    saveRequired = true;
                }
            }
        }

        if (saveRequired)
            saveSettings();

        return true;
    }

    /// Writes settings to EEPROM. Blocking
    virtual void saveSettings()
    {
        rawContent.numberOfSettings = SettingsCount;
        rawContent.magicString = Signature;
        rawContent.settingsHashesHash = settingsHashesHash;

        // copy persistent settings to temporary instance
        const auto &settingsArray = settings.getAllSettings();
        for (size_t i = 0; i < SettingsCount; i++)
        {
            rawContent.settingsContainer.getContainerArray()[i].hash =
                settings.getContainerArray()[i].hash;

            const auto Value = settings.getValue(settingsArray[i].name);
            SafeAssert(rawContent.settingsContainer.setValue(settingsArray[i].name, Value));
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
        SettingsContainer<SettingsCount, entryArray> settingsContainer{};

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
                hash, reinterpret_cast<const uint8_t *>(&settingEntry.NameHash),
                reinterpret_cast<const uint8_t *>(&settingEntry.NameHash) + sizeof(uint64_t));
        }
        return hash;
    }

    const uint64_t settingsHashesHash = hashSettingsHashes();

    void migrateSettings(std::unique_ptr<memoryEntry[]> oldEepromAdditionalContent = nullptr,
                         size_t length = 0)
    {
        size_t numberOfEntries = 0;
        memoryEntry *pointerToContent = nullptr;

        if (oldEepromAdditionalContent == nullptr || length == 0)
        {
            numberOfEntries = SettingsCount;
            pointerToContent = rawContent.settingsContainer.getContainerArray().data();
        }
        else
        {
            // if old eeprom with more settings than the current exists,
            // then use this instead of rawContent
            numberOfEntries = length;
            pointerToContent = oldEepromAdditionalContent.get();
        }

        for (auto &entry : settings.getContainerArray())
            for (size_t i = 0; i < numberOfEntries; i++)
            {
                const auto Foo = pointerToContent[i].hash;
                if (entry.hash == Foo) // compare hashes
                {
                    // hash found - rescue old value
                    std::optional<size_t> index = settings.getIndexFromHash(entry.hash);
                    SafeAssert(index.has_value());
                    settings.setValue(index.value(), pointerToContent[i].value);
                    break;
                }
            }
    }
}; // namespace settings

} // namespace settings