#pragma once
#include <cstdint>
#include <cstring>

template <size_t sizeInBytes>
class Eeprom24LCXX
{
public:
    Eeprom24LCXX()
    {
        fakeMemory.fill(0xFF);
    }

    void read(uint16_t address, uint8_t *buffer, uint16_t length)
    {
        if (address >= sizeInBytes || address + length > sizeInBytes)
            return;

        std::memcpy(buffer, fakeMemory.data() + address, length);
    }

    void write(uint16_t address, const uint8_t *data, uint16_t length)
    {
        if (address >= sizeInBytes || address + length > sizeInBytes)
            return;

        std::memcpy(fakeMemory.data() + address, data, length);
    }

private:
    std::array<uint8_t, sizeInBytes> fakeMemory;
};

using Eeprom24LC64 = Eeprom24LCXX<256>;