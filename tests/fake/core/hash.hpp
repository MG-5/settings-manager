#pragma once

#include <cstdint>

namespace bus_node_base
{
static constexpr uint64_t HASH_SEED = 0xcbf29ce484222325;

static uint64_t fnvWithSeed(uint64_t hash, const uint8_t *data, const uint8_t *const dataEnd)
{
    constexpr uint64_t MagicPrime = 0x00000100000001b3;

    for (; data < dataEnd; data++)
    {
        hash = (hash ^ *data) * MagicPrime;
    }

    return hash;
}
} // namespace bus_node_base