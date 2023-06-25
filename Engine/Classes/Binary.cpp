//
// Created by Nikolay Kanchevski on 6.11.22.
//

#include "Binary.h"

namespace Sierra::Engine::Classes
{

    /* --- CONSTRUCTORS --- */

    Binary::Binary(uint givenValue)
        : value(givenValue)
    {

    }

    /* --- SETTER METHODS --- */

    void Binary::SetBit(uint bitIndex, uint newValue)
    {
        newValue != 0 ? static_cast<void>(value |= (1 << bitIndex)) : static_cast<void>(value &= ~(1 << bitIndex));
    }

    /* --- GETTER METHODS --- */

    uint Binary::GetBitCount(Binary binary)
    {
        size_t counter = 0;

        for (size_t i = 0; i < 8 * UINT_SIZE; i++) {
            counter += static_cast<bool>(binary.value & (1 << i));
        }

        return counter;
    }

    /* --- OPERATORS --- */

    Binary &Binary::operator=(uint givenValue)
    {
        value = givenValue;
        return *this;
    }
}