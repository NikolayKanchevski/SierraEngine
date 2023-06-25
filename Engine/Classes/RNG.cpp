//
// Created by Nikolay Kanchevski on 24.12.22.
//

#include "RNG.h"

namespace Sierra::Engine::Classes
{
    std::random_device RNG::randomDevice;
    std::mt19937_64 RNG::randomizer(randomDevice());

    std::uniform_int_distribution<uint> RNG::uniformDistribution32;
    std::uniform_int_distribution<uint64> RNG::uniformDistribution64;

    const char RNG::charList[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    /* --- GETTER METHODS --- */

    uint RNG::GetRandomUInt()
    {
        return uniformDistribution32(randomizer);
    }

    uint RNG::GetRandomUInt(uint min, uint max)
    {
        std::uniform_int_distribution<> distribution(static_cast<uint>(min), static_cast<uint>(max));
        return distribution(randomizer);
    }

    uint64 RNG::GetRandomUInt64()
    {
        return uniformDistribution64(randomizer);
    }

    uint64 RNG::GetRandomUInt64(uint64 min, uint64 max)
    {
        std::uniform_int_distribution<> distribution(static_cast<uint>(min), static_cast<uint>(max));
        return distribution(randomizer);
    }

    int RNG::GetRandomInt()
    {
        return static_cast<int>(uniformDistribution32(randomizer));
    }

    int RNG::GetRandomInt(const int min, const int max)
    {
        std::uniform_int_distribution<> distribution(min, max);
        return distribution(randomizer);
    }

    float RNG::GetRandomFloat()
    {
        std::uniform_real_distribution<> distribution(FLOAT_MIN, FLOAT_MAX);
        return static_cast<float>(distribution(randomizer));
    }

    float RNG::GetRandomFloat(const float min, const float max)
    {
        std::uniform_real_distribution<> distribution(min, max);
        return static_cast<float>(distribution(randomizer));
    }

    char RNG::GetRandomChar()
    {
        return charList[GetRandomUInt(0, 61)];
    }

    String RNG::GetRandomString(const uint length)
    {
        String string;
        for (uint i = 0; i < length; i++)
        {
            string += GetRandomChar();
        }

        return string;
    }

}

/* --- CONSTRUCTORS --- */

/* --- POLLING METHODS --- */

/* --- SETTER METHODS --- */



/* --- DESTRUCTOR --- */
