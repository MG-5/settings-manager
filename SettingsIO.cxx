#include "SettingsIO.hpp"
#include "SettingsUser.hpp"
#include "SpecialAssert.hpp"
#include "core/hash.hpp"
#include "sync.hpp"
#include "task.h"

#include <string_view>

namespace settings
{
SettingsIO::SettingsIO(Eeprom24LC64 &eeprom,
                       SettingsContainer &settings)
    : eeprom(eeprom),    //
      settings(settings) //
{
}

void SettingsIO::init()
{
    // hash field names to automatically invalidate eeprom content if settings datastructure changed
    settingsFieldNamesHash = bus_node_base::HASH_SEED;

    for (const auto &settingEntry : settings.getAllSettings().getEntries())
    {
        settingsFieldNamesHash = bus_node_base::fnvWithSeed(
            settingsFieldNamesHash,
            reinterpret_cast<const uint8_t *>(std::begin(settingEntry.name)),
            reinterpret_cast<const uint8_t *>(std::end(settingEntry.name)));
    }

    // retrieve content
    bool readContentValid = true;
    bool memoryisIncomplete = false;
    eeprom.read(MemoryOffset, reinterpret_cast<uint8_t *>(&rawContent), sizeof(EepromContent));

    // apply settings without notifying
    for (const auto &settingEntry : settings.getAllSettings().getEntries())
    {
        if (!rawContent.settingsContainer.doesSettingExist(settingEntry.name))
        {
            memoryisIncomplete = true;
            continue;
        }

        if (!settings.setValue(settingEntry.name,
                               rawContent.settingsContainer.getValue(settingEntry.name), false))
            readContentValid = false;
    }

    // validate header
    if (readContentValid && !memoryisIncomplete)
    {
        readContentValid = (rawContent.magicString == Signature) &&
                           rawContent.fieldsHash == settingsFieldNamesHash &&
                           rawContent.settingsHash == hashSettingsValues();
    }

    if (!readContentValid || memoryisIncomplete)
    {
        rawContent.settingsContainer.resetAllToDefault();
        write();
    }
}

uint64_t SettingsIO::hashSettingsValues() const
{
    uint64_t hash = bus_node_base::HASH_SEED;
    for (const auto &settingEntry : settings.getAllSettings().getEntries())
    {
        SettingsValue_t value = settings.getValue(settingEntry.name);
        const auto ptr = reinterpret_cast<const uint8_t *>(&value);
        hash = bus_node_base::fnvWithSeed(hash, ptr, ptr + sizeof(value));
    }
    return hash;
}

void SettingsIO::saveSettings()
{
    write();
}

void SettingsIO::write()
{
    rawContent.magicString = Signature;
    rawContent.fieldsHash = settingsFieldNamesHash;
    rawContent.settingsHash = hashSettingsValues();

    for (const auto &settingEntry : settings.getAllSettings().getEntries())
    {
        const auto Value = settings.getValue(settingEntry.name);
        rawContent.settingsContainer.setValue(settingEntry.name, Value, false);
    }

    eeprom.write(MemoryOffset, reinterpret_cast<uint8_t *>(&rawContent), sizeof(EepromContent));
}
} // namespace settings