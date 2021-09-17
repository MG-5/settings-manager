#include "ParameterManager.hpp"

using namespace uavcan::protocol::param;

void ParameterManager::getParamNameByIndex(Index index, Name &outName) const
{
    if (index < settingContainer.getAllSettings().size())
    {
        outName = settingContainer.getAllSettings().at(index).name;
    }
}

void ParameterManager::assignParamValue(const Name &name, const Value &value)
{
    if (settingContainer.doesSettingExist(name.c_str()))
    {
        if (value.is(Value::Tag::real_value))
        {
            settingContainer.set(name.c_str(),
                                 *value.as<uavcan::protocol::param::Value::Tag::real_value>());
        }
    }
}

void ParameterManager::readParamValue(const Name &name, Value &outValue) const
{
    if (settingContainer.doesSettingExist(name.c_str()))
    {
        outValue.to<Value::Tag::real_value>() = settingContainer.get(name.c_str()).get();
    }
}

void ParameterManager::readParamDefaultMaxMin(const Name &name, Value &outDef, NumericValue &outMax,
                                              NumericValue &outMin) const
{
    if (settingContainer.doesSettingExist(name.c_str()))
    {
        const auto entry = settingContainer.get(name.c_str());
        outMin.to<NumericValue::Tag::real_value>() = entry.minValue;
        outDef.to<Value::Tag::real_value>() = entry.defaultValue;
        outMax.to<NumericValue::Tag::real_value>() = entry.maxValue;
    }
}

int ParameterManager::saveAllParams()
{
    return 0;
}

int ParameterManager::eraseAllParams()
{
    settingContainer.resetAllToDefault();
    return 0;
}
ParameterManager::ParameterManager(settings::SettingsContainer &settingsContainer)
    : settingContainer(settingsContainer)
{
}