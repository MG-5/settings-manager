#include "SettingsIO.hpp"
#include "core/hash.hpp"

namespace
{
using namespace settings;

constexpr size_t Signature = 0x0110CA6E;

struct EepromContent
{
    uint64_t fieldsHash;
    uint64_t settingsHash;
    size_t magicString = Signature;
    SettingsContainer settingsContainer{};
};

class SettingsIOTest : public ::testing::Test
{
protected:
    SettingsIOTest() : settingsIo(eeprom, settingsContainer)
    {
    }

    Eeprom24LC64 eeprom;
    SettingsContainer settingsContainer;
    SettingsIO settingsIo;
};

TEST_F(SettingsIOTest, initFromEmptyEeprom)
{
    settingsIo.init();

    EepromContent currentContent;

    eeprom.read(0, reinterpret_cast<uint8_t *>(&currentContent), sizeof(EepromContent));

    EXPECT_EQ(currentContent.magicString, Signature);
    EXPECT_EQ(currentContent.settingsContainer, settingsContainer);
};

TEST_F(SettingsIOTest, initFromDefaultEeprom)
{
    settingsIo.init();
    EepromContent currentContent;
    eeprom.read(0, reinterpret_cast<uint8_t *>(&currentContent), sizeof(EepromContent));

    currentContent.settingsContainer.setValue(Entry1, 2.0, false);
    eeprom.write(0, reinterpret_cast<uint8_t *>(&currentContent), sizeof(EepromContent));

    settingsIo.init();
    eeprom.read(0, reinterpret_cast<uint8_t *>(&currentContent), sizeof(EepromContent));

    EXPECT_EQ(currentContent.settingsContainer, settingsContainer);
};

TEST_F(SettingsIOTest, initFromEepromWithWrongValues)
{
    settingsIo.init();
    EepromContent currentContent;
    eeprom.read(0, reinterpret_cast<uint8_t *>(&currentContent), sizeof(EepromContent));
    currentContent.settingsContainer.setValue(Entry1, 2.0, false);
    currentContent.settingsContainer.setValue(Entry3, 5.0, false);
    EepromContent prevContent = currentContent;

    eeprom.write(0, reinterpret_cast<uint8_t *>(&currentContent), sizeof(EepromContent));
    float wrongValue = 22.0f;
    eeprom.write(64, reinterpret_cast<uint8_t *>(&wrongValue), sizeof(float));

    settingsIo.init();
    eeprom.read(0, reinterpret_cast<uint8_t *>(&currentContent), sizeof(EepromContent));

    EXPECT_EQ(prevContent.settingsContainer, settingsContainer);
};

TEST_F(SettingsIOTest, saveSettings)
{
    settingsIo.init();

    settingsContainer.setValue(Entry3, 5.0, true);
    settingsIo.saveSettings();

    EepromContent currentContent;
    eeprom.read(0, reinterpret_cast<uint8_t *>(&currentContent), sizeof(EepromContent));
    EXPECT_EQ(currentContent.settingsContainer, settingsContainer);
};

} // namespace
