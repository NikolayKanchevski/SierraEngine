//
// Created by Nikolay Kanchevski on 6.11.22.
//

#pragma once

namespace Sierra::Engine
{

    class Binary
    {
    public:
        /* --- CONSTRUCTORS --- */
        Binary() = default;

        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "google-explicit-constructor"
            Binary(uint32 givenValue);
        #pragma clang diagnostic pop

        /* --- SETTER METHODS --- */
        void SetBit(uint32 bitIndex, bool newValue);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsBitSet(const uint32 bitIndex) const { return (value & (1 << bitIndex)) != 0; };
        [[nodiscard]] inline uint32 GetBitCount() const { return GetBitCount(value); };
        [[nodiscard]] static uint32 GetBitCount(Binary binary);
        [[nodiscard]] String ToString() const { return std::bitset<16>(value).to_string(); };

        /* --- OPERATORS --- */
        Binary& operator=(uint32 givenValue);
        operator uint() const { return value; }
    private:
        uint32 value = 0;

    };

}
