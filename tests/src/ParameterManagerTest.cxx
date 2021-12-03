#include "TestSettings.hpp"
#include "fake/i2c-drivers/Fake24LC64.hpp"
#include "mock/SettingsIOMock.hpp"
#include "stub/BusAccessor.hpp"
#include <array>
#include <gtest/gtest.h>
#include <numeric>

using TestSettings::Container;
using TestSettings::IO;
using TestSettings::ParameterManager;

using uavcan::IParamManager;

class ParameterManagerTest : public ::testing::Test
{
protected:
    ParameterManagerTest()
        : eeprom(accessor), settingsIo(eeprom, settingsContainer),
          manager(settingsContainer, settingsIo)
    {
        static_assert(ValidName != InvalidName);
    }

    BusAccessorStub accessor;
    FakeEeprom24LC64 eeprom;
    Container settingsContainer;
    SettingsIOMock<TestSettings::EntryArray.size(), TestSettings::EntryArray> settingsIo;

    ParameterManager manager;

    static constexpr std::string_view InvalidName = "{{{{invalidName}}}";
    static constexpr std::string_view ValidName = TestSettings::Entry1;
    static constexpr auto ValidNameIndex =
        TestSettings::Container::getIndex<ValidName>();
};

TEST_F(ParameterManagerTest, saveAllParams)
{
    EXPECT_CALL(settingsIo, saveSettings()).Times(1);
    ASSERT_EQ(manager.saveAllParams(), 0);
}

TEST_F(ParameterManagerTest, eraseAllParams)
{
    static_assert(TestSettings::Entry1_default != TestSettings::Entry1_min);
    ASSERT_EQ(settingsContainer.getValue(TestSettings::Entry1), TestSettings::Entry1_default);
    ASSERT_TRUE(settingsContainer.setValue(TestSettings::Entry1, TestSettings::Entry1_min));

    EXPECT_CALL(settingsIo, saveSettings()).Times(1);
    ASSERT_EQ(manager.eraseAllParams(),0);

    ASSERT_EQ(settingsContainer.getValue(TestSettings::Entry1), TestSettings::Entry1_default);
}

TEST_F(ParameterManagerTest, getParamNameByIndex)
{
    uavcan::IParamManager::Name name(InvalidName.data());
    uavcan::IParamManager::Index invalid(TestSettings::EntryArray.size() + 1);
    uavcan::IParamManager::Index invalid2(TestSettings::EntryArray.size());
    uavcan::IParamManager::Index valid(ValidNameIndex);

    manager.getParamNameByIndex(invalid, name);
    EXPECT_EQ(name, InvalidName.data());

    manager.getParamNameByIndex(invalid2, name);
    EXPECT_EQ(name, InvalidName.data());

    manager.getParamNameByIndex(valid, name);
    EXPECT_EQ(name, ValidName.data());
}

TEST_F(ParameterManagerTest, assignParamValue)
{
    const auto newValue = settingsContainer.getMinValue(ValidNameIndex);
    IParamManager::Value newValueUAVCAN;
    newValueUAVCAN.to<IParamManager::Value::Tag::real_value>() = newValue;

    const auto currentValue = settingsContainer.getValue(ValidNameIndex);
    ASSERT_NE(currentValue, newValue);

    // no setting change with illegal name
    Container defaultContainer;
    ASSERT_EQ(defaultContainer, settingsContainer);

    uavcan::IParamManager::Name invalidNameUAVCAN(InvalidName.data());
    manager.assignParamValue(invalidNameUAVCAN, newValueUAVCAN);

    ASSERT_EQ(defaultContainer, settingsContainer);
    ASSERT_EQ(settingsContainer.getValue(ValidNameIndex), currentValue);

    // settings change with valid name
    uavcan::IParamManager::Name validNameUAVCAN(ValidName.data());
    manager.assignParamValue(validNameUAVCAN, newValueUAVCAN);

    ASSERT_NE(defaultContainer, settingsContainer);
    ASSERT_EQ(settingsContainer.getValue(ValidNameIndex), newValue);
}

