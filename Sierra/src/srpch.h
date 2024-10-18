//
// Created by Nikolay Kanchevski on 08.11.23.
//

#pragma once

#if defined(__cplusplus)
    #pragma region Platform Detection
        #define SR_PLATFORM_WINDOWS 0
        #define SR_PLATFORM_LINUX 0
        #define SR_PLATFORM_macOS 0
        #define SR_PLATFORM_ANDROID 0
        #define SR_PLATFORM_iOS 0
        #define SR_PLATFORM_EMULATOR 0

        #if defined(_WIN32) || defined(_WIN64)
            #undef SR_PLATFORM_WINDOWS
            #define SR_PLATFORM_WINDOWS 1
        #elif defined(__APPLE__) || defined(__MACH__)
            #include "TargetConditionals.h"
            #if TARGET_OS_IOS
                #undef SR_PLATFORM_iOS
                #define SR_PLATFORM_iOS 1
            #elif TARGET_OS_OSX
                #undef SR_PLATFORM_macOS
                #define SR_PLATFORM_macOS 1
            #endif
            #if TARGET_OS_SIMULATOR
                #undef SR_PLATFORM_EMULATOR
                #define SR_PLATFORM_EMULATOR 1
            #endif
        #elif defined(__ANDROID__)
            #undef SR_PLATFORM_ANDROID
            #define SR_PLATFORM_ANDROID 1
        #elif defined(__linux__)
            #undef SR_PLATFORM_LINUX
            #define SR_PLATFORM_LINUX 1
        #endif
    #pragma endregion

    #pragma region Standard Library
        #include <algorithm>
        #include <array>
        #include <atomic>
        #define _LIBCPP_HAS_NO_LOCALIZATION // For some reason <chrono> imports <format>, which is only supported as of iOS 16.3, so we use fmt instead
            #include <chrono>
        #undef _LIBCPP_HAS_NO_LOCALIZATION
        #include <concepts>
        #include <deque>
        #include <exception>
        #include <filesystem>
        #include <fstream>
        #include <functional>
        #include <future>
        #if SR_PLATFORM_WINDOWS
            #define NOMINMAX
        #endif
        #include <limits>
        #include <memory>
        #include <mutex>
        #include <optional>
        #include <queue>
        #include <random>
        #include <regex>
        #include <span>
        #include <string>
        #include <string_view>
        #include <thread>
        #include <type_traits>
        #include <unordered_map>
        #include <vector>
    #pragma endregion

    #pragma region External Libraries
        #include <fmt/format.h>
    #pragma endregion

    #pragma region Macros
        #define SR_VERSION_MAJOR 1
        #define SR_VERSION_MINOR 0
        #define SR_VERSION_PATCH 0

        #define SR_PLATFORM_DESKTOP (SR_PLATFORM_WINDOWS || SR_PLATFORM_LINUX || SR_PLATFORM_macOS)
        #define SR_PLATFORM_MOBILE (SR_PLATFORM_iOS || SR_PLATFORM_ANDROID)
        #define SR_PLATFORM_APPLE (SR_PLATFORM_macOS || SR_PLATFORM_iOS)

        #if !defined(SR_METAL_SUPPORTED)
            #define SR_METAL_SUPPORTED 0
        #endif
        #if !defined(SR_VULKAN_SUPPORTED)
            #define SR_VULKAN_SUPPORTED 0
        #endif
        #if !defined(SR_DIRECTX_SUPPORTED)
            #define SR_DIRECTX_SUPPORTED 0
        #endif
        #if !defined(SR_OPENGL_SUPPORTED)
            #define SR_OPENGL_SUPPORTED 0
        #endif
        #if !defined(SR_WEBGPU_SUPPORTED)
            #define SR_WEBGPU_SUPPORTED 0
        #endif

        #define SR_FORMAT(...) ::fmt::format(__VA_ARGS__)
        #define SR_DEFINE_ENUM_FLAG_OPERATORS(T)                                                                                                                                                         \
            inline constexpr std::underlying_type_t<T> operator&(const T left, const T right) { return static_cast<std::underlying_type_t<T>>(left) & static_cast<std::underlying_type_t<T>>(right); }   \
            inline constexpr bool operator<(const T left, const T right) { return static_cast<std::underlying_type_t<T>>(left) < static_cast<std::underlying_type_t<T>>(right); }   \
            inline constexpr bool operator>(const T left, const T right) { return static_cast<std::underlying_type_t<T>>(left) > static_cast<std::underlying_type_t<T>>(right); }   \
            inline constexpr bool operator>=(const T left, const T right) { return static_cast<std::underlying_type_t<T>>(left) >= static_cast<std::underlying_type_t<T>>(right); } \
            inline constexpr bool operator<=(const T left, const T right) { return static_cast<std::underlying_type_t<T>>(left) <= static_cast<std::underlying_type_t<T>>(right); } \
            inline constexpr T operator~(const T self) { return static_cast<T>(~static_cast<std::underlying_type_t<T>>(self)); }                                                                         \
            inline constexpr T operator^(const T left, const T right) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(left) ^ static_cast<std::underlying_type_t<T>>(right)); }           \
            inline constexpr T operator|(const T left, const T right) { return static_cast<T>(static_cast<std::underlying_type_t<T>>(left) | static_cast<std::underlying_type_t<T>>(right)); }           \
            inline T& operator|=(T& left, const T right) { return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type_t<T>&>(left) |= static_cast<std::underlying_type_t<T>>(right)); }           \
            inline T& operator&=(T& left, const T right) { return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type_t<T>&>(left) &= static_cast<std::underlying_type_t<T>>(right)); }           \
            inline T& operator^=(T& left, const T right) { return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type_t<T>&>(left) ^= static_cast<std::underlying_type_t<T>>(right)); }

        #if SR_ENABLE_LOGGING
            #if defined(SR_LIBRARY_IMPLEMENTATION)
                #define SR_INFO(...) ::Sierra::Logger::GetCoreLogger().info(__VA_ARGS__)
                #define SR_WARNING(...) ::Sierra::Logger::GetCoreLogger().warn(__VA_ARGS__)
                #define SR_ERROR(...) ::Sierra::Logger::GetCoreLogger().error(__VA_ARGS__)
                #define SR_THROW(EXCEPTION) throw EXCEPTION

                #define SR_INFO_IF(EXPRESSION, ...) if (EXPRESSION) SR_INFO(__VA_ARGS__)
                #define SR_WARNING_IF(EXPRESSION, ...) if (EXPRESSION) SR_WARNING(__VA_ARGS__)
                #define SR_ERROR_IF(EXPRESSION, ...) if (EXPRESSION) SR_ERROR(__VA_ARGS__)
                #define SR_THROW_IF(EXPRESSION, EXCEPTION) if (EXPRESSION) SR_THROW(EXCEPTION)
            #else
                #define APP_INFO(...) ::Sierra::Logger::GetApplicationLogger().info(__VA_ARGS__)
                #define APP_WARNING(...) ::Sierra::Logger::GetApplicationLogger().warn(__VA_ARGS__)
                #define APP_ERROR(...) ::Sierra::Logger::GetApplicationLogger().error(__VA_ARGS__)
                #define APP_THROW(EXCEPTION) throw EXCEPTION

                #define APP_INFO_IF(EXPRESSION, ...) if (EXPRESSION) APP_INFO(__VA_ARGS__)
                #define APP_WARNING_IF(EXPRESSION, ...) if (EXPRESSION) APP_WARNING(__VA_ARGS__)
                #define APP_ERROR_IF(EXPRESSION, ...) if (EXPRESSION) APP_ERROR(__VA_ARGS__)
                #define APP_THROW_IF(EXPRESSION, EXCEPTION) if (EXPRESSION) APP_THROW(EXCEPTION)
            #endif

            #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
                #define SR_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
            #elif defined(__DMC__) && (__DMC__ >= 0x810)
            #define SR_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
            #elif (defined(__FUNCSIG__) || (_MSC_VER))
                #define SR_FUNCTION_SIGNATURE __FUNCSIG__
            #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
                #define SR_FUNCTION_SIGNATURE __FUNCTION__
            #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
                #define SR_FUNCTION_SIGNATURE __FUNC__
            #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
                #define SR_FUNCTION_SIGNATURE __func__
            #elif defined(__cplusplus) && (__cplusplus >= 201103)
                #define SR_FUNCTION_SIGNATURE __func__
            #else
                #define SR_FUNCTION_SIGNATURE "Unknown function signature!"
            #endif

            #define PROFILE_SCOPE() const ::Sierra::ScopeProfiler __PROFILER__LINE_##__LINE__(SR_FUNCTION_SIGNATURE)
        #else
            #if defined(SR_LIBRARY_IMPLEMENTATION)
                #define SR_INFO(...)
                #define SR_WARNING(...)
                #define SR_ERROR(...)
                #define SR_THROW(EXCEPTION)

                #define SR_INFO_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
                #define SR_WARNING_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
                #define SR_ERROR_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
                #define SR_THROW_IF(EXPRESSION, EXCEPTION) static_cast<void>(EXPRESSION)
            #else
                #define APP_INFO(...)
                #define APP_WARNING(...)
                #define APP_ERROR(...)
                #define APP_THROW(EXCEPTION)

                #define APP_INFO_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
                #define APP_WARNING_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
                #define APP_ERROR_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
                #define APP_THROW_IF(EXPRESSION, EXCEPTION) static_cast<void>(EXPRESSION)
            #endif
        #endif

        #pragma region Source Files
            #include "Core/API.h"
            #include "Core/Types.h"
            #include "Core/Logging.h"
            #include "Core/Errors.h"
        #pragma endregion
    #pragma endregion
#endif
