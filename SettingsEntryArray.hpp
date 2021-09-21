#pragma once

#include "SettingsEntry.hpp"

#include <array>
#include <cstdlib>

namespace settings
{

template <size_t SettingsCount>
class SettingsEntryArray
{
public:
    using EntryArray_t = std::array<SettingsEntry, SettingsCount>;

    constexpr explicit SettingsEntryArray(EntryArray_t entryArray) : entryArray(entryArray)
    {
        // check for duplicates
        for (auto &i : entryArray)
        {
            for (auto &j : entryArray)
            {
                if (std::addressof(i) == std::addressof(j))
                {
                    continue;
                }
                if (i == j)
                    std::abort();
                // at compile-time you will get an error which says that
                // std::abort is not a constexpr function. This is only
                // triggered if you have a duplicate
            }
        }
    }

    ~SettingsEntryArray() = default;

    //----------------------------------------------------------------------------------------------
    [[nodiscard]] constexpr size_t size() const
    {
        return SettingsCount;
    }

    //----------------------------------------------------------------------------------------------
    [[nodiscard]] constexpr const EntryArray_t &getEntries() const
    {
        return entryArray;
    }

    //----------------------------------------------------------------------------------------------
    constexpr const SettingsEntry &getEntryByName(std::string_view name) const
    {
        return entryArray[getIndex(name)];
    }

    constexpr const SettingsEntry &at(size_t index) const
    {
        if (index < SettingsCount)
            return entryArray[index];

        specialAssert(false);
        return entryArray[0];
    }

    constexpr const SettingsEntry &operator[](size_t index) const
    {
        return at(index);
    }

    constexpr size_t getIndex(std::string_view name) const
    {
        for (size_t i = 0; i < SettingsCount; i++)
        {
            if (entryArray[i].hasSameName(name))
            {
                return i;
            }
        }
        specialAssert(false);
        return 0;
    }

private:
    EntryArray_t entryArray;
};

} // namespace settings