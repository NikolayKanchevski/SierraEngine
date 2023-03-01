//
// Created by Nikolay Kanchevski on 12.02.23.
//

#pragma once

struct NullType { };

#ifdef UINT8_MAX
#undef UINT8_MAX
#endif

#ifdef UINT8_MIN
#undef UINT8_MIN
#endif

#ifdef UINT16_MAX
#undef UINT16_MAX
#endif

#ifdef UINT16_MIN
#undef UINT16_MIN
#endif

#ifdef UINT_MAX
#undef UINT_MAX
#endif

#ifdef UINT_MIN
#undef UINT_MIN
#endif

#ifdef UINT32_MAX
#undef UINT32_MAX
#endif

#ifdef UINT32_MIN
#undef UINT32_MIN
#endif

#ifdef UINT64_MAX
#undef UINT64_MAX
#endif

#ifdef UINT64_MIN
#undef UINT64_MIN
#endif

#ifdef INT8_MAX
#undef INT8_MAX
#endif

#ifdef INT8_MIN
#undef INT8_MIN
#endif

#ifdef INT16_MAX
#undef INT16_MAX
#endif

#ifdef INT16_MIN
#undef INT16_MIN
#endif

#ifdef INT_MAX
#undef INT_MAX
#endif

#ifdef INT_MIN
#undef INT_MIN
#endif

#ifdef INT32_MAX
#undef INT32_MAX
#endif

#ifdef INT32_MIN
#undef INT32_MIN
#endif

#ifdef INT64_MAX
#undef INT64_MAX
#endif

#ifdef INT64_MIN
#undef INT64_MIN
#endif

#ifdef FLOAT_MAX
#undef FLOAT_MAX
#endif

#ifdef FLOAT_MIN
#undef FLOAT_MIN
#endif

#ifdef DOUBLE_MAX
#undef DOUBLE_MAX
#endif

#ifdef DOUBLE_MIN
#undef DOUBLE_MIN
#endif

#define UINT8_MAX 255U
#define UINT8_MIN 0U

#define UINT16_MAX 65535U
#define UINT16_MIN 0U

#define UINT_MAX 4294967295U
#define UINT_MIN 0U

#define UINT32_MAX UINT_MAX
#define UINT32_MIN UINT_MIN

#define UINT64_MAX 18446744073709551615ULL
#define UINT64_MIN 0U

#define INT8_MAX 127
#define INT8_MIN -128

#define INT16_MAX 32767
#define INT16_MIN -32768

#define INT_MAX 2147483647
#define INT_MIN -2147483648

#define INT32_MAX INT_MAX
#define INT32_MIN INT_MIN

#define INT64_MAX 9223372036854775807LL
#define INT64_MIN -9223372036854775808LL

#define FLOAT_MAX 3.40282347e+38F
#define FLOAT_MIN 1.17549435e-38F

#define DOUBLE_MAX 1.7976931348623157e+308
#define DOUBLE_MIN 2.2250738585072014e-308

#define UINT8_SIZE 1
#define UINT16_SIZE 2
#define UINT_SIZE 4
#define UINT32_SIZE UINT_SIZE
#define UINT64_SIZE 8

#define INT8_SIZE 1
#define INT16_SIZE 2
#define INT_SIZE 4
#define INT32_SIZE INT_SIZE
#define INT64_SIZE 8

#define FLOAT_SIZE 4
#define DOUBLE_SIZE 8

#define ARRAY_SIZE(array) ((uSize)(sizeof(array) / sizeof(*(array))))
#define MODIFY_CONST(type, name, value) \
    type *name##Pointer;                \
    name##Pointer = (type*)(&name);     \
    *name##Pointer = value;

#define PLATFORM_WINDOWS 0
#define PLATFORM_LINUX 0
#define PLATFORM_APPLE 0
#define PLATFORM_MACOS 0
#define PLATFORM_MOBILE 0
#define PLATFORM_iOS 0
#define PLATFORM_ANDROID 0

#if _WIN32
    #undef PLATFORM_WINDOWS
    #define PLATFORM_WINDOWS 1
#elif __APPLE__
    #if TARGET_OS_IPHONE && TARGET_OS_MACCATALYST
        #undef PLATFORM_MACOS
        #define PLATFORM_MACOS 1
        #undef PLATFORM_APPLE
        #define PLATFORM_APPLE 1
    #elif TARGET_OS_IPHONE
        #undef PLATFORM_iOS
        #define PLATFORM_iOS 1
        #undef PLATFORM_APPLE
        #define PLATFORM_APPLE 1
        #undef PLATFORM_MOBILE
        #define PLATFORM_MOBILE 1
    #else
        #undef PLATFORM_MACOS
        #define PLATFORM_MACOS 1
        #undef PLATFORM_APPLE
        #define PLATFORM_APPLE 1
    #endif
#elif __linux
    #undef PLATFORM_LINUX
    #define PLATFORM_LINUX 1
#elif __ANDROID__
    #undef PLATFORM_ANDROID
    #define PLATFORM_ANDROID 1
    #undef PLATFORM_MOBILE
    #define PLATFORM_MOBILE 1
#endif