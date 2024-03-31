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
        #define SR_PLATFORM_APPLE (SR_PLATFORM_macOS || SR_PLATFORM_iOS)
        #if defined(__ANDROID__)
            #define SR_PLATFORM_ANDROID __ANDROID__ // This is necessary due to a bug in Android's precompiled headers
        #else
            #define SR_PLATFORM_ANDROID 0
        #endif

        #define SR_PLATFORM_DESKTOP (SR_PLATFORM_WINDOWS || SR_PLATFORM_LINUX || SR_PLATFORM_macOS)
        #define SR_PLATFORM_MOBILE (SR_PLATFORM_iOS || SR_PLATFORM_ANDROID)
        #define SR_PLATFORM_EMULATOR 0

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
        #include <array>
        #include <atomic>
        #include <bitset>
        #include <chrono>
        #include <deque>
        #include <execution>
        #include <filesystem>
        #include <fstream>
        #include <functional>
        #include <future>
        #include <iostream>
        #if SR_PLATFORM_WINDOWS
            #define NOMINMAX
        #endif
        #include <limits>
        #include <list>
        #include <memory>
        #include <mutex>
        #include <optional>
        #include <queue>
        #include <random>
        #include <ratio>
        #include <regex>
        #include <span>
        #include <sstream>
        #include <string>
        #include <string_view>
        #include <thread>
        #include <type_traits>
        #include <unordered_map>
        #include <unordered_set>
        #include <vector>
    #pragma endregion

    #pragma region External Libraries
        /* --- GLM --- */
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
    #pragma endregion

    #pragma region Type Definitions
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
        using ColorRGBA8 = glm::vec<4, uint8>;
        using ColorRGBA32 = glm::vec<4, float32>;

	    template<uint32 C, uint32 R>
	    using Matrix = glm::mat<C, R, float32>;
	    template<uint32 C, uint32 R>
	    using MatrixInt = glm::mat<C, R, int32>;
	    template<uint32 C, uint32 R>
	    using MatrixUInt = glm::mat<C, R, uint32>;
    #pragma endregion

    #pragma region Macros
        #define SR_DEFINE_ENUM_FLAG_OPERATORS(T)                                                                                                                                                \
            inline constexpr T operator~ (const T a) { return static_cast<T>(~static_cast<std::underlying_type_t<T>>(a)); }                                                                     \
            inline constexpr T operator| (const T a, const T b) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) | static_cast<std::underlying_type_t<T>>(b)); }               \
            inline constexpr std::underlying_type_t<T> operator& (const T a, const T b) { return static_cast<std::underlying_type_t<T>>(a) & static_cast<std::underlying_type_t<T>>(b); }       \
            inline constexpr T operator^ (const T a, const T b) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(a) ^ static_cast<std::underlying_type_t<T>>(b)); }               \
            inline T& operator|= (T &a, const T b) { return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type_t<T>&>(a) |= static_cast<std::underlying_type_t<T>>(b)); }               \
            inline T& operator&= (T &a, const T b) { return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type_t<T>&>(a) &= static_cast<std::underlying_type_t<T>>(b)); }               \
            inline T& operator^= (T &a, const T b) { return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type_t<T>&>(a) ^= static_cast<std::underlying_type_t<T>>(b)); }
    #pragma endregion

    #pragma region Source Files
        #include "Core/Logger.h"
        #include "Core/ScopeProfiler.h"
    #pragma endregion
#endif