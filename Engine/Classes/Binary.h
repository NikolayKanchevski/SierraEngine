//
// Created by Nikolay Kanchevski on 6.11.22.
//

#pragma once

#include <cstdint>
#include <bitset>

namespace Sierra::Engine::Classes
{

    class Binary
    {
    public:
        /* --- CONSTRUCTORS --- */
        Binary() = default;
        Binary(uint32_t givenValue);

        /* --- SETTER METHODS --- */
        void SetBit(uint32_t bitIndex, uint32_t newValue);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsBitSet(const uint32_t bitIndex) const { return (value & (1 << bitIndex)) != 0; };
        [[nodiscard]] std::string ToString() const { return std::bitset<16>(value).to_string(); };

        /* --- OPERATORS --- */
        Binary& operator=(uint32_t givenValue);
        operator uint32_t() const noexcept { return value; }
    private:
        uint32_t value = 0;

    };

}
