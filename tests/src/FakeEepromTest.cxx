#include "fake/FakeEeprom.hpp"
#include <exception>
#include <gtest/gtest.h>

namespace
{
constexpr auto SizeInBytes = FakeEeprom::getSizeInBytes();

class FakeEepromTest : public ::testing::Test
{
protected:
    FakeEepromTest() = default;

    FakeEeprom eeprom{};
};

TEST_F(FakeEepromTest, readDefaultEeprom)
{
    std::array<uint8_t, SizeInBytes> tempMemory = {0};
    eeprom.read(0, tempMemory.data(), SizeInBytes);

    for (size_t i = 0; i < SizeInBytes; i++)
    {
        ASSERT_EQ(tempMemory[i], 0xFF);
    }
};

TEST_F(FakeEepromTest, writeRead)
{
    std::array<uint8_t, SizeInBytes> tempMemory = {0};

    uint8_t rollingData = 0;
    for (size_t i = 0; i < SizeInBytes; i++)
    {
        eeprom.write(i, &rollingData, 1);

        uint8_t readData = 0;
        eeprom.read(i, &readData, 1);
        ASSERT_EQ(readData, rollingData);
        rollingData++;
    }
};

TEST_F(FakeEepromTest, rangeBounds)
{
    std::array<uint8_t, SizeInBytes> tempMemory{};
    std::array<uint8_t, SizeInBytes> tempMemory2{};

    uint8_t rollingData = 0;
    for (size_t i = 0; i < SizeInBytes; i++)
    {
        tempMemory[i] = rollingData;
        rollingData++;
    }

    // range bound violation, content should stay uninitialized
    EXPECT_THROW(eeprom.write(0, tempMemory.data(), SizeInBytes + 1), std::runtime_error);
    eeprom.read(0, tempMemory2.data(), SizeInBytes);
    for (size_t i = 0; i < SizeInBytes; i++)
    {
        ASSERT_EQ(tempMemory2[i], 0xFF);
    }

    std::array<uint8_t, SizeInBytes> pattern{};
    pattern.fill(0xA5);
    tempMemory2 = pattern;

    eeprom.write(0, tempMemory.data(), SizeInBytes);
    // range bound violation, nothing should be read
    EXPECT_THROW(eeprom.read(0, tempMemory2.data(), SizeInBytes + 1), std::runtime_error);
    ASSERT_EQ(tempMemory2, pattern);

    eeprom.read(0, tempMemory2.data(), SizeInBytes);
    ASSERT_EQ(tempMemory, tempMemory2);
};

} // namespace