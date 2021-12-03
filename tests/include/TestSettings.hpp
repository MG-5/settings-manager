#pragma once

#include "parameter_manager/SettingsContainer.hpp"
#include "parameter_manager/ParameterManager.hpp"

namespace TestSettings
{
constexpr std::string_view Entry1 = "entry1";
constexpr std::string_view Entry2 = "entry2";
constexpr std::string_view Entry3 = "entry3";

constexpr settings::SettingsValue_t Entry1_min = 1;
constexpr settings::SettingsValue_t Entry1_default = 10;
constexpr settings::SettingsValue_t Entry1_max = 100;

constexpr settings::SettingsValue_t Entry2_min = 2;
constexpr settings::SettingsValue_t Entry2_default = 20;
constexpr settings::SettingsValue_t Entry2_max = 200;

constexpr settings::SettingsValue_t Entry3_min = 3;
constexpr settings::SettingsValue_t Entry3_default = 30;
constexpr settings::SettingsValue_t Entry3_max = 300;

constexpr std::array EntryArray = {
    settings::SettingsEntry{Entry1_min, Entry1_default, Entry1_max, Entry1}, //
    settings::SettingsEntry{Entry2_min, Entry2_default, Entry2_max, Entry2}, //
    settings::SettingsEntry{Entry3_min, Entry3_default, Entry3_max, Entry3}  //
};
using Container = settings::SettingsContainer<EntryArray.size(), EntryArray>;
using IO = settings::SettingsIO<EntryArray.size(), EntryArray>;
using ParameterManager = settings::ParameterManager<EntryArray.size(), EntryArray>;
} // namespace TestSettings