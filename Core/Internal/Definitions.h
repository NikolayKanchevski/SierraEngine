//
// Created by Nikolay Kanchevski on 12.02.23.
//

#pragma once

#define UINT_MAX std::numeric_limits<uint>::max()
#define UINT_MIN std::numeric_limits<uint>::min()

#define UINT64_MAX std::numeric_limits<uint64>::max()
#define UINT64_MIN std::numeric_limits<uint64>::min()

#define INT_MAX std::numeric_limits<int>::max()
#define INT_MIN std::numeric_limits<int>::min()

#define INT64_MAX std::numeric_limits<int64>::max()
#define INT64_MIN std::numeric_limits<int64>::min()

#define FLOAT_MAX std::numeric_limits<float>::max()
#define FLOAT_MIN std::numeric_limits<float>::min()

#define DOUBLE_MAX std::numeric_limits<double>::max()
#define DOUBLE_MIN std::numeric_limits<double>::min()

#define UINT_SIZE sizeof(uint)
#define UINT64_SIZE sizeof(uint64)

#define INT_SIZE sizeof(int)
#define INT64_SIZE sizeof(int64)

#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)

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