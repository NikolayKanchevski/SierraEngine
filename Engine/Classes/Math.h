//
// Created by Nikolay Kanchevski on 2.10.22.
//

#pragma once

/// @brief Contains many useful mathematical methods.
namespace Sierra::Engine::Classes::Math
{
    /// @brief The value of PI as a double. Happy March 14!
    constexpr const double PI = 3.141592653589793;

    /// @brief A compile-time expression, that checks whether a given type <T> is numeric (int, uint, float, etc.).
    template<typename T>
    inline constexpr bool IsTypeNumeric =
        std::is_same<T, uint8>::value ||
        std::is_same<T, uint16>::value ||
        std::is_same<T, uint32>::value ||
        std::is_same<T, uint64>::value ||
        std::is_same<T, uSize>::value ||
        std::is_same<T, int8>::value ||
        std::is_same<T, int16>::value ||
        std::is_same<T, int32>::value ||
        std::is_same<T, int64>::value ||
        std::is_same<T, iSize>::value ||
        std::is_same<T, short>::value ||
        std::is_same<T, unsigned short>::value ||
        std::is_same<T, float>::value ||
        std::is_same<T, double>::value
    ;

    /// @brief A compile-time expression, that checks whether a given type <T> supports mathematical expressions (such as +, -, *, etc.).
    ///        The most common ones are: float, Vector3, Matrix4x4, etc.
    template<typename T>
    inline constexpr bool IsTypeMathematical =
        IsTypeNumeric<T> ||
        std::is_same<T, Vector2>::value ||
        std::is_same<T, Vector3>::value ||
        std::is_same<T, Vector4>::value ||
        std::is_same<T, Vector2Int>::value ||
        std::is_same<T, Vector3Int>::value ||
        std::is_same<T, Vector4Int>::value ||
        std::is_same<T, Quaternion>::value ||
        std::is_same<T, Matrix3x3>::value ||
        std::is_same<T, Matrix4x4>::value ||
        std::is_same<T, Matrix3x3Int>::value ||
        std::is_same<T, Matrix4x4Int>::value
    ;

    /// @brief Returns the absolute value of a given number.
    template<typename T, ENABLE_IF(IsTypeNumeric<T>)>
    [[nodiscard]] inline T Absolute(T a)
    {
        return glm::abs(a);
    }

    /// @brief Returns the difference between two values. It is always positive.
    /// @param x First value.
    /// @param y Second value.
    template<typename T, ENABLE_IF(IsTypeNumeric<T>)>
    [[nodiscard]] inline T Difference(T x, T y)
    {
        return Absolute(x - y);
    }

    /// @brief Checks if the difference between two values is bigger than or equal to some value.
    /// @param x First value.
    /// @param y Second value.
    /// @param value Minimum difference (exclusive).
    template<typename T, ENABLE_IF(IsTypeNumeric<T>)>
    [[nodiscard]] inline bool DifferenceIsBiggerThan(T x, T y, T value)
    {
        return Difference(x, y) > value;
    }

    /// @brief Rounds a given value to N decimal places. By default, it round the value to a whole number.
    /// @param value The value to round.
    /// @param decimalPlaces To how many decimal places to round the value.
    template<typename T, ENABLE_IF(IsTypeNumeric<T>)>
    [[nodiscard]] inline T Round(T value, int decimalPlaces = 0)
    {
        T multiplier = glm::pow(10.0, decimalPlaces);
        return glm::round(value * multiplier) / multiplier;
    }

    /// @brief Checks if a given value is beyond the maximum limit or less than the minimum limit.
    /// @param value The variable to be tested.
    /// @param minLimit Minimum limit.
    /// @param maxLimit Maximum limit.
    template<typename T, ENABLE_IF(IsTypeNumeric<T>)>
    [[nodiscard]] inline T Clamp(T value, T minLimit, T maxLimit)
    {
        if (value > maxLimit) return maxLimit;
        if (value < minLimit) return minLimit;

        return value;
    }

    /// @brief Converts a given value in degrees to radians
    /// @param degrees Value to convert (in degrees).
    template<typename T, ENABLE_IF(IsTypeNumeric<T>)>
    [[nodiscard]] inline T ToRadians(T degrees)
    {
        return (PI / 180) * degrees;
    }
}