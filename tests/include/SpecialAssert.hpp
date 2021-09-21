#pragma once
#include <gtest/gtest.h>

static void specialAssert(bool val)
{
    if (!val)
        throw std::runtime_error("special assert");
}