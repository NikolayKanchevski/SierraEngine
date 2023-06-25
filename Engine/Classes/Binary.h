//
// Created by Nikolay Kanchevski on 6.11.22.
//

#pragma once

namespace Sierra::Engine::Classes
{

    class Binary
    {
    public:
        /* --- CONSTRUCTORS --- */
        Binary() = default;

        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "google-explicit-constructor"
        Binary(uint givenValue);
        #pragma clang diagnostic pop

        /* --- SETTER METHODS --- */
        void SetBit(uint bitIndex, uint newValue);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsBitSet(const uint bitIndex) const { return (value & (1 << bitIndex)) != 0; };
        [[nodiscard]] inline uint GetBitCount() const { return GetBitCount(value); };
        [[nodiscard]] static uint GetBitCount(Binary binary);
        [[nodiscard]] String ToString() const { return std::bitset<16>(value).to_string(); };

        /* --- OPERATORS --- */
        Binary& operator=(uint givenValue);
        operator uint() const noexcept { return value; }
    private:
        uint value = 0;

    };

}
