#pragma once

#include "parameter_manager/SettingsContainer.hpp"
#include "parameter_manager/SettingsIO.hpp"
#include <uavcan/protocol/param_server.hpp>

/**
 * Hooks up uavcan to all settings. With this running uavcan can change every parameter registered
 * within SettingsEntryArray / SettingsContainer.  Caution: Writing to eerpom only happens
 * when the SAVE command is executed.
 * @tparam SettingsCount
 * @tparam entryArray
 */
namespace settings
{
template <size_t SettingsCount, const std::array<SettingsEntry, SettingsCount> &entryArray>
class ParameterManager : public uavcan::IParamManager
{
public:
    ParameterManager(SettingsContainer<SettingsCount, entryArray> &container,
                     SettingsIO<SettingsCount, entryArray> &io)
        : settingContainer(container), settingsIO(io)
    {
    }

    void getParamNameByIndex(Index index, Name &outName) const override
    {
        if (index < settingContainer.size())
        {
            outName = settingContainer.getAllSettings()[index].name.data();
        }
    }

    void assignParamValue(const Name &name, const Value &value) override
    {
        if (!settingContainer.doesSettingExist(name.c_str()))
            return;

        if (value.is(Value::Tag::boolean_value))
        {
            settingContainer.setValue(
                name.c_str(), *value.as<uavcan::protocol::param::Value::Tag::boolean_value>());
        }
        else if (value.is(Value::Tag::integer_value))
        {
            settingContainer.setValue(
                name.c_str(), *value.as<uavcan::protocol::param::Value::Tag::integer_value>());
        }
        else if (value.is(Value::Tag::real_value))
        {
            settingContainer.setValue(name.c_str(),
                                      *value.as<uavcan::protocol::param::Value::Tag::real_value>());
        }
    }

    void readParamValue(const Name &name, Value &outValue) const override
    {
        if (!settingContainer.doesSettingExist(name.c_str()))
            return;

        const auto index = settingContainer.getIndex(name.c_str());
        const auto settingValue = settingContainer.getValue(index);

        switch (settingContainer.getVariableType(index))
        {
        case VariableType::booleanType:
            outValue.to<Value::Tag::boolean_value>() = settingValue;
            break;

        case VariableType::integerType:
            outValue.to<Value::Tag::integer_value>() = settingValue;
            break;

        case VariableType::realType:
        default:
            outValue.to<Value::Tag::real_value>() = settingValue;
            break;
        }
    }

    void readParamDefaultMaxMin(const Name &name, Value &outDef, NumericValue &outMax,
                                NumericValue &outMin) const override
    {
        if (!settingContainer.doesSettingExist(name.c_str()))
            return;

        const auto index = settingContainer.getIndex(name.c_str());
        const auto entryVariableType = settingContainer.getVariableType(index);

        switch (entryVariableType)
        {
        case VariableType::booleanType:
            outDef.to<Value::Tag::boolean_value>() = (settingContainer.getDefaultValue(index) != 0);
            break;

        case VariableType::integerType:
            outMin.to<NumericValue::Tag::integer_value>() = settingContainer.getMinValue(index);
            outDef.to<Value::Tag::integer_value>() = settingContainer.getDefaultValue(index);
            outMax.to<NumericValue::Tag::integer_value>() = settingContainer.getMaxValue(index);
            break;

        default:
        case VariableType::realType:
            outMin.to<NumericValue::Tag::real_value>() = settingContainer.getMinValue(index);
            outDef.to<Value::Tag::real_value>() = settingContainer.getDefaultValue(index);
            outMax.to<NumericValue::Tag::real_value>() = settingContainer.getMaxValue(index);
            break;
        }
    }

    int saveAllParams() override
    {
        settingsIO.saveSettings();
        return 0;
    }

    int eraseAllParams() override
    {
        settingContainer.resetAllToDefault();
        settingsIO.saveSettings();
        return 0;
    }

private:
    SettingsContainer<SettingsCount, entryArray> &settingContainer;
    SettingsIO<SettingsCount, entryArray> &settingsIO;
};
} // namespace settings