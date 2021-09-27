### parameter_manager

Collects all settings with their string names, min/default/max values and the current value.
All settings are automatically made available to UAVCAN's parameter server.

### How to

You must create a .hpp file, which contains a new class with all of your settings. This class inherited from `AbstractSettingsContainer` class.
Here is a template file with example settings:

```
#pragma once

#include "parameter_manager/AbstractSettingsContainer.hpp"
#include <string_view>

namespace settings
{
constexpr std::string_view CarMass = "car mass";
constexpr std::string_view CarWheelRadius = "car wheel radius";
constexpr std::string_view MotorMagnetCount = "motor magnet count";

constexpr std::array SettingsEntries = {
    SettingsEntry{0.001, 2.5, 10.0, CarMass},           //
    SettingsEntry{0.001, 0.05, 10.0, CarWheelRadius},   //
    SettingsEntry{2.0, 24.0 , 100.0, MotorMagnetCount}, //
};

inline constexpr SettingsEntryArray<SettingsEntries.size()> EntryArray{SettingsEntries};

class SettingsContainer : public AbstractSettingsContainer<EntryArray.size(), EntryArray>
{
};

} // namespace settings

extern settings::SettingsContainer settingsContainer;
```

To add a new setting entry to your system, do following things:

1. Add a `constexpr string_view` constant with your settings name to the settings namespace. This will be used to access settings from anywhere. **Attention: this name must be unique!**
2. Create a `SettingsEntry` with the name from 1. and its minimum, default and maximum value and add this to the `SettingsEntries` array;
  
<br><!-- <stupid_meme> --></br>
<br></br>
<sup>Stand Master: Child Class </sup>  
<sup>Stand Name: </sup>  
### 「SettingsUser」
<!-- </stupid_meme> -->

All classes using settings should inherit from here and implement the `onSettingsUpdate()` function. This function is guaranteed to be called at least once when the EEPROM is finished initializing (your class must be contstructed before that of course). When called before EEPROM is ready you will only get default values.
Will be called when someone updates the value by calling the static `notifySettingsUpdate()` function.

Here is an example .hpp file related to example in **How To**:

```
#pragma once

#include "settings/SettingsContainer.hpp"
#include "settings/SettingsUser.hpp"
#include <units/si/mass.hpp>
#include <units/si/length.hpp>


class Motor : public settings::SettingsUser
{
public:
    Motor(settings::SettingsContainer &settings): settings(settings);

    void onSettingsUpdate()
    {
        carMass = settings.getValue<units::si::Mass>(settings::CarMass);
        carWheelRadius = settings.getValue<units::si::Length>(settings::CarWheelRadius);
        motorMagnetCount = settings.getValue(settings::MotorMagnetCount);
    }

private:
    settings::SettingsContainer &settings;
    units::si::Mass carMass{0.0_kg};
    units::si:Length carWheelRadius
    float motorMagnetCount
}
```
