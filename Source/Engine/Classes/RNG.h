//
// Created by Nikolay Kanchevski on 24.12.22.
//

#pragma once

namespace Sierra::Engine
{
    class RNG
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] static uint32 GetRandomUInt();
        [[nodiscard]] static uint32 GetRandomUInt(uint32 min, uint32 max);

        [[nodiscard]] static uint64 GetRandomUInt64();
        [[nodiscard]] static uint64 GetRandomUInt64(uint64 min, uint64 max);

        [[nodiscard]] static int32 GetRandomInt();
        [[nodiscard]] static int32 GetRandomInt(int32 min, int32 max);

        [[nodiscard]] static float GetRandomFloat();
        [[nodiscard]] static float GetRandomFloat(float min, float max);

        [[nodiscard]] static char GetRandomChar();
        [[nodiscard]] static String GetRandomString(uint32 length);

    private:
        static std::random_device randomDevice;
        static std::mt19937_64 randomizer;
        static std::uniform_int_distribution<uint32> uniformDistribution32;
        static std::uniform_int_distribution<uint64> uniformDistribution64;
        static const char charList[];

    };
}
