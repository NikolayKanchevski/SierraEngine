//
// Created by Nikolay Kanchevski on 6.11.22.
//

#include "Binary.h"

namespace Sierra::Engine::Classes
{

    /* --- CONSTRUCTORS --- */

    Binary::Binary(uint32_t givenValue)
        : value(givenValue)
    {

    }

    /* --- SETTER METHODS --- */

    void Binary::SetBit(uint32_t bitIndex, uint32_t newValue)
    {
        newValue != 0 ? static_cast<void>(value |= (1 << bitIndex)) : static_cast<void>(value &= ~(1 << bitIndex));
    }

    /* --- OPERATORS --- */

    Binary &Binary::operator=(uint32_t givenValue)
    {
        this->value = givenValue;
        return *this;
    }
}