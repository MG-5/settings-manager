#include "settings-manager/SettingsUser.hpp"
#include "settings-manager/SettingsIO.hpp"

#include "TestSettings.hpp"
#include "fake/FakeEeprom.hpp"

#include <gtest/gtest.h>

namespace
{
using namespace settings;

class SettingsIOTest
{
public:
    SettingsIOTest() = default;

    FakeEeprom eeprom{};
    TestSettings::Container settingsContainer{};
    TestSettings::IO settingsIo{eeprom, settingsContainer};
};

class SettingsUserTest : public settings::SettingsUser, public ::testing::Test
{
protected:
    SettingsUserTest()
    {
        settingsIoTest.settingsIo.loadSettings();
    }
    void onSettingsUpdate() override;

    bool settingsUpdateFunctionCalled = false;
    SettingsIOTest settingsIoTest;

    static constexpr std::string_view EntryName = TestSettings::Entry2;
    float value = 0.0f;
};

void SettingsUserTest::onSettingsUpdate()
{
    settingsUpdateFunctionCalled = true;
    value = settingsIoTest.settingsContainer.getValue<EntryName>();
}

TEST_F(SettingsUserTest, notifySettings)
{
    EXPECT_FALSE(settingsUpdateFunctionCalled);
    EXPECT_FLOAT_EQ(value, 0.0f);

    SettingsUser::notifySettingsUpdate();

    EXPECT_TRUE(settingsUpdateFunctionCalled);
    EXPECT_FLOAT_EQ(value, 20.0f);
}
} // namespace