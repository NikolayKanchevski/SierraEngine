//
// Created by Nikolay Kanchevski on 30.01.23.
//

#pragma once

// === Numeric === //
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint32_t uint;
typedef uint64_t uint64;
typedef uint64_t ulong;
typedef size_t uSize;
typedef uSize Hash;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef ptrdiff_t iSize;

// === Mathematical === //
typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;
typedef glm::vec4 Vector4;
typedef glm::vec<2, int> Vector2Int;
typedef glm::vec<3, int> Vector3Int;
typedef glm::vec<4, int> Vector4Int;
typedef glm::quat Quaternion;
typedef glm::mat3x3 Matrix3x3;
typedef glm::mat4x4 Matrix4x4;
typedef glm::mat<3, 3, int> Matrix3x3Int;
typedef glm::mat<4, 4, int> Matrix4x4Int;

template<int C, int R>
using Matrix = glm::mat<C, R, float>;
template<int C, int R>
using MatrixInt = glm::mat<C, R, int>;

// === Pointers === //
template<class T>
using UniquePtr = std::unique_ptr<T>;
template<class T>
using SharedPtr = std::shared_ptr<T>;

// === Utilities === //
typedef std::string String;
typedef std::string_view StringView;
typedef std::function<void()> Callback;
template<class T>
using Optional =  std::optional<T>;
#define NO_CHANGE std::nullopt

// === Deprecated === //
// template<class T, size_t Size>
// using Array = std::array<T, Size>;
// template<class T>
// using Vector = std::vector<T>;
// template<class T, class K>
// using Map = std::map<T, K>;
// template<class T, class K>
// using UnorderedMap = std::unordered_map<T, K>;
// template<class T, class K>
// using Set = std::set<T, K>;
// template<class T, class K>
// using UnorderedSet = std::unordered_set<T, K>;