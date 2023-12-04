//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#include "RNG.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    RNG::RNG(const uint64 seed)
    {
        generator.seed(seed);
    }

    /* --- SETTER METHODS -- */

    void RNG::SetSeed(uint64 seed)
    {
        generator.seed(seed);
    }

}