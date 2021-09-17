#include "SettingsEntry.hpp"

namespace settings
{
SettingsEntry::SettingsEntry(const SettingsValue_t min, const SettingsValue_t defaultValue,
                             const SettingsValue_t max, const Unit unit, const char *name)
    : value{defaultValue}, minValue{min},
      defaultValue{defaultValue}, maxValue{max}, unit{unit}, name{name}
{
    specialAssert(minValue <= maxValue);
    specialAssert(defaultValue <= maxValue);
    specialAssert(defaultValue >= minValue);
}

//--------------------------------------------------------------------------------------------------
bool SettingsEntry::set(SettingsValue_t rawValue, bool notify = true)
{
    if (rawValue > maxValue || rawValue < minValue)
    {
        return false;
    }

    value = rawValue;
    if (notify)
    {
        SettingsUser::notifySettingsUpdate();
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
SettingsValue_t SettingsEntry::get() const
{
    return value;
}

//--------------------------------------------------------------------------------------------------
bool SettingsEntry::operator==(const SettingsEntry &other) const
{
    return strcmp(name, other.name) == 0;
}

//--------------------------------------------------------------------------------------------------
bool SettingsEntry::operator!=(const SettingsEntry &other) const
{
    return !((*this) == other);
}

//--------------------------------------------------------------------------------------------------
bool SettingsEntry::hasSameName(const char *otherName) const
{
    return strcmp(name, otherName) == 0;
}
} // namespace settings