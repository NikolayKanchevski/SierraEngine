//
// Created by Nikolay Kanchevski on 9.10.24.
//

#pragma once

#define GLM_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/integer.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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

using uint = unsigned int;
using uchar = unsigned char;
using ulong = unsigned long;
using llong = long long;
using ullong = unsigned long long;
using ldouble = long double;
using size = size_t;

using Vector2 = glm::vec<2, float32>;
using Vector3 = glm::vec<3, float32>;
using Vector4 = glm::vec<4, float32>;
using Vector2Int = glm::vec<2, int32>;
using Vector3Int = glm::vec<3, int32>;
using Vector4Int = glm::vec<4, int32>;
using Vector2UInt = glm::vec<2, uint32>;
using Vector3UInt = glm::vec<3, uint32>;
using Vector4UInt = glm::vec<4, uint32>;
using Quaternion = glm::quat;
using Matrix3x3 = glm::mat<3, 3, float32>;
using Matrix4x4 = glm::mat<4, 4, float32>;
using Matrix3x3Int = glm::mat<3, 3, int32>;
using Matrix4x4Int = glm::mat<4, 4, int32>;
using Color32 = glm::vec<4, float32>;
using Color64 = glm::vec<4, float64>;

template<uint32 C, uint32 R>
using Matrix = glm::mat<C, R, float32>;
template<uint32 C, uint32 R>
using MatrixInt = glm::mat<C, R, int32>;
template<uint32 C, uint32 R>
using MatrixUInt = glm::mat<C, R, uint32>;