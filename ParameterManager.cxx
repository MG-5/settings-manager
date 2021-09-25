#include "ParameterManager.hpp"

using namespace uavcan::protocol::param;

//----------------------------------------------------------------------------------------------
void ParameterManager::getParamNameByIndex(Index index, Name &outName) const
{
    if (index < settingContainer.size())
    {
        outName = settingContainer.getAllSettings()[index].name.data();
    }
}

//----------------------------------------------------------------------------------------------
void ParameterManager::assignParamValue(const Name &name, const Value &value)
{
    if (settingContainer.doesSettingExist(name.c_str()))
    {
        if (value.is(Value::Tag::real_value))
        {
            settingContainer.setValue(name.c_str(),
                                      *value.as<uavcan::protocol::param::Value::Tag::real_value>());
        }
    }
}

//----------------------------------------------------------------------------------------------
void ParameterManager::readParamValue(const Name &name, Value &outValue) const
{
    if (settingContainer.doesSettingExist(name.c_str()))
    {
        outValue.to<Value::Tag::real_value>() = settingContainer.getValue(name.c_str());
    }
}

//----------------------------------------------------------------------------------------------
void ParameterManager::readParamDefaultMaxMin(const Name &name, Value &outDef, NumericValue &outMax,
                                              NumericValue &outMin) const
{
    if (settingContainer.doesSettingExist(name.c_str()))
    {
        const auto entry = settingContainer.getAllSettings().getEntryByName(name.c_str());
        outMin.to<NumericValue::Tag::real_value>() = entry.minValue;
        outDef.to<Value::Tag::real_value>() = entry.defaultValue;
        outMax.to<NumericValue::Tag::real_value>() = entry.maxValue;
    }
}

//----------------------------------------------------------------------------------------------
int ParameterManager::saveAllParams()
{
    settingsIO.saveSettings();
    return 0;
}

//----------------------------------------------------------------------------------------------
int ParameterManager::eraseAllParams()
{
    settingContainer.resetAllToDefault(true);
    settingsIO.saveSettings();
    return 0;
}

//----------------------------------------------------------------------------------------------
ParameterManager::ParameterManager(settings::SettingsContainer &settingsContainer,
                                   settings::SettingsIO &settingsIO)
    : settingContainer(settingsContainer), settingsIO(settingsIO)
{
}