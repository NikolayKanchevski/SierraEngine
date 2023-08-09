//
// Created by Nikolay Kanchevski on 24.12.22.
//

#include "RNG.h"

namespace Sierra::Engine
{
    std::random_device RNG::randomDevice;
    std::mt19937_64 RNG::randomizer(randomDevice());

    std::uniform_int_distribution<uint32> RNG::uniformDistribution32;
    std::uniform_int_distribution<uint64> RNG::uniformDistribution64;

    const char RNG::charList[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    /* --- GETTER METHODS --- */

    uint32 RNG::GetRandomUInt()
    {
        return uniformDistribution32(randomizer);
    }

    uint32 RNG::GetRandomUInt(const uint32 min, const uint32 max)
    {
        std::uniform_int_distribution<> distribution(min, max);
        return distribution(randomizer);
    }

    uint64 RNG::GetRandomUInt64()
    {
        return uniformDistribution64(randomizer);
    }

    uint64 RNG::GetRandomUInt64(const uint64 min, const uint64 max)
    {
        std::uniform_int_distribution<> distribution(min, max);
        return distribution(randomizer);
    }

    int32 RNG::GetRandomInt()
    {
        return static_cast<int32>(uniformDistribution32(randomizer));
    }

    int32 RNG::GetRandomInt(const int32 min, const int32 max)
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

    String RNG::GetRandomString(const uint32 length)
    {
        String string;
        for (uint32 i = 0; i < length; i++)
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
