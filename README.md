### Settings

Collects all settings, and their string names. To add a new one add the name to the list in the header. 
Increase the array size at the end of the file and add a new entry to the list in the source file.
All settings are automatically made available to UAVCAN's parameter server.


### SettingsUser

All classes using settings should inherit from here and implement. This function is guaranteed to be called at least once when the eeprom is 
finished initializing (your class must be contstructed before that of course). When called before eeprom is ready you will only get default values.
Call anytime to update. Will be called when someone updates the value.

```
 virtual void onSettingsUpdate() = 0;
```

### Credits
Credits go to Michael Albrecht. This version was adopted to the project, but is mostly based on his version in [Drive-Controller](https://ottocar.cs.ovgu.de/gitlab/ottocar/Firmware/generation/drive_controller).

