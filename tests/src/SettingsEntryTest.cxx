#include "SettingsEntry.hpp"
#include <gtest/gtest.h>

namespace
{
using namespace settings;

SettingsEntry entry1(0, 1, 2, "testEntry");
SettingsEntry entry2(1, 2, 3, "testEntry");
SettingsEntry entry3(0, 1, 2, "otherEntry");

constexpr SettingsEntry entry4(0, 1, 2, "testEntry");
constexpr SettingsEntry entry5(1, 2, 3, "testEntry");
constexpr SettingsEntry entry6(0, 1, 2, "otherEntry");

TEST(SettingsEntry, Equality)
{
    EXPECT_TRUE(entry1 == entry2);
    EXPECT_FALSE(entry1 == entry3);
    EXPECT_TRUE(entry4 == entry5);
    EXPECT_FALSE(entry4 == entry6);
};

TEST(SettingsEntry, NotEquality)
{
    EXPECT_FALSE(entry1 != entry2);
    EXPECT_TRUE(entry1 != entry3);
    EXPECT_FALSE(entry4 != entry5);
    EXPECT_TRUE(entry4 != entry6);
};

TEST(SettingsEntry, WrongConstruction)
{
    EXPECT_DEATH(SettingsEntry(3, 1, 2, "wrongEntry"), "");
    EXPECT_DEATH(SettingsEntry(0, 3, 2, "wrongEntry"), "");
    EXPECT_DEATH(SettingsEntry(1, 1, 0, "wrongEntry"), "");
};

TEST(SettingsEntry, CopyConstructor)
{
    SettingsEntry testEntry1 = entry1;
    constexpr SettingsEntry testEntry2 = entry4;

    EXPECT_EQ(testEntry1, entry1);
    EXPECT_EQ(testEntry2, entry4);
}

} // namespace