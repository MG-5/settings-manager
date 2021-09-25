#include "SettingsUser.hpp"
#include "SettingsIO.hpp"
#include <gtest/gtest.h>

namespace
{
using namespace settings;

class SettingsIOTest
{
public:
    SettingsIOTest() : settingsIo(eeprom, settingsContainer)
    {
    }

    Eeprom24LC64 eeprom;
    SettingsContainer settingsContainer;
    SettingsIO settingsIo;
};

class SettingsUserTest : public settings::SettingsUser, public ::testing::Test
{
protected:
    SettingsUserTest()
    {
        settingsIoTest.settingsIo.loadSettings(false);
    }
    void onSettingsUpdate() override;

    bool settingsUpdateFunctionCalled = false;
    SettingsIOTest settingsIoTest;

    static constexpr std::string_view EntryName = Entry2;
    float value = 0.0f;
};

void SettingsUserTest::onSettingsUpdate()
{
    settingsUpdateFunctionCalled = true;
    value = settingsIoTest.settingsContainer.getValue(EntryName);
}

TEST_F(SettingsUserTest, notifySettings)
{
    EXPECT_FALSE(settingsUpdateFunctionCalled);
    EXPECT_FLOAT_EQ(value, 0.0f);

    SettingsUser::notifySettingsUpdate();

    EXPECT_TRUE(settingsUpdateFunctionCalled);
    EXPECT_FLOAT_EQ(value, 3.0f);
}
} // namespace