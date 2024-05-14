//
// Created by Nikolay Kanchevski on 13.05.2024.
//

#pragma once

#pragma region API Definitions
    #if defined(SRE_USE_SHARED_LIBRARY)
        #if defined(_WIN32)
            #if defined(SRE_BUILD_SHARED_LIBRARY)
                #define SIERRA_ENGINE_API __declspec(dllexport)
            #else
                #define SIERRA_ENGINE_API __declspec(dllimport)
            #endif
        #elif defined(__GNUC__)
            #if defined(SRE_BUILD_SHARED_LIBRARY)
                #define SIERRA_ENGINE_API __attribute__((visibility("default")))
            #else
                #define SIERRA_ENGINE_API
            #endif
        #else
            #error "Cannot export symbols on this system!"
        #endif
    #else
        #define SIERRA_ENGINE_API
    #endif
#pragma endregion