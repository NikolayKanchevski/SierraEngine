//
// Created by Nikolay Kanchevski on 9.09.23.
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
#elif SR_PLATFORM_ANDROID
    #include <vulkan/vulkan_android.h>
#endif

#include "../RenderingResource.h"

namespace Sierra
{

    /* --- TYPE DEFINITIONS --- */
    using VkHandle32 = ullong;
    using VkHandle64 = void*;

    class SIERRA_API VulkanResource : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] RenderingBackendType GetBackendType() const noexcept override { return RenderingBackendType ::Vulkan; }

        /* --- COPY SEMANTICS --- */
        VulkanResource(const VulkanResource&) = delete;
        VulkanResource& operator=(const VulkanResource&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanResource(VulkanResource&&) = delete;
        VulkanResource& operator=(VulkanResource&&) = delete;

        /* --- DESTRUCTORS --- */
        ~VulkanResource() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        VulkanResource() noexcept = default;

        /* --- PROTECTED METHODS --- */
        void AddToPNextChain(void* mainStruct, void* newStruct) const noexcept
        {
            // We just cast them to any Vulkan structure, as they all have their pNext stored exactly 4 bytes within the struct
            VkBufferMemoryBarrier* mainStructAsVkStruct = reinterpret_cast<VkBufferMemoryBarrier*>(mainStruct);
            VkBufferMemoryBarrier* newStructAsVkStruct = reinterpret_cast<VkBufferMemoryBarrier*>(newStruct);
            newStructAsVkStruct->pNext = mainStructAsVkStruct->pNext;
            mainStructAsVkStruct->pNext = newStruct;
        }

    };

}
