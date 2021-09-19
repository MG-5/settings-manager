#include "SettingsIO.hpp"
#include "SettingsUser.hpp"
#include "SpecialAssert.hpp"
#include "core/hash.hpp"
#include "sync.hpp"
#include "task.h"

#include <string_view>

namespace settings
{

SettingsIO *SettingsIO::instance{nullptr};

SettingsIO::SettingsIO(i2c::RtosAccessor &i2cAccessor, Eeprom24LC64 &eeprom,
                       SettingsContainer &settings)
    : i2cBusAccessor(i2cAccessor), //
      eeprom(eeprom),              //
      settings(settings)           //
{
    specialAssert(instance == nullptr);
    instance = this;

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
    eeprom.read(MemoryOffset, reinterpret_cast<uint8_t *>(&rawContent), sizeof(EepromContent));

    // apply settings without notifying
    for (const auto &settingEntry : settings.getAllSettings().getEntries())
    {
        if (!rawContent.settingsContainer.getAllSettings().doesSettingExist(settingEntry.name))
            continue;

        readContentValid = settings.setValue(
            settingEntry.name, rawContent.settingsContainer.getValue(settingEntry.name), false);

        if (!readContentValid)
            break;
    }

    // validate header
    if (readContentValid)
    {
        readContentValid = (rawContent.magicString == Signature) &&
                           rawContent.fieldsHash == settingsFieldNamesHash &&
                           rawContent.settingsHash == hashSettingsValues();
    }

    if (!readContentValid)
    {
        rawContent.settingsContainer.resetAllToDefault();
        write();
    }
}

void SettingsIO::run()
{
    SettingsUser::notifySettingsUpdate();
    for (;;)
    {
        // wait for call to saveSettings
        //_task.notifyTake(portMAX_DELAY);
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
    // _task.notifyGive();
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

void SettingsIO::signalFromISR(bool error)
{
    auto higherPrioTaskWoken = pdFALSE;
    if (error)
    {
        instance->i2cBusAccessor.signalErrorFromIsr(&higherPrioTaskWoken);
    }
    else
    {
        instance->i2cBusAccessor.signalTransferCompleteFromIsr(&higherPrioTaskWoken);
    }
    portYIELD_FROM_ISR(higherPrioTaskWoken);
}

void SettingsIO::taskMain(void *param)
{
    auto inst = reinterpret_cast<SettingsIO *>(param);
    inst->run();
    for (;;)
    {
    }
}

} // namespace settings