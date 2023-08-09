//
// Created by Nikolay Kanchevski on 30.01.23.
//

#pragma once

#pragma region Numeric
    typedef uint8_t uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint32_t uint;
    typedef uint64_t uint64;
    typedef size_t uSize;
    typedef uSize Hash;

    typedef int8_t int8;
    typedef int16_t int16;
    typedef int32_t int32;
    typedef int64_t int64;
    typedef ptrdiff_t iSize;
#pragma endregion

#pragma region Mathematical
    typedef glm::vec2 Vector2;
    typedef glm::vec3 Vector3;
    typedef glm::vec4 Vector4;
    typedef glm::vec<2, int32> Vector2Int;
    typedef glm::vec<3, int32> Vector3Int;
    typedef glm::vec<4, int32> Vector4Int;
    typedef glm::quat Quaternion;
    typedef glm::mat3x3 Matrix3x3;
    typedef glm::mat4x4 Matrix4x4;
    typedef glm::mat<3, 3, int32> Matrix3x3Int;
    typedef glm::mat<4, 4, int32> Matrix4x4Int;

    template<int32 C, int32 R>
    using Matrix = glm::mat<C, R, float>;
    template<int32 C, int32 R>
    using MatrixInt = glm::mat<C, R, int32>;
#pragma endregion

#pragma region Smart Pointers
    template<class T>
    using UniquePtr = std::unique_ptr<T>;
    template<class T>
    using SharedPtr = std::shared_ptr<T>;
    template<class T>
    using ReferenceWrapper = std::reference_wrapper<T>;
#pragma endregion

#pragma region Utilities
    typedef std::string String;
    typedef std::string_view StringView;
    typedef std::function<void()> Callback;
    typedef std::filesystem::path FilePath;

    template<class T>
    using Optional =  std::optional<T>;
    template<class A, class B>
    using Tuple = std::tuple<A, B>;
    #define NO_CHANGE std::nullopt
#pragma endregion

#pragma region Deprecated
    // template<class T, uSize Size>
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
#pragma endregion