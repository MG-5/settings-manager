#include "SettingsContainerTest.hpp"
#include <gtest/gtest.h>

namespace
{
using namespace settings;

SettingsContainer settingsContainer;

TEST(SettingsContainer, Size)
{
    EXPECT_EQ(settingsContainer.size(), SettingsEntries.size());
};

TEST(SettingsContainer, getDefaultValues)
{
    EXPECT_FLOAT_EQ(settingsContainer.getValue("entry1"), 4.0);
    EXPECT_FLOAT_EQ(settingsContainer.getValue("entry2"), 3.0);
    EXPECT_FLOAT_EQ(settingsContainer.getValue("entry3"), 3.7);
    EXPECT_FLOAT_EQ(settingsContainer.getValue("entry4"), 1.0);
    EXPECT_DEATH(settingsContainer.getValue(""), "");
    EXPECT_DEATH(settingsContainer.getValue("nonExistentEntry"), "");
};

TEST(SettingsContainer, setValues)
{
    EXPECT_TRUE(settingsContainer.setValue("entry1", 2.1f, false));
    EXPECT_TRUE(settingsContainer.setValue("entry2", 2.2f, false));
    EXPECT_TRUE(settingsContainer.setValue("entry3", 2.3f, false));
    EXPECT_TRUE(settingsContainer.setValue("entry4", 2.4f, false));

    EXPECT_FALSE(settingsContainer.setValue("entry1", 0.0f, false));
    EXPECT_FALSE(settingsContainer.setValue("entry2", 22.0f, false));
    EXPECT_FALSE(settingsContainer.setValue("entry3", 0.8f, false));
    EXPECT_FALSE(settingsContainer.setValue("entry4", 6.0f, false));

    EXPECT_DEATH(settingsContainer.setValue("", 1), "");
    EXPECT_DEATH(settingsContainer.setValue("nonExistentEntry", 1), "");
};

TEST(SettingsContainer, getDefaultValuesAfterReset)
{
    settingsContainer.resetAllToDefault();

    EXPECT_FLOAT_EQ(settingsContainer.getValue("entry1"), 4.0);
    EXPECT_FLOAT_EQ(settingsContainer.getValue("entry2"), 3.0);
    EXPECT_FLOAT_EQ(settingsContainer.getValue("entry3"), 3.7);
    EXPECT_FLOAT_EQ(settingsContainer.getValue("entry4"), 1.0);
};

TEST(SettingsContainer, doesSettingExist)
{
    EXPECT_TRUE(settingsContainer.doesSettingExist("entry1"));
    EXPECT_TRUE(settingsContainer.doesSettingExist("entry2"));
    EXPECT_TRUE(settingsContainer.doesSettingExist("entry3"));
    EXPECT_TRUE(settingsContainer.doesSettingExist("entry4"));
    EXPECT_FALSE(settingsContainer.doesSettingExist(""));
    EXPECT_FALSE(settingsContainer.doesSettingExist("nonExistentEntry"));
};

} // namespace