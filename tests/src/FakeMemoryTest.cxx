#include <gtest/gtest.h>
#include <i2c-drivers/24lcxx.hpp>

namespace
{
constexpr auto SizeInBytes = 256;

class FakeMemoryTest : public ::testing::Test
{
protected:
    FakeMemoryTest()
    {
    }
    Eeprom24LCXX<SizeInBytes> eeprom;
};

TEST_F(FakeMemoryTest, readDefaultEeprom)
{
    std::array<uint8_t, SizeInBytes> tempMemory;
    eeprom.read(0, tempMemory.data(), SizeInBytes);

    for (size_t i = 0; i < SizeInBytes; i++)
    {
        EXPECT_EQ(tempMemory[i], 0xFF);
    }
};

TEST_F(FakeMemoryTest, writeRead)
{
    std::array<uint8_t, SizeInBytes> tempMemory;

    for (size_t i = 0; i < SizeInBytes; i++)
    {
        eeprom.write(i, reinterpret_cast<uint8_t *>(&i), 1);
    }

    std::array<uint8_t, SizeInBytes> temp2Memory;
    eeprom.read(0, temp2Memory.data(), SizeInBytes);

    for (size_t i = 0; i < SizeInBytes; i++)
    {
        EXPECT_EQ(temp2Memory[i], i);
    }
};

TEST_F(FakeMemoryTest, rangeBounds)
{
    std::array<uint8_t, SizeInBytes> tempMemory{};
    std::array<uint8_t, SizeInBytes> tempMemory2{};

    for (size_t i = 0; i < SizeInBytes; i++)
    {
        tempMemory[i] = i;
    }
    eeprom.write(0, tempMemory.data(), SizeInBytes + 8); // range bound violation
    eeprom.read(0, tempMemory2.data(), SizeInBytes);

    for (size_t i = 0; i < SizeInBytes; i++)
    {
        EXPECT_EQ(tempMemory2[i], 0xFF);
    }

    std::array<uint8_t, SizeInBytes> pattern{};
    pattern.fill(0xA5);
    tempMemory2 = pattern;

    eeprom.write(0, tempMemory.data(), SizeInBytes);
    eeprom.read(0, tempMemory2.data(), SizeInBytes + 8); // range bound violation
    EXPECT_EQ(tempMemory2, pattern);

    eeprom.read(0, tempMemory2.data(), SizeInBytes);
    EXPECT_EQ(tempMemory, tempMemory2);
};

} // namespace