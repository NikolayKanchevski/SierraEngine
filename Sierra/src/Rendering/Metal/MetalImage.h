//
// Created by Nikolay Kanchevski on 7.12.23.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLTexture = void;
        using MTLTextureType = ulong;
        using MTLPixelFormat = ulong;
        using MTLTextureUsage = ulong;
        using NSUInteger = ulong;
        using MTLStorageMode = ulong;
        using MTLCPUCacheMode = ulong;
    }
#endif

#include "../Image.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API MTLTextureType ImageSettingsToTextureType(ImageType type, uint32 layerCount, ImageSampling sampling) noexcept;
    [[nodiscard]] SIERRA_API MTLPixelFormat ImageFormatToPixelFormat(ImageFormat format) noexcept;
    [[nodiscard]] SIERRA_API MTLTextureUsage ImageUsageToTextureUsage(ImageUsage usage) noexcept;
    [[nodiscard]] SIERRA_API NSUInteger ImageSamplingToUInteger(ImageSampling sampling) noexcept;
    [[nodiscard]] SIERRA_API MTLTextureSwizzleChannels ImageComponentSwizzlesToTextureSwizzleChannels(ImageComponentSwizzle redSwizzle, ImageComponentSwizzle greenSwizzle, ImageComponentSwizzle blueSwizzle, ImageComponentSwizzle alphaSwizzle) noexcept;
    [[nodiscard]] SIERRA_API MTLStorageMode ImageMemoryLocationToStorageMode(ImageMemoryLocation memoryLocation) noexcept;
    [[nodiscard]] SIERRA_API MTLCPUCacheMode ImageMemoryLocationToCPUCacheMode(ImageMemoryLocation memoryLocation) noexcept;

    class SIERRA_API MetalImage final : public Image, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalImage(const MetalDevice& device, const ImageCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override;

        [[nodiscard]] uint32 GetWidth() const noexcept override { return width; }
        [[nodiscard]] uint32 GetHeight() const noexcept override { return height; }
        [[nodiscard]] uint32 GetDepth() const noexcept override { return depth; }
        [[nodiscard]] ImageFormat GetFormat() const noexcept override { return format; }

        [[nodiscard]] uint32 GetLevelCount() const noexcept override { return levelCount; }
        [[nodiscard]] uint32 GetLayerCount() const noexcept override { return layerCount; }
        [[nodiscard]] ImageSampling GetSampling() const noexcept override { return sampling; }

        [[nodiscard]] id<MTLTexture> GetMetalTexture() const noexcept { return texture; }

        /* --- COPY SEMANTICS --- */
        MetalImage(const MetalImage&) = delete;
        MetalImage& operator=(const MetalImage&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalImage(MetalImage&&) = delete;
        MetalImage& operator=(MetalImage&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalImage() noexcept override;

    private:
        const uint32 width = 0;
        const uint32 height = 0;
        const uint32 depth = 0;
        const ImageFormat format = ImageFormat::Undefined;

        const uint32 levelCount = 1;
        const uint32 layerCount = 1;
        const ImageSampling sampling = ImageSampling::x1;

        id<MTLTexture> texture = nil;
        bool swapchainImage = false;

        #if !defined(__OBJC__)
            using MTLPixelFormat = ulong;
        #endif

        friend class MetalSwapchain;
        struct SwapchainImageCreateInfo
        {
            std::string_view name = "Swapchain Image";
            id<MTLTexture> texture = nil;

            uint32 width = 0;
            uint32 height = 0;
            MTLPixelFormat format = MTLPixelFormatInvalid;
        };

        MetalImage(const MetalDevice& device, const SwapchainImageCreateInfo& createInfo);

    };

}
