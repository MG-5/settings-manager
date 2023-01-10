#include "TestSettings.hpp"
#include "fake/FakeEeprom.hpp"
#include <array>
#include <core/hash.hpp>
#include <gtest/gtest.h>
#include <numeric>

using namespace settings;
using TestSettings::Container;
using TestSettings::IO;

class SettingsIOTest : public ::testing::Test
{
protected:
    SettingsIOTest()
    {
        // placement information of IO::EepromContent members
        loadEepromContentOffsets();
    }

    FakeEeprom eeprom{};
    Container settingsContainer{};
    IO settingsIo{eeprom, settingsContainer};

    IO::EepromContent temporaryContent;

    using Offset_t = uint64_t;
    using OffsetEntry_t = std::pair<uint64_t, uint64_t>;
    void loadEepromContentOffsets();

    OffsetEntry_t ValuesHashOffset;
    OffsetEntry_t MagicStringOffset;
    // keep size in line with number of OffsetEntry_t above, too big array will fail asserts in
    // loadEepromContentOffsets
    std::array<OffsetEntry_t, 2> allOffsets;
};

void SettingsIOTest::loadEepromContentOffsets()
{
    // get offsets so we can precisely corrupt data
    static_assert(sizeof(OffsetEntry_t::first) == sizeof(OffsetEntry_t::second));
    static_assert(sizeof(OffsetEntry_t::first) == sizeof(Offset_t));
    static_assert(
        sizeof(IO::EepromContent *) <= sizeof(Offset_t),
        "your architecture has a higher pointer length, increase uint64_t everywhere here");

    ValuesHashOffset = std::pair(reinterpret_cast<Offset_t>(&temporaryContent.settingsValuesHash) -
                                     reinterpret_cast<Offset_t>(&temporaryContent),
                                 sizeof(temporaryContent.settingsValuesHash));

    MagicStringOffset = std::pair(reinterpret_cast<Offset_t>(&temporaryContent.magicString) -
                                      reinterpret_cast<Offset_t>(&temporaryContent),
                                  sizeof(temporaryContent.magicString));

    allOffsets = {ValuesHashOffset, MagicStringOffset};
}

TEST_F(SettingsIOTest, initFromEmptyEeprom)
{
    // eeprom is filled with 0xFF fresh out of the package,
    // ensure a default settings initialisation and write back of
    // default values
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));
    ASSERT_EQ(temporaryContent.settingsContainer, settingsContainer);
}

TEST_F(SettingsIOTest, initFromDefaultEeprom)
{
    // init eeprom content, which fills it with default values and saves
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    // change a setting, so we can spot if default values are read again later
    static_assert(TestSettings::Entry1_min != TestSettings::Entry1_default);
    settingsContainer.setValue(TestSettings::Entry1, TestSettings::Entry1_min);
    EXPECT_NE(temporaryContent.settingsContainer, settingsContainer);

    // restore default eeprom and check if we are back to default
    eeprom.write(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                 sizeof(IO::EepromContent));
    ASSERT_TRUE(settingsIo.loadSettings());
    EXPECT_EQ(temporaryContent.settingsContainer, settingsContainer);
}

TEST_F(SettingsIOTest, initFromCorruptedEeprom)
{
    // default values
    IO::EepromContent cleanDefault;
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&cleanDefault),
                sizeof(IO::EepromContent));

    // corrupt every single byte of every single member and see
    // it content resets
    for (const auto &target : allOffsets)
    {
        for (uint64_t i = 0; i < target.second; ++i)
        {
            temporaryContent = cleanDefault;
            auto location = reinterpret_cast<uint8_t *>(&temporaryContent);
            location += target.first + i;
            (*location)++;

            // reset eeprom to clean and check if it reads ok
            eeprom.write(0, reinterpret_cast<uint8_t *>(&cleanDefault), sizeof(IO::EepromContent));
            ASSERT_TRUE(settingsIo.loadSettings());

            // now the corrupted stuff
            eeprom.write(0, reinterpret_cast<uint8_t *>(&temporaryContent),
                         sizeof(IO::EepromContent));
            ASSERT_FALSE(settingsIo.loadSettings());
        }
    }
}

