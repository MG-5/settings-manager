#pragma once

#include "SettingsEntry.hpp"
#include "SettingsUser.hpp"

#include <array>
#include <cstdlib>

namespace settings
{

template <size_t SettingsCount>
class AbstractSettingsContainer
{
public:
    using SettingsArray_t = std::array<SettingsEntry, SettingsCount>;

    AbstractSettingsContainer(SettingsArray_t settingsArray) : settingsArray(settingsArray)
    {
        // check for duplicates
        for (auto &i : settingsArray)
        {
            for (auto &j : settingsArray)
            {
                if (std::addressof(i) == std::addressof(j))
                {
                    continue;
                }
                specialAssert(i != j);
            }
        }
    }

    ~AbstractSettingsContainer() = default;

    //----------------------------------------------------------------------------------------------
    bool doesSettingExist(const char *name) const
    {
        for (auto &entry : settingsArray)
        {
            if (entry.hasSameName(name))
            {
                return true;
            }
        }
        return false;
    }

    //----------------------------------------------------------------------------------------------
    const SettingsEntry &get(const char *name)
    {
        return getNonConst(name);
    }

    //----------------------------------------------------------------------------------------------
    bool set(const char *name, const float newValue, const bool notify = true)
    {
        return getNonConst(name).set(newValue, notify);
    }

    //----------------------------------------------------------------------------------------------
    void resetAllToDefault()
    {
        for (uint8_t i = 0; i < settingsArray.size(); ++i)
        {
            // only notify on last parameter to avoid spam
            settingsArray[i].set(settingsArray[i].defaultValue, i == settingsArray.size() - 1);
        }
    }

    //----------------------------------------------------------------------------------------------
    const SettingsArray_t &getAllSettings() const
    {
        return settingsArray;
    }

private:
    SettingsArray_t settingsArray;
    SettingsEntry &getNonConst(const char *name)
    {
        for (auto &entry : settingsArray)
        {
            if (entry.hasSameName(name))
            {
                return entry;
            }
        }
        specialAssert(false);
        return settingsArray[0];
    }
};

} // namespace settings