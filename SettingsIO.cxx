#include "SettingsIO.hpp"
#include "SettingsUser.hpp"
#include "core/hash.hpp"

#include <string_view>

namespace settings
{
SettingsIO::SettingsIO(Eeprom24LC64 &eeprom,
                       SettingsContainer &settings)
    : eeprom(eeprom),    //
      settings(settings) //
{
}

//----------------------------------------------------------------------------------------------
bool SettingsIO::loadSettings(bool shouldNotify)
{
    eeprom.read(MemoryOffset, reinterpret_cast<uint8_t *>(&rawContent), sizeof(EepromContent));

    // verify header
    bool isValid = (rawContent.magicString == Signature) &&             //
                   rawContent.settingsNamesHash == settingsNamesHash && //
                   rawContent.settingsValuesHash == hashSettingsValues();

    if (!isValid)
    {
        rawContent.settingsContainer.resetAllToDefault(false);
        saveSettings();
        return false;
    }

    // apply settings
    for (const auto &settingEntry : settings.getAllSettings().getEntries())
    {
        if (!settings.setValue(settingEntry.name,
                               rawContent.settingsContainer.getValue(settingEntry.name), false))
        {
            rawContent.settingsContainer.setValue(settingEntry.name, settingEntry.defaultValue,
                                                  false);
            saveSettings();
        }
    }

    if (shouldNotify)
        SettingsUser::notifySettingsUpdate();
    return true;
}

//----------------------------------------------------------------------------------------------
uint64_t SettingsIO::hashSettingsNames() const
{
    uint64_t hash = bus_node_base::HASH_SEED;
    for (const auto &settingEntry : settings.getAllSettings().getEntries())
    {
        hash = bus_node_base::fnvWithSeed(
            hash, reinterpret_cast<const uint8_t *>(std::begin(settingEntry.name)),
            reinterpret_cast<const uint8_t *>(std::end(settingEntry.name)));
    }
    return hash;
}

//----------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------
void SettingsIO::saveSettings()
{
    rawContent.magicString = Signature;
    rawContent.settingsNamesHash = settingsNamesHash;
    rawContent.settingsValuesHash = hashSettingsValues();

    for (const auto &settingEntry : settings.getAllSettings().getEntries())
    {
        const auto Value = settings.getValue(settingEntry.name);
        rawContent.settingsContainer.setValue(settingEntry.name, Value, false);
    }

    eeprom.write(MemoryOffset, reinterpret_cast<uint8_t *>(&rawContent), sizeof(EepromContent));
}
} // namespace settings