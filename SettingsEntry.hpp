#pragma once

#include "SettingsUser.hpp"
#include "SpecialAssert.hpp"

#include <cstring>

namespace settings
{
enum class Unit : uint8_t
{
    NoUnit = 0,
    Angle,
    Acceleration,
    Velocity,
    Percentage
};

using SettingsValue_t = float;

class SettingsEntry
{
    SettingsValue_t value;

public:
    const SettingsValue_t minValue;
    const SettingsValue_t defaultValue;
    const SettingsValue_t maxValue;
    const Unit unit;
    const char *name;

    SettingsEntry(const SettingsValue_t min, const SettingsValue_t defaultValue,
                  const SettingsValue_t max, const Unit unit, const char *name);

    bool set(SettingsValue_t rawValue, bool notify);
    SettingsValue_t get() const;

    bool operator==(const SettingsEntry &other) const;
    bool operator!=(const SettingsEntry &other) const;
    bool hasSameName(const char *otherName) const;
};
} // namespace settings