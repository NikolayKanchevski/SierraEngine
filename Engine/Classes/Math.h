//
// Created by Nikolay Kanchevski on 2.10.22.
//

#pragma once

/// @brief Contains many useful mathematical methods.
namespace Sierra::Engine::Classes::Math
{
    /// @brief The value of PI as a double. Happy March 14!
    constexpr const double PI = 3.141592653589793;

    /// @brief Rounds a given value to N decimal places. By default, it round the value to a whole number.
    /// @param value The value to round. (double)
    /// @param decimalPlaces To how many decimal places to round the value.
    double Round(double value, int decimalPlaces = 0);

    /// @brief Rounds a given value to N decimal places. By default, it round the value to a whole number.
    /// @param value The value to round. (float)
    /// @param decimalPlaces To how many decimal places to round the value.
    float Round(float value, int decimalPlaces = 0);

    /// Returns the difference between two values. It is always positive.
    /// @param x First value. (double)
    /// @param y Second value. (double)
    double Difference(double x, double y);

    /// @brief Returns the difference between two values. It is always positive.
    /// @param x First value. (float)
    /// @param y Second value. (float)
    float Difference(float x, float y);

    /// @brief Returns the difference between two values. It is always positive.
    /// @param x First value. (int)
    /// @param y Second value. (int)
    int Difference(int x, int y);

    /// @brief Checks if the difference between two values is bigger than or equal to some value.
    /// @param x First value. (double)
    /// @param y Second value. (double)
    /// @param value Minimum difference. (double)
    bool DifferenceIsBiggerThan(double x, double y, double value);

    /// @brief Checks if the difference between two values is bigger than or equal to some value.
    /// @param x First value. (float)
    /// @param y Second value. (float)
    /// @param value Minimum difference. (float)
    bool DifferenceIsBiggerThan(float x, float y, float value);

    /// @brief Checks if the difference between two values is bigger than or equal to some value.
    /// @param x First value. (int)
    /// @param y Second value. (int)
    /// @param value Minimum difference. (int)
    bool DifferenceIsBiggerThan(int x, int y, int value);

    /// @brief Checks if a given value is beyond the maximum limit or less than the minimum limit.
    /// @param value The variable to be tested. (double)
    /// @param minLimit Minimum limit. (double)
    /// @param maxLimit Maximum limit. (double)
    double Clamp(double value, double minLimit, double maxLimit);

    /// @brief Checks if a given value is beyond the maximum limit or less than the minimum limit.
    /// @param value The variable to be tested. (float)
    /// @param minLimit Minimum limit. (float)
    /// @param maxLimit Maximum limit. (float)
    float Clamp(float value, float minLimit, float maxLimit);

    /// @brief Checks if a given value is beyond the maximum limit or less than the minimum limit.
    /// @param value The variable to be tested. (int)
    /// @param minLimit Minimum limit. (int)
    /// @param maxLimit Maximum limit. (int)
    int Clamp(int value, int minLimit, int maxLimit);

    /// @brief Converts a given value in degrees to radians
    /// @param degrees Value to convert (double in degrees)
    double ToRadians(double degrees);

    /// @brief Converts a given value in degrees to radians
    /// @param degrees Value to convert (float in degrees)
    float ToRadians(float degrees);
}