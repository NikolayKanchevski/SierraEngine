//
// Created by Nikolay Kanchevski on 2.10.22.
//

#include "Math.h"

#include <glm/glm.hpp>

namespace Sierra::Engine::Classes::Math
{

    double Round(const double value, const int decimalPlaces)
    {
        const double multiplier = std::pow(10.0, decimalPlaces);
        return std::ceil(value * multiplier) / multiplier;
    }

    float Round(const float value, const int decimalPlaces)
    {
        const float multiplier = std::pow(10.0, decimalPlaces);
        return std::ceil(value * multiplier) / multiplier;
    }

    double Difference(const double x, const double y)
    {
        return std::abs(x - y);
    }

    float Difference(const float x, const float y)
    {
        return std::abs(x - y);
    }

    int Difference(const int x, const int y)
    {
        return std::abs(x - y);
    }

    bool DifferenceIsBiggerThan(const double x, const double y, const double value)
    {
        return Difference(x, y) > value;
    }

    bool DifferenceIsBiggerThan(const float x, const float y, const float value)
    {
        return Difference(x, y) > value;
    }

    bool DifferenceIsBiggerThan(const int x, const int y, const int value)
    {
        return Difference(x, y) > value;
    }

    double Clamp(double value, double minLimit, double maxLimit)
    {
        if (value > maxLimit) return maxLimit;
        if (value < minLimit) return minLimit;

        return value;
    }

    float Clamp(float value, float minLimit, float maxLimit)
    {
        if (value > maxLimit) return maxLimit;
        if (value < minLimit) return minLimit;

        return value;
    }

    int Clamp(int value, int minLimit, int maxLimit)
    {
        if (value > maxLimit) return maxLimit;
        if (value < minLimit) return minLimit;

        return value;
    }

    double ToRadians(const double degrees)
    {
        return (PI / 180) * degrees;
    }

    float ToRadians(const float degrees)
    {
        return (PI / 180) * degrees;
    }

}