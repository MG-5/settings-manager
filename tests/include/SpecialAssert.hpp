#pragma once
#include <gtest/gtest.h>

static void specialAssert(bool val)
{
    std::abort();
}