TEST_F(SettingsIOTest, saveSettings)
{
    // init eeprom content, which fills it with default values and saves
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    // change a setting
    static_assert(TestSettings::Entry1_min != TestSettings::Entry1_default);
    ASSERT_EQ(settingsContainer.getValue(TestSettings::Entry1), TestSettings::Entry1_default);
    settingsContainer.setValue(TestSettings::Entry1, TestSettings::Entry1_min);
    ASSERT_EQ(settingsContainer.getValue(TestSettings::Entry1), TestSettings::Entry1_min);
    EXPECT_NE(temporaryContent.settingsContainer, settingsContainer); // applying setting worked

    // assure altered content is saved
    IO::EepromContent alteredContent;
    settingsIo.saveSettings();
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&alteredContent),
                sizeof(IO::EepromContent));
    ASSERT_NE(temporaryContent, alteredContent);
}

TEST_F(SettingsIOTest, EepromContentEquality)
{
    ASSERT_EQ(temporaryContent, temporaryContent);
    auto other = temporaryContent;
    other.settingsContainer.setValue(TestSettings::Entry1, TestSettings::Entry1_min);
    ASSERT_NE(temporaryContent, other);
}

TEST_F(SettingsIOTest, BoundsCheckFailOnLoad)
{
    // init eeprom content, which fills it with default values and saves
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    // bounds check fails when eeprom content is loaded after
    // min / max of one or more SettingsEntries was changed
    // so that the freshly loaded value doesn't fit anymore

    // instead of creating a changed duplicate of TestSettings
    // this test will directly modify a value in temporaryContent with aid of
    // the precalculated offsets from loadEepromContentOffsets
    // also the values hash will be recalculated so we read without causing a
    // reset to defaults

    // change a setting outside of bounds
    auto rawData = reinterpret_cast<SettingsValue_t *>(&temporaryContent.settingsContainer);
    ASSERT_LT(temporaryContent.settingsContainer.getValue(TestSettings::Entry1),
              TestSettings::Entry1_max);

    constexpr auto SkipHash = sizeof(uint64_t) / sizeof(SettingsValue_t);
    rawData[0 + SkipHash] = TestSettings::Entry1_max + static_cast<SettingsValue_t>(1);
    ASSERT_GT(temporaryContent.settingsContainer.getValue(TestSettings::Entry1),
              TestSettings::Entry1_max);
    temporaryContent.settingsValuesHash = IO::hashValues(temporaryContent.settingsContainer);

    // write back to eeprom
    eeprom.write(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                 sizeof(IO::EepromContent));

    // load without reset
    ASSERT_TRUE(settingsIo.loadSettings());

    // check if reset to default worked
    ASSERT_EQ(settingsContainer.getValue(TestSettings::Entry1), TestSettings::Entry1_default);

    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));
    ASSERT_EQ(settingsContainer, temporaryContent.settingsContainer);
}

TEST_F(SettingsIOTest, HashesHashNotEqual)
{
    // init eeprom content, which fills it with default values and saves
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    temporaryContent.settingsContainer.setValue<TestSettings::Entry1>(TestSettings::Entry1_max - 5);
    temporaryContent.settingsContainer.setValue<TestSettings::Entry2>(TestSettings::Entry2_max - 5);
    temporaryContent.settingsContainer.setValue<TestSettings::Entry3>(TestSettings::Entry3_max - 5);
    temporaryContent.settingsValuesHash = IO::hashValues(temporaryContent.settingsContainer);

    IO::EepromContent correctEeprom = temporaryContent;

    // make hashes hash corrupt
    temporaryContent.settingsHashesHash = 0;
    eeprom.write(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                 sizeof(IO::EepromContent));

    ASSERT_TRUE(settingsIo.loadSettings());

    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));
    ASSERT_EQ(correctEeprom, temporaryContent);
    ASSERT_EQ(settingsContainer, temporaryContent.settingsContainer);
}

