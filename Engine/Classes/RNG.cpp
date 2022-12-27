//
// Created by Nikolay Kanchevski on 24.12.22.
//

#include "RNG.h"

namespace Sierra::Engine::Classes
{
    std::random_device RNG::randomDevice;
    std::mt19937_64 RNG::randomizer(randomDevice());

    std::uniform_int_distribution<uint32_t> RNG::uniformDistribution32;
    std::uniform_int_distribution<uint64_t> RNG::uniformDistribution64;

    /* --- GETTER METHODS --- */

    uint32_t RNG::GetRandomUInt()
    {
        return uniformDistribution32(randomizer);
    }

    uint64_t RNG::GetRandomUInt64()
    {
        return uniformDistribution64(randomizer);
    }

}

/* --- CONSTRUCTORS --- */

/* --- POLLING METHODS --- */

/* --- SETTER METHODS --- */



/* --- DESTRUCTOR --- */
