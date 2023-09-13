//
// Created by Nikolay Kanchevski on 8.12.2023.
//

#pragma once

#pragma region API Definitions
    // Only export symbols in debug, as the engine will be built as a shared library
    #if defined(SIERRA_USE_SHARED_LIBRARY)
        #if defined(_WIN32)
            #if defined(SIERRA_BUILD_SHARED_LIBRARY)
                #define SIERRA_API __declspec(dllexport)
            #else
                #define SIERRA_API __declspec(dllimport)
            #endif
        #elif defined(__GNUC__)
            #if defined(SIERRA_BUILD_SHARED_LIBRARY)
                #define SIERRA_API __attribute__((visibility("default")))
            #else
                #define SIERRA_API
            #endif
        #else
            #error "Cannot export symbols for current system!"
        #endif
    #else
        #define SIERRA_API
    #endif
#pragma endregion