TEST_F(SettingsIOTest, HashesHashFlipEntries)
{
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    temporaryContent.settingsContainer.setValue<TestSettings::Entry1>(TestSettings::Entry1_max - 5);
    temporaryContent.settingsContainer.setValue<TestSettings::Entry2>(TestSettings::Entry2_min + 5);
    temporaryContent.settingsValuesHash = IO::hashValues(temporaryContent.settingsContainer);
    temporaryContent.settingsHashesHash = IO::hashHashes(temporaryContent.settingsContainer);

    IO::EepromContent correctEeprom = temporaryContent;

    // flip two entries and recalculate hash
    uint64_t entry1Hash = temporaryContent.settingsContainer.getContainerArray()[0].hash;
    uint64_t entry2Hash = temporaryContent.settingsContainer.getContainerArray()[1].hash;
    uint64_t entry1Value = temporaryContent.settingsContainer.getContainerArray()[0].value;
    uint64_t entry2Value = temporaryContent.settingsContainer.getContainerArray()[1].value;

    temporaryContent.settingsContainer.getContainerArray()[0].hash = entry2Hash;
    temporaryContent.settingsContainer.getContainerArray()[1].hash = entry1Hash;
    temporaryContent.settingsContainer.getContainerArray()[0].value = entry2Value;
    temporaryContent.settingsContainer.getContainerArray()[1].value = entry1Value;

    temporaryContent.settingsValuesHash = IO::hashValues(temporaryContent.settingsContainer);
    temporaryContent.settingsHashesHash = IO::hashHashes(temporaryContent.settingsContainer);

    eeprom.write(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                 sizeof(IO::EepromContent));

    ASSERT_TRUE(settingsIo.loadSettings());

    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));
    ASSERT_EQ(correctEeprom, temporaryContent);
    ASSERT_EQ(settingsContainer, temporaryContent.settingsContainer);
}

TEST_F(SettingsIOTest, NumberOfSettingsGreater)
{
    // init eeprom content, which fills it with default values and saves
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    temporaryContent.settingsContainer.setValue<TestSettings::Entry1>(TestSettings::Entry1_max - 5);
    temporaryContent.settingsContainer.setValue<TestSettings::Entry2>(TestSettings::Entry2_max - 5);
    temporaryContent.settingsContainer.setValue<TestSettings::Entry3>(TestSettings::Entry3_max - 5);
    temporaryContent.settingsValuesHash = IO::hashValues(temporaryContent.settingsContainer);

    IO::EepromContent correctEeprom = temporaryContent;

    temporaryContent.numberOfSettings = 3;

    eeprom.write(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                 sizeof(IO::EepromContent));

    ASSERT_TRUE(settingsIo.loadSettings());

    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    ASSERT_EQ(correctEeprom, temporaryContent);
    ASSERT_EQ(settingsContainer, temporaryContent.settingsContainer);
}

TEST_F(SettingsIOTest, NumberOfSettingsSmaller)
{
    // init eeprom content, which fills it with default values and saves
    ASSERT_FALSE(settingsIo.loadSettings());
    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    temporaryContent.settingsContainer.setValue<TestSettings::Entry1>(TestSettings::Entry1_max - 5);
    temporaryContent.settingsContainer.setValue<TestSettings::Entry2>(TestSettings::Entry2_max - 5);
    temporaryContent.settingsContainer.setValue<TestSettings::Entry3>(TestSettings::Entry3_max - 5);
    temporaryContent.settingsValuesHash = IO::hashValues(temporaryContent.settingsContainer);

    IO::EepromContent correctEeprom = temporaryContent;

    temporaryContent.numberOfSettings = 8;
    uint64_t entry1Hash = temporaryContent.settingsContainer.getContainerArray()[0].hash;
    uint64_t entry2Hash = temporaryContent.settingsContainer.getContainerArray()[1].hash;
    uint64_t entry3Hash = temporaryContent.settingsContainer.getContainerArray()[2].hash;

    temporaryContent.settingsContainer.getContainerArray()[0].hash = 0x42;
    temporaryContent.settingsContainer.getContainerArray()[1].hash = 0x42;
    temporaryContent.settingsContainer.getContainerArray()[2].hash = 0x42;
    temporaryContent.settingsContainer.getContainerArray()[0].value = 0x42;
    temporaryContent.settingsContainer.getContainerArray()[1].value = 0x42;
    temporaryContent.settingsContainer.getContainerArray()[2].value = 0x42;

    std::array<Container::memoryEntry, 3> additionalData;
    additionalData[0].hash = entry3Hash;
    additionalData[0].value = TestSettings::Entry3_max - 5;
    additionalData[1].hash = entry1Hash;
    additionalData[1].value = TestSettings::Entry1_max - 5;
    additionalData[2].hash = entry2Hash;
    additionalData[2].value = TestSettings::Entry2_max - 5;

    eeprom.write(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                 sizeof(IO::EepromContent));
    eeprom.write(IO::MemoryOffset + sizeof(IO::EepromContent),
                 reinterpret_cast<uint8_t *>(additionalData.data()),
                 additionalData.size() * sizeof(Container::memoryEntry));

    ASSERT_TRUE(settingsIo.loadSettings());

    eeprom.read(IO::MemoryOffset, reinterpret_cast<uint8_t *>(&temporaryContent),
                sizeof(IO::EepromContent));

    ASSERT_EQ(correctEeprom, temporaryContent);
}