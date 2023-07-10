//
// Created by Nikolay Kanchevski on 2.10.22.
//

#pragma once

/// @brief Contains many useful mathematical methods.
namespace Sierra::Engine::Math
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

    /// @brief A compile-time expression, that checks whether a given type <T> supports mathematical expressions (such as +, -, *, etc.). Most common ones are: float, Vector3, Matrix4x4, etc.
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

    /// @brief Creates only a translation matrix.
    [[nodiscard]] inline Matrix4x4 CreateTranslation(const Vector3 position)
    {
        return glm::translate(Matrix4x4(1.0f), position);
    }

    /// @brief Creates only a rotation matrix.
    [[nodiscard]] inline Matrix4x4 CreateRotation(const float angle, const Vector3 direction)
    {
        return glm::rotate(Matrix4x4(1.0f), glm::radians(angle), direction);
    }

    /// @brief Creates only a scale matrix.
    [[nodiscard]] inline Matrix4x4 CreateScale(const Vector3 scale)
    {
        return glm::scale(Matrix4x4(1.0f), scale);
    }

    /// @brief Creates a complete model matrix from given position, rotation and scale.
    [[nodiscard]] inline Matrix4x4 CreateModelMatrix(const Vector3 position, const Vector3 rotation, const Vector3 scale = { 1.0f, 1.0f, 1.0f })
    {
        Matrix4x4 rotationMatrix = glm::toMat4(Quaternion({ glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z) }));
        return CreateTranslation(position) * rotationMatrix * CreateScale(scale);
    }

    /// @brief Splits a 4x4 model matrix into the components it is made up op - translation, rotation and scale.
    [[nodiscard]] inline bool DecomposeModelMatrix(const Matrix4x4 &matrix, Vector3 &translation, Vector3 &rotation, Vector3 &scale)
    {
        Matrix4x4 localMatrix(matrix);

        // Normalize the matrix.
        if (glm::epsilonEqual(localMatrix[3][3], 0.0f, FLOAT_EPSILON))
            return false;

        // First, isolate perspective. This is the messiest.
        if (
            glm::epsilonNotEqual(localMatrix[0][3], 0.0f, FLOAT_EPSILON) ||
            glm::epsilonNotEqual(localMatrix[1][3], 0.0f, FLOAT_EPSILON) ||
            glm::epsilonNotEqual(localMatrix[2][3], 0.0f, FLOAT_EPSILON))
        {
            // Clear the perspective partition
            localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = 0.0f;
            localMatrix[3][3] = 1.0f;
        }

        // Next take care of translation (easy).
        translation = Vector3 (localMatrix[3]);
        localMatrix[3] = Vector4 (0, 0, 0, localMatrix[3].w);

        Vector3 Row[3];

        // Now get scale and shear.
        for (glm::length_t i = 0; i < 3; ++i)
        {
            for (glm::length_t j = 0; j < 3; ++j)
            {
                Row[i][j] = localMatrix[i][j];
            }
        }

        // Compute X scale factor and normalize first row.
        scale.x = length(Row[0]);
        Row[0] = glm::detail::scale(Row[0], 1.0f);
        scale.y = length(Row[1]);
        Row[1] = glm::detail::scale(Row[1], 1.0f);
        scale.z = length(Row[2]);
        Row[2] = glm::detail::scale(Row[2], 1.0f);

        rotation.y = asin(-Row[0][2]);
        if (cos(rotation.y) != 0) {
            rotation.x = atan2(Row[1][2], Row[2][2]);
            rotation.z = atan2(Row[0][1], Row[0][0]);
        }
        else {
            rotation.x = atan2(-Row[2][0], Row[1][1]);
            rotation.z = 0;
        }

        rotation.x = glm::degrees(rotation.x);
        rotation.y = glm::degrees(rotation.y);
        rotation.z = glm::degrees(rotation.z);

        return true;
    }

    /// @brief Creates a 4x4 matrix from a given eye.
    /// @param eyePosition Position of the eye.
    /// @param eyeDirection Forward (pointing) direction of the eye.
    /// @param upDirection Coordinate system to follow (+X, +Y, +Z by default - { 0.0f, 1.0f, 0.0f }).
    [[nodiscard]] inline Matrix4x4 CreateViewMatrix(const Vector3 eyePosition, const Vector3 eyeDirection, const Vector3 upDirection = { 0.0f, 1.0f, 0.0f })
    {
        return glm::lookAtRH(eyePosition, eyePosition + glm::normalize(eyeDirection), upDirection);
    }

    /// @brief Creates a 4x4 projection matrix.
    /// @param FOV Field of view angle for camera.
    /// @param aspect Aspect ratio to follow.
    /// @param nearClip Near clipping plane distance.
    /// @param farClip Far clipping plane.
    [[nodiscard]] inline Matrix4x4 CreateProjectionMatrix(const float FOV, const float aspect, const float nearClip, const float farClip)
    {
        return glm::perspectiveRH(glm::radians(FOV), aspect, nearClip, farClip);
    }

    /// @brief Creates a 4x4 orthographic projection matrix.
    /// @param dimensions Dimensions of the matrix's view.
    /// @param nearClip Near clipping plane distance.
    /// @param farClip Far clipping plane.
    [[nodiscard]] inline Matrix4x4 CreateOrthographicProjectionMatrix(const float dimensions, const float nearClip, const float farClip)
    {
        return glm::orthoRH(-dimensions, dimensions, -dimensions, dimensions, nearClip, farClip);
    }
}