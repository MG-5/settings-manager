#pragma once

#include "SettingsEntryArray.hpp"
#include "SettingsUser.hpp"

namespace settings
{

template <size_t SettingsCount, const SettingsEntryArray<SettingsCount> &entryArray>
class AbstractSettingsContainer
{
public:
    using ContainerArray = std::array<std::pair<std::string_view, SettingsValue_t>, SettingsCount>;

    AbstractSettingsContainer()
    {
        for (size_t i = 0; i < SettingsCount; i++)
        {
            containerArray[i].first = entryArray[i].name;
            containerArray[i].second = entryArray[i].defaultValue;
        }
    };

    //----------------------------------------------------------------------------------------------
    [[nodiscard]] size_t size() const
    {
        return SettingsCount;
    }

    //----------------------------------------------------------------------------------------------
    template <typename T = SettingsValue_t>
    T getValue(std::string_view name) const
    {
        auto i = entryArray.getIndex(name);
        auto value = containerArray[i].second;

        if constexpr (std::is_same<T, SettingsValue_t>::value)
            return value;

        else
            return static_cast<T>(value);
    }

    //----------------------------------------------------------------------------------------------
    bool setValue(std::string_view name, const float newValue, const bool notify = true)
    {
        const auto Index = entryArray.getIndex(name);
        const auto MaxValue = entryArray[Index].maxValue;
        const auto MinValue = entryArray[Index].minValue;

        if (newValue > MaxValue || newValue < MinValue)
        {
            return false;
        }

        containerArray[Index].second = newValue;

        if (notify)
            SettingsUser::notifySettingsUpdate();

        return true;
    }

    //----------------------------------------------------------------------------------------------
    void resetAllToDefault()
    {
        for (size_t i = 0; i < SettingsCount; ++i)
        {
            containerArray[i].first = entryArray[i].name;
            containerArray[i].second = entryArray[i].defaultValue;
        }
        SettingsUser::notifySettingsUpdate();
    }

    //----------------------------------------------------------------------------------------------
    constexpr const SettingsEntryArray<SettingsCount> &getAllSettings() const
    {
        return entryArray;
    }

    //----------------------------------------------------------------------------------------------
    constexpr bool doesSettingExist(std::string_view name) const
    {
        for (auto &entry : containerArray)
        {
            if (entry.first.compare(name) == 0)
            {
                return true;
            }
        }
        return false;
    }

private:
    ContainerArray containerArray{};
};

} // namespace settings