//
// Created by Nikolay Kanchevski on 24.12.22.
//

#pragma once

namespace Sierra::Engine::Classes
{
    class RNG
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] static uint GetRandomUInt();
        [[nodiscard]] static uint GetRandomUInt(uint min, uint max);

        [[nodiscard]] static uint64 GetRandomUInt64();
        [[nodiscard]] static uint64 GetRandomUInt64(uint64 min, uint64 max);

        [[nodiscard]] static int GetRandomInt();
        [[nodiscard]] static int GetRandomInt(int min, int max);

        [[nodiscard]] static float GetRandomFloat();
        [[nodiscard]] static float GetRandomFloat(float min, float max);

        [[nodiscard]] static char GetRandomChar();
        [[nodiscard]] static String GetRandomString(uint length);

    private:
        static std::random_device randomDevice;
        static std::mt19937_64 randomizer;
        static std::uniform_int_distribution<uint> uniformDistribution32;
        static std::uniform_int_distribution<uint64> uniformDistribution64;
        static const char charList[];

    };
}
