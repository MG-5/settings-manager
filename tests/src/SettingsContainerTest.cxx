#include "SettingsContainer.hpp"
#include <gtest/gtest.h>

namespace
{
using namespace settings;

class SettingsContainerTest : public ::testing::Test
{
protected:
    SettingsContainerTest()
    {
    }
    SettingsContainer settingsContainer;
};

TEST_F(SettingsContainerTest, Size)
{
    EXPECT_EQ(settingsContainer.size(), SettingsEntries.size());
};

TEST_F(SettingsContainerTest, getDefaultValues)
{
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry1), 4.0);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry2), 3.0);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry3), 3.7);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry4), 1.0);

    EXPECT_DEATH(settingsContainer.getValue(""), "");
    EXPECT_DEATH(settingsContainer.getValue("nonExistentEntry"), "");
};

TEST_F(SettingsContainerTest, setValues)
{
    EXPECT_TRUE(settingsContainer.setValue(Entry1, 2.1f, false));
    EXPECT_TRUE(settingsContainer.setValue(Entry2, 2.2f, false));
    EXPECT_TRUE(settingsContainer.setValue(Entry3, 2.3f, false));
    EXPECT_TRUE(settingsContainer.setValue(Entry4, 2.4f, false));

    EXPECT_FALSE(settingsContainer.setValue(Entry1, 0.0f, false));
    EXPECT_FALSE(settingsContainer.setValue(Entry2, 22.0f, false));
    EXPECT_FALSE(settingsContainer.setValue(Entry3, 0.8f, false));
    EXPECT_FALSE(settingsContainer.setValue(Entry4, 6.0f, false));

    EXPECT_DEATH(settingsContainer.setValue("", 1), "");
    EXPECT_DEATH(settingsContainer.setValue("nonExistentEntry", 1), "");

    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry1), 2.1);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry2), 2.2);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry3), 2.3);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry4), 2.4);
};

TEST_F(SettingsContainerTest, getDefaultValuesAfterReset)
{
    EXPECT_TRUE(settingsContainer.setValue(Entry1, 2.1f, false));
    EXPECT_TRUE(settingsContainer.setValue(Entry2, 2.2f, false));
    EXPECT_TRUE(settingsContainer.setValue(Entry3, 2.3f, false));
    EXPECT_TRUE(settingsContainer.setValue(Entry4, 2.4f, false));

    settingsContainer.resetAllToDefault();

    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry1), 4.0);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry2), 3.0);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry3), 3.7);
    EXPECT_FLOAT_EQ(settingsContainer.getValue(Entry4), 1.0);
};

TEST_F(SettingsContainerTest, doesSettingExist)
{
    EXPECT_TRUE(settingsContainer.doesSettingExist(Entry1));
    EXPECT_TRUE(settingsContainer.doesSettingExist(Entry2));
    EXPECT_TRUE(settingsContainer.doesSettingExist(Entry3));
    EXPECT_TRUE(settingsContainer.doesSettingExist(Entry4));
    EXPECT_FALSE(settingsContainer.doesSettingExist(""));
    EXPECT_FALSE(settingsContainer.doesSettingExist("nonExistentEntry"));
};

} // namespace