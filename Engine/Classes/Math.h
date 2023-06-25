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
        std::is_same_v<T, uint8> ||
        std::is_same_v<T, uint16> ||
        std::is_same_v<T, uint32> ||
        std::is_same_v<T, uint64> ||
        std::is_same_v<T, uSize> ||
        std::is_same_v<T, int8> ||
        std::is_same_v<T, int16> ||
        std::is_same_v<T, int32> ||
        std::is_same_v<T, int64> ||
        std::is_same_v<T, iSize> ||
        std::is_same_v<T, short> ||
        std::is_same_v<T, unsigned short> ||
        std::is_same_v<T, float> ||
        std::is_same_v<T, double>
    ;

    /// @brief A compile-time expression, that checks whether a given type <T> supports mathematical expressions (such as +, -, *, etc.).
    ///        The most common ones are: float, Vector3, Matrix4x4, etc.
    template<typename T>
    inline constexpr bool IsTypeMathematical =
        IsTypeNumeric<T> ||
        std::is_same_v<T, Vector2> ||
        std::is_same_v<T, Vector3> ||
        std::is_same_v<T, Vector4> ||
        std::is_same_v<T, Vector2Int> ||
        std::is_same_v<T, Vector3Int> ||
        std::is_same_v<T, Vector4Int> ||
        std::is_same_v<T, Quaternion> ||
        std::is_same_v<T, Matrix3x3> ||
        std::is_same_v<T, Matrix4x4> ||
        std::is_same_v<T, Matrix3x3Int> ||
        std::is_same_v<T, Matrix4x4Int>
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
    [[nodiscard]] inline bool IsDifferenceBiggerThan(T x, T y, T value)
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

    /// @brief Finds the closest number that is a multiple of N. Useful for alignment.
    /// @param number What number to find the closes multiple of.
    /// @param multiple What multiple to be looking for.
    [[nodiscard]] inline int FindClosestMultipleOfN(const int number, const int multiple)
    {
        return ((number + multiple - 1) / multiple) * multiple;
    }

    /// @brief Finds the closest number that is a multiple of 4. Useful for alignment.
    /// @param number What number to find the closes multiple of.
    [[nodiscard]] inline int FindClosestMultipleOf4(const int number)
    {
        return FindClosestMultipleOfN(number, 4);
    }

    /// @brief Finds the closest number that is a multiple of 16. Useful for alignment.
    /// @param number What number to find the closes multiple of.
    [[nodiscard]] inline int FindClosestMultipleOf16(const int number)
    {
        return FindClosestMultipleOfN(number, 16);
    }
}