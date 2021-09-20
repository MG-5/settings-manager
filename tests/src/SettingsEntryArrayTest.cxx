#include "SettingsEntryArray.hpp"

#include <gtest/gtest.h>

namespace
{
using namespace settings;

constexpr std::array settingsArray1 = {
    SettingsEntry(0, 1, 2, "entry1"), //
    SettingsEntry(1, 2, 3, "entry2"), //
    SettingsEntry(0, 1, 2, "entry3")  //
};

constexpr std::array settingsArray2 = {
    SettingsEntry(0, 1, 2, "entry1"), //
    SettingsEntry(1, 2, 3, "entry2"), //
    SettingsEntry(0, 1, 2, "entry1")  //
};

SettingsEntryArray<settingsArray1.size()> entryArray(settingsArray1);

TEST(SettingsEntryArray, NoDuplicates)
{
    EXPECT_NO_FATAL_FAILURE(SettingsEntryArray<settingsArray1.size()> foo1(settingsArray1));

    EXPECT_DEATH(SettingsEntryArray<settingsArray2.size()> foo2(settingsArray2), "");
};

TEST(SettingsEntryArray, Size)
{
    EXPECT_EQ(entryArray.size(), settingsArray1.size());
};

TEST(SettingsEntryArray, getEntries)
{
    EXPECT_EQ(entryArray.getEntries(), settingsArray1);
};

TEST(SettingsEntryArray, getEntryByName)
{
    EXPECT_EQ(entryArray.getEntryByName("entry1"), settingsArray1[0]);
    EXPECT_EQ(entryArray.getEntryByName("entry2"), settingsArray1[1]);
    EXPECT_EQ(entryArray.getEntryByName("entry3"), settingsArray1[2]);
    EXPECT_DEATH(entryArray.getEntryByName(""), "");
    EXPECT_DEATH(entryArray.getEntryByName("nonExistentEntry"), "");
};

TEST(SettingsEntryArray, atOperator)
{
    EXPECT_EQ(entryArray.at(0), settingsArray1[0]);
    EXPECT_EQ(entryArray.at(1), settingsArray1[1]);
    EXPECT_EQ(entryArray.at(2), settingsArray1[2]);
    EXPECT_DEATH(entryArray.at(10), "");
};

TEST(SettingsEntryArray, getIndex)
{
    EXPECT_EQ(entryArray.getIndex("entry1"), 0);
    EXPECT_EQ(entryArray.getIndex("entry2"), 1);
    EXPECT_EQ(entryArray.getIndex("entry3"), 2);
    EXPECT_DEATH(entryArray.getIndex(""), "");
    EXPECT_DEATH(entryArray.getIndex("nonExistentEntry"), "");
};

} // namespace
