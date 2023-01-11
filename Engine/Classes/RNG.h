//
// Created by Nikolay Kanchevski on 24.12.22.
//

#pragma once

#include <random>

namespace Sierra::Engine::Classes
{
    class RNG
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] static uint32_t GetRandomUInt();
        [[nodiscard]] static uint64_t GetRandomUInt64();

        [[nodiscard]] static int GetRandomInt(int min, int max);
        [[nodiscard]] static float GetRandomFloat(float min, float max);

    private:
        static std::random_device randomDevice;
        static std::mt19937_64 randomizer;
        static std::uniform_int_distribution<uint32_t> uniformDistribution32;
        static std::uniform_int_distribution<uint64_t> uniformDistribution64;

    };
}
