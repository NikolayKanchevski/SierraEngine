//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#pragma once

namespace Sierra
{

    template<typename T> concept RNGType = std::is_integral_v<T> || std::is_floating_point_v<T>;

    class SIERRA_API RNG final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit RNG(uint64 seed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

        /* --- SETTER METHODS --- */
        void SetSeed(uint64 seed);

        /* --- GETTER METHODS --- */
        template<RNGType Type>
        [[nodiscard]] Type Random(const Type min = std::numeric_limits<Type>::min(), const Type max = std::numeric_limits<Type>::max())
        {
            if constexpr(std::is_same_v<Type, int16> || std::is_same_v<Type, int32> || std::is_same_v<Type, int64> || std::is_same_v<Type, uint16> || std::is_same_v<Type, uint32> || std::is_same_v<Type, uint64>)
            {
                std::uniform_int_distribution<Type> distribution(min, max);
                return distribution(generator);
            }
            else if constexpr(std::is_same_v<Type, char> || std::is_same_v<Type, uchar>)
            {
                return static_cast<char>(Random<int8>('!', '~'));
            }
            else if constexpr(std::is_same_v<Type, int8>)
            {
                return static_cast<int8>(Random<int16>(min, max));
            }
            else if constexpr(std::is_same_v<Type, uint8>)
            {
                return static_cast<uint8>(Random<int16>(min, max));
            }
            else if constexpr(std::is_same_v<Type, float32> || std::is_same_v<Type, float64>)
            {
                std::uniform_real_distribution<Type> distribution(min, max);
                return distribution(generator);
            }
        }

    private:
        std::mt19937_64 generator;

    };

}
