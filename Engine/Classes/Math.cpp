//
// Created by Nikolay Kanchevski on 2.10.22.
//

#include "Math.h"
#include <glm/common.hpp>

namespace Sierra::Engine::Classes
{

    double Math::Round(const double value, const int decimalPlaces)
    {
        const double multiplier = std::pow(10.0, decimalPlaces);
        return std::ceil(value * multiplier) / multiplier;
    }

    float Math::Round(const float value, const int decimalPlaces)
    {
        const float multiplier = std::pow(10.0, decimalPlaces);
        return std::ceil(value * multiplier) / multiplier;
    }

    double Math::Difference(const double x, const double y)
    {
        return std::abs(x - y);
    }

    float Math::Difference(const float x, const float y)
    {
        return std::abs(x - y);
    }

    int Math::Difference(const int x, const int y)
    {
        return std::abs(x - y);
    }

    bool Math::DifferenceIsBiggerThan(const double x, const double y, const double value)
    {
        return Difference(x, y) > value;
    }

    bool Math::DifferenceIsBiggerThan(const float x, const float y, const float value)
    {
        return Difference(x, y) > value;
    }

    bool Math::DifferenceIsBiggerThan(const int x, const int y, const int value)
    {
        return Difference(x, y) > value;
    }

    double Math::Clamp(double value, double minLimit, double maxLimit)
    {
        return 0;
    }

    float Math::Clamp(float value, float minLimit, float maxLimit)
    {
        if (value > maxLimit) return maxLimit;
        if (value < minLimit) return minLimit;

        return value;
    }

    int Math::Clamp(int value, int minLimit, int maxLimit)
    {
        if (value > maxLimit) return maxLimit;
        if (value < minLimit) return minLimit;

        return value;
    }

    double Math::ToRadians(const double degrees)
    {
        return (PI / 180) * degrees;
    }

    float Math::ToRadians(const float degrees)
    {
        return (PI / 180) * degrees;
    }
}

/* --- CONSTRUCTORS --- */

/* --- POLLING METHODS --- */

/* --- SETTER METHODS --- */

/* --- GETTER METHODS --- */

/* --- DESTRUCTOR --- */