TEST_F(ParameterManagerTest, readParamValue)
{
    const auto currentValue = settingsContainer.getValue(ValidNameIndex);
    const auto otherValue = currentValue + 1;
    IParamManager::Value valueUAVCAN;
    valueUAVCAN.to<IParamManager::Value::Tag::real_value>() = otherValue;

    ASSERT_EQ(otherValue, *valueUAVCAN.as<uavcan::protocol::param::Value::Tag::real_value>());
    ASSERT_NE(currentValue, otherValue);

    // no readout with invalid name
    uavcan::IParamManager::Name invalidNameUAVCAN(InvalidName.data());
    manager.readParamValue(invalidNameUAVCAN, valueUAVCAN);
    ASSERT_EQ(otherValue, *valueUAVCAN.as<uavcan::protocol::param::Value::Tag::real_value>());

    // readout with valid name
    uavcan::IParamManager::Name validNameUAVCAN(ValidName.data());
    manager.readParamValue(validNameUAVCAN, valueUAVCAN);
    ASSERT_EQ(currentValue, *valueUAVCAN.as<uavcan::protocol::param::Value::Tag::real_value>());
}

TEST_F(ParameterManagerTest, readParamDefaultMaxMin)
{
    const auto currentMin = settingsContainer.getMinValue(ValidNameIndex);
    const auto currentDefault = settingsContainer.getDefaultValue(ValidNameIndex);
    const auto currentMax = settingsContainer.getMaxValue(ValidNameIndex);

    const auto otherMin = currentMin + 1;
    const auto otherDefault = currentDefault + 1;
    const auto otherMax = currentMax + 1;

    ASSERT_NE(currentMin, otherMin);
    ASSERT_NE(currentDefault, otherDefault);
    ASSERT_NE(currentMax, otherMax);

    ASSERT_NE(currentMin, currentDefault);
    ASSERT_NE(currentMin, currentMax);
    ASSERT_NE(currentDefault, currentMax);


    IParamManager::Value defaultUAVCAN;
    IParamManager::NumericValue minUAVCAN, maxUAVCAN;
    minUAVCAN.to<IParamManager::NumericValue::Tag::real_value>() = otherMin;
    defaultUAVCAN.to<IParamManager::Value::Tag::real_value>() = otherDefault;
    maxUAVCAN.to<IParamManager::NumericValue::Tag::real_value>() = otherMax;

    ASSERT_EQ(otherMin, *minUAVCAN.as<uavcan::protocol::param::NumericValue::Tag::real_value>());
    ASSERT_EQ(otherDefault, *defaultUAVCAN.as<uavcan::protocol::param::Value::Tag::real_value>());
    ASSERT_EQ(otherMax, *maxUAVCAN.as<uavcan::protocol::param::NumericValue::Tag::real_value>());


    // no readout with invalid name
    uavcan::IParamManager::Name invalidNameUAVCAN(InvalidName.data());
    manager.readParamDefaultMaxMin(invalidNameUAVCAN, defaultUAVCAN, maxUAVCAN, minUAVCAN);
    ASSERT_EQ(otherMin, *minUAVCAN.as<uavcan::protocol::param::NumericValue::Tag::real_value>());
    ASSERT_EQ(otherDefault, *defaultUAVCAN.as<uavcan::protocol::param::Value::Tag::real_value>());
    ASSERT_EQ(otherMax, *maxUAVCAN.as<uavcan::protocol::param::NumericValue::Tag::real_value>());


    // readout with valid name
    uavcan::IParamManager::Name validNameUAVCAN(ValidName.data());
    manager.readParamDefaultMaxMin(validNameUAVCAN, defaultUAVCAN, maxUAVCAN, minUAVCAN);
    ASSERT_EQ(currentMin, *minUAVCAN.as<uavcan::protocol::param::NumericValue::Tag::real_value>());
    ASSERT_EQ(currentDefault, *defaultUAVCAN.as<uavcan::protocol::param::Value::Tag::real_value>());
    ASSERT_EQ(currentMax, *maxUAVCAN.as<uavcan::protocol::param::NumericValue::Tag::real_value>());
}