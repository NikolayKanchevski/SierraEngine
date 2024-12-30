//
// Created by Nikolay Kanchevski on 9.10.24.
//

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_RIGHT_HANDED
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SIZE_T_LENGTH
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/integer.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#pragma region Concepts
    template<typename T>
    concept EnumType = std::is_enum_v<T>;

    template<typename T>
    concept NumericType = std::is_arithmetic_v<T>;

    template<typename T>
    concept SignedType = std::is_signed_v<T>;

    template<typename T>
    concept UnsignedType = std::is_unsigned_v<T>;

    template<typename T>
    concept FloatingPointType = std::is_floating_point_v<T>;

    template<typename T>
    concept ClassType = std::is_class_v<T>;

    template<typename T>
    concept PointerType = std::is_pointer_v<T>;

    template<typename T>
    concept ReferenceType = std::is_reference_v<T>;
#pragma endregion

#pragma region Types
    using int8 = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;
    using uint8 = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;
    using float32 = float;
    using float64 = double;

    using uchar = unsigned char;
    using uint = unsigned int;
    using ulong = unsigned long;
    using llong = long long;
    using ullong = unsigned long long;
    using ldouble = long double;
    using size = size_t;

    template<size Length, NumericType Numeric>
    using VectorBase = glm::vec<Length, Numeric>;
    using Vector2 = VectorBase<2, float32>;
    using Vector3 = VectorBase<3, float32>;
    using Vector4 = VectorBase<4, float32>;
    using Vector2Int = VectorBase<2, int32>;
    using Vector3Int = VectorBase<3, int32>;
    using Vector4Int = VectorBase<4, int32>;
    using Vector2UInt = VectorBase<2, uint32>;
    using Vector3UInt = VectorBase<3, uint32>;
    using Vector4UInt = VectorBase<4, uint32>;

    template<FloatingPointType FloatingPoint>
    using QuaternionBase = glm::qua<FloatingPoint>;
    using Quaternion = QuaternionBase<float32>;

    template<size Columns, size Rows, NumericType Numeric>
    using MatrixBase = glm::mat<Columns, Rows, Numeric>;
    using Matrix3x3 = MatrixBase<3, 3, float32>;
    using Matrix4x4 = MatrixBase<4, 4, float32>;

    template<size Length, FloatingPointType FloatingPoint>
    using ColorBase = glm::vec<Length, FloatingPoint>;
    using Color32 = ColorBase<4, float32>;
    using Color64 = ColorBase<4, float64>;
#pragma endregion

#pragma region Concepts
    template<typename>
    struct IsVector : std::false_type { };

    template<size Length, NumericType Numeric>
    struct IsVector<VectorBase<Length, Numeric>> : std::true_type { };

    template<typename T>
    concept VectorType = IsVector<T>::value;

    template<typename>
    struct IsQuaternion : std::false_type { };

    template<FloatingPointType FloatingPoint>
    struct IsQuaternion<QuaternionBase<FloatingPoint>> : std::true_type { };

    template<typename T>
    concept QuaternionType = IsQuaternion<T>::value;

    template<typename>
    struct IsMatrix : std::false_type { };

    template<size Columns, size Rows, NumericType Numeric>
    struct IsMatrix<MatrixBase<Columns, Rows, Numeric>> : std::true_type { };

    template<typename T>
    concept MatrixType = IsMatrix<T>::value;

    template<typename>
    struct IsColor : std::false_type { };

    template<size Length, NumericType Numeric>
    struct IsColor<ColorBase<Length, Numeric>> : std::true_type { };

    template<typename T>
    concept ColorType = IsColor<T>::value;
#pragma endregion