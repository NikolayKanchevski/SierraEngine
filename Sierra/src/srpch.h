//
// Created by Nikolay Kanchevski on 08.11.23.
//

#pragma once

#if defined(__cplusplus)
    #pragma region Platform Detection
        #define SR_PLATFORM_WINDOWS 0
        #define SR_PLATFORM_LINUX 0
        #define SR_PLATFORM_macOS 0
        #define SR_PLATFORM_iOS 0
        #define SR_PLATFORM_EMULATOR 0
        #if defined(__ANDROID__)
            #define SR_PLATFORM_ANDROID __ANDROID__ // This is necessary due to a bug in Android's precompiled headers
        #else
            #define SR_PLATFORM_ANDROID 0
        #endif

        #define SR_PLATFORM_APPLE (SR_PLATFORM_macOS || SR_PLATFORM_iOS)
        #define SR_PLATFORM_MOBILE (SR_PLATFORM_iOS || SR_PLATFORM_ANDROID)

        #if _WIN32 || _WIN64
            #undef SR_PLATFORM_WINDOWS
            #define SR_PLATFORM_WINDOWS 1
        #elif __APPLE__ || __MACH__
            #include "TargetConditionals.h"
            #if TARGET_OS_IPHONE
                #undef SR_PLATFORM_iOS
                #define SR_PLATFORM_iOS 1
            #else
                #undef SR_PLATFORM_macOS
                #define SR_PLATFORM_macOS 1
            #endif
            #if TARGET_OS_SIMULATOR
                #undef SR_PLATFORM_EMULATOR
                #define SR_PLATFORM_EMULATOR 1
            #endif
        #elif __ANDROID__
            #undef PLATFORM_ANDROID
            #define PLATFORM_ANDROID 1
        #elif __linux__
            #undef SR_PLATFORM_LINUX
            #define SR_PLATFORM_LINUX 1
        #endif
    #pragma endregion

    #pragma region Symbol Definitions
        #include "Core/API.h"
    #pragma endregion

    #pragma region Standard Library
        #include <algorithm>
        #include <functional>
        #include <memory>
        #include <thread>
        #include <utility>
        #include <cstdint>
        #include <array>
        #include <vector>
        #include <unordered_map>
        #include <set>
        #include <unordered_set>
        #include <exception>
        #include <execution>
        #include <random>
        #include <regex>
        #include <chrono>
        #include <mutex>
        #include <shared_mutex>
        #include <future>
        #include <string>
        #include <cstring>
        #include <string_view>
        #include <sstream>
        #include <stack>
        #include <queue>
        #include <deque>
        #include <optional>
        #include <fstream>
        #include <filesystem>
        #include <bitset>
        #include <any>
        #include <cstdio>
        #if SR_PLATFORM_WINDOWS
            #define NOMINMAX
        #endif
        #include <limits>
    #pragma endregion

    #pragma region External Libraries
        /* --- GLM --- */
        #define GLM_DEPTH_ZERO_TO_ONE
        #include <glm/glm.hpp>
        #include <glm/gtc/type_ptr.hpp>
        #include <glm/gtx/quaternion.hpp>
        #include <glm/gtx/euler_angles.hpp>
        #include <glm/ext/matrix_transform.hpp>
        #include <glm/ext/matrix_clip_space.hpp>
        #include <glm/gtx/matrix_decompose.hpp>
    #pragma endregion

    #pragma region Type Definitions
        typedef int8_t int8;
        typedef int16_t int16;
        typedef int32_t int32;
        typedef int64_t int64;
	    typedef uint8_t uint8;
	    typedef uint16_t uint16;
	    typedef uint32_t uint32;
	    typedef uint64_t uint64;
        typedef float float32;
        typedef double float64;

        typedef unsigned int uint;
        typedef unsigned char uchar;
        typedef unsigned long ulong;
        typedef long long llong;
        typedef unsigned long long ullong;
        typedef long double ldouble;
        typedef size_t size;
        typedef size Hash;

	    typedef glm::vec<2, float32> Vector2;
	    typedef glm::vec<3, float32> Vector3;
	    typedef glm::vec<4, float32> Vector4;
	    typedef glm::vec<2, int32> Vector2Int;
	    typedef glm::vec<3, int32> Vector3Int;
	    typedef glm::vec<4, int32> Vector4Int;
	    typedef glm::vec<2, uint32> Vector2UInt;
	    typedef glm::vec<3, uint32> Vector3UInt;
	    typedef glm::vec<4, uint32> Vector4UInt;
	    typedef glm::quat Quaternion;
	    typedef glm::mat<3, 3, float32> Matrix3x3;
	    typedef glm::mat<4, 4, float32> Matrix4x4;
	    typedef glm::mat<3, 3, int32> Matrix3x3Int;
	    typedef glm::mat<4, 4, int32> Matrix4x4Int;

	    template<uint32 C, uint32 R>
	    using Matrix = glm::mat<C, R, float32>;
	    template<uint32 C, uint32 R>
	    using MatrixInt = glm::mat<C, R, int32>;
	    template<uint32 C, uint32 R>
	    using MatrixUInt = glm::mat<C, R, uint32>;
    #pragma endregion

    #pragma region Macros
        #define SR_DEFINE_ENUM_FLAG_OPERATORS(T)                                                                                                                                                      \
            inline constexpr T operator~ (const T a) { return static_cast<T>(~static_cast<std::underlying_type<T>::type>(a)); }                                                                       \
            inline constexpr T operator| (const T a, const T b) { return static_cast<T>(static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b)); }             \
            inline constexpr std::underlying_type<T>::type operator& (const T a, const T b) { return static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b); } \
            inline constexpr T operator^ (const T a, const T b) { return static_cast<T>(static_cast<std::underlying_type<T>::type>(a) ^ static_cast<std::underlying_type<T>::type>(b)); }             \
            inline T& operator|= (T& a, const T b) { return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type<T>::type&>(a) |= static_cast<std::underlying_type<T>::type>(b)); }             \
            inline T& operator&= (T& a, const T b) { return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type<T>::type&>(a) &= static_cast<std::underlying_type<T>::type>(b)); }             \
            inline T& operator^= (T& a, const T b) { return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type<T>::type&>(a) ^= static_cast<std::underlying_type<T>::type>(b)); }             
    #pragma endregion

    #pragma region Source Files
        #include "Core/Logger.h"
        #include "Core/ScopeProfiler.h"
    #pragma endregion
#endif