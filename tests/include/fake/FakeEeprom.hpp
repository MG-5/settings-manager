#pragma once

#include "core/SafeAssert.h"
#include "eeprom-drivers/EepromBase.hpp"
#include <array>
#include <cstdint>
#include <cstring>

using AddressSize = uint16_t;

class FakeEeprom : public EepromBase<64, AddressSize>
{
public:
    explicit FakeEeprom()
    {
        fakeMemory.fill(0xFF);
    }

    void read(AddressSize address, uint8_t *buffer, size_t length) override
    {
        SafeAssert(length != 0);
        SafeAssert(length - 1 <= std::numeric_limits<uint16_t>::max());
        SafeAssert(!doesAddressExceedLimit(address + length - 1));

        std::memcpy(buffer, fakeMemory.data() + address, length);
    }

    void write(AddressSize address, const uint8_t *data, size_t length) override
    {
        SafeAssert(length != 0);
        SafeAssert(length - 1 <= std::numeric_limits<uint16_t>::max());
        SafeAssert(!doesAddressExceedLimit(address + length - 1));

        std::memcpy(fakeMemory.data() + address, data, length);
    }

private:
    std::array<uint8_t, getSizeInBytes()> fakeMemory;
};