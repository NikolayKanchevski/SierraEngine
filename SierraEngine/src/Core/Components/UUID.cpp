//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "UUID.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "cert-msc50-cpp"

    UUID::UUID()
        // NOTE: We use std::rand(), despite its limited randomness, because it is ~20x faster for serialization than std::std::mt19937_64
        : hash(static_cast<uint64>(std::rand()) << 32 | std::rand())
    {

    }

    #pragma clang diagnostic pop

}