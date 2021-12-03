#pragma once
#include <string_view>

namespace settings
{
using SettingsValue_t = float;

/**
 * A single settings entry. Static content.
 */
class SettingsEntry
{
public:
    const SettingsValue_t minValue;
    const SettingsValue_t defaultValue;
    const SettingsValue_t maxValue;
    const std::string_view name;

    //----------------------------------------------------------------------------------------------
    constexpr SettingsEntry(const SettingsValue_t min, const SettingsValue_t defaultValue,
                            const SettingsValue_t max, std::string_view name)
        : minValue{min}, defaultValue{defaultValue}, maxValue{max}, name{name}
    {
    }

    constexpr bool isValid() const
    {
        return !(minValue > maxValue || defaultValue > maxValue || defaultValue < minValue);
    }

    [[nodiscard]] constexpr bool hasSameName(const std::string_view& otherName) const
    {
        return name.compare(otherName) == 0;
    }
};
} // namespace settings