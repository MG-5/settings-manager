#pragma once

#include "AbstractSettingsContainer.hpp"

namespace settings
{
constexpr std::string_view Entry1 = "entry1";
constexpr std::string_view Entry2 = "entry2";
constexpr std::string_view Entry3 = "entry3";
constexpr std::string_view Entry4 = "entry4";

constexpr std::array SettingsEntries = {
    SettingsEntry{1, 4, 6, Entry1},        //
    SettingsEntry{0.5, 3.0, 12.0, Entry2}, //
    SettingsEntry{1.0, 3.7, 5.0, Entry3},  //
    SettingsEntry{0.0, 1.0, 5.0, Entry4},  //
};

inline constexpr SettingsEntryArray<SettingsEntries.size()> EntryArray{SettingsEntries};

class SettingsContainer : public AbstractSettingsContainer<EntryArray.size(), EntryArray>
{
};

} // namespace settings