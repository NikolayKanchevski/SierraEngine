//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#pragma once

namespace Sierra
{

    class SIERRA_API RNG
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit RNG(uint64 seed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

        /* --- SETTER METHODS --- */
        void SetSeed(uint64 seed);

        /* --- GETTER METHODS --- */
        template<typename T>
        [[nodiscard]] T Random(const T min, const T max)
        {
            static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "Method requires the given template argument to be a numeric type!");

            if constexpr(std::is_same_v<T, int16> || std::is_same_v<T, int32> || std::is_same_v<T, int64> || std::is_same_v<T, uint16> || std::is_same_v<T, uint32> || std::is_same_v<T, uint64>)
            {
                std::uniform_int_distribution<T> distribution(min, max);
                return distribution(generator);
            }
            else if constexpr(std::is_same_v<T, char> || std::is_same_v<T, uchar>)
            {
                return static_cast<char>(Random<int8>(33, 126)); // [ ! | ~ ]
            }
            else if constexpr(std::is_same_v<T, int8>)
            {
                return static_cast<int8>(Random<int16>(min, max));
            }
            else if constexpr(std::is_same_v<T, uint8>)
            {
                return static_cast<uint8>(Random<int16>(min, max));
            }
            else if constexpr(std::is_same_v<T, float32> || std::is_same_v<T, float64>)
            {
                std::uniform_real_distribution<T> distribution(min, max);
                return distribution(generator);
            }
        }

        template<typename T>
        [[nodiscard]] T Random()
        {
            return Random<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        }

    private:
        std::mt19937_64 generator;

    };

}
