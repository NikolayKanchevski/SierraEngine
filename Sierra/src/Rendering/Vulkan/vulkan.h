//
// Created by nikolay on 9/28/26.
//

#pragma once


#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#if SR_PLATFORM_WINDOWS
    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <vulkan/vulkan_win32.h>
#elif SR_PLATFORM_APPLE
    #if SR_PLATFORM_macOS
        #include <vulkan/vulkan_macos.h>
    #elif SR_PLATFORM_iOS
        #include <vulkan/vulkan_ios.h>
    #endif

    #include <vulkan/vulkan_metal.h>
    #define VK_KHR_PORTABILITY_SUBSET_SPEC_VERSION 1
    #define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"

    #define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR static_cast<VkStructureType>(1000163000)
    struct VkPhysicalDevicePortabilitySubsetFeaturesKHR
    {
        VkStructureType    sType;
        void*              pNext;
        VkBool32           constantAlphaColorBlendFactors;
        VkBool32           events;
        VkBool32           imageViewFormatReinterpretation;
        VkBool32           imageViewFormatSwizzle;
        VkBool32           imageView2DOn3DImage;
        VkBool32           multisampleArrayImage;
        VkBool32           mutableComparisonSamplers;
        VkBool32           pointPolygons;
        VkBool32           samplerMipLodBias;
        VkBool32           separateStencilMaskRef;
        VkBool32           shaderSampleRateInterpolationFunctions;
        VkBool32           tessellationIsolines;
        VkBool32           tessellationPointMode;
        VkBool32           triangleFans;
        VkBool32           vertexAttributeAccessBeyondStride;
    };
#elif SR_PLATFORM_LINUX
    #include <X11/Xlib.h>
    #include <vulkan/vulkan_xlib.h>

    #if defined(None)
        #undef None
    #endif

    #include <vulkan/vulkan_wayland.h>
#elif SR_PLATFORM_ANDROID
    #include <vulkan/vulkan_android.h>
#endif
