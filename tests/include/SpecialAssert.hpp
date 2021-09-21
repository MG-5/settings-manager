#pragma once
#include <gtest/gtest.h>

static void specialAssert(bool val)
{
    if (!val)
        std::abort();
}