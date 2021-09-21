#pragma once

#include "SpecialAssert.hpp"

#include <stdexcept>
#include <string_view>

namespace settings
{
using SettingsValue_t = float;

class SettingsEntry
{
public:
    const SettingsValue_t minValue;
    const SettingsValue_t defaultValue;
    const SettingsValue_t maxValue;
    const std::string_view name;

    constexpr SettingsEntry(const SettingsValue_t min, const SettingsValue_t defaultValue,
                            const SettingsValue_t max, std::string_view name)
        : minValue{min}, defaultValue{defaultValue}, maxValue{max}, name{name}
    {
        // if these checks are triggered at compile time you will get errors which say that
        // std::runtime_error is not a constexpr function
        // to get rid this errors make sure your min, default and max values are correct ;)

        if (minValue > maxValue)
            throw std::runtime_error("min value is greater than max value");

        if (defaultValue > maxValue)
            throw std::runtime_error("default value is greater than max value");

        if (defaultValue < minValue)
            throw std::runtime_error("default value is smaller than min value");
    }

    constexpr bool operator==(const SettingsEntry &other) const
    {
        return name.compare(other.name) == 0;
    }

    constexpr bool operator!=(const SettingsEntry &other) const
    {
        return !(*this == other);
    }

    constexpr bool hasSameName(std::string_view otherName) const
    {
        return name.compare(otherName) == 0;
    }
};
} // namespace settings