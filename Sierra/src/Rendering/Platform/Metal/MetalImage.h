//
// Created by Nikolay Kanchevski on 7.12.23.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLTexture = void;

        using MTLTextureType = std::uintptr_t;
        using MTLPixelFormat = std::uintptr_t;
        using MTLTextureUsage = std::uintptr_t;
        using NSUInteger = std::uintptr_t;
        using MTLStorageMode = std::uintptr_t;
        using MTLCPUCacheMode = std::uintptr_t;
    }
#endif

#include "../../Image.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalImage final : public Image, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalImage(const MetalDevice &device, const ImageCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override;

        [[nodiscard]] uint32 GetWidth() const override { return width; }
        [[nodiscard]] uint32 GetHeight() const override { return height; }
        [[nodiscard]] uint32 GetDepth() const override { return depth; }
        [[nodiscard]] ImageFormat GetFormat() const override { return format; }

        [[nodiscard]] uint32 GetLevelCount() const override { return levelCount; }
        [[nodiscard]] uint32 GetLayerCount() const override { return layerCount; }
        [[nodiscard]] ImageSampling GetSampling() const override { return sampling; }

        [[nodiscard]] id<MTLTexture> GetMetalTexture() const { return texture; }

        /* --- DESTRUCTOR --- */
        ~MetalImage() override;

        /* --- CONVERSIONS --- */
        static MTLTextureType ImageSettingsToTextureType(ImageType type, uint32 layerCount, ImageSampling sampling);
        static MTLPixelFormat ImageFormatToPixelFormat(ImageFormat format);
        static MTLTextureUsage ImageUsageToTextureUsage(ImageUsage usage);
        static NSUInteger ImageSamplingToUInteger(ImageSampling sampling);
        static MTLStorageMode ImageMemoryLocationToStorageMode(ImageMemoryLocation memoryLocation);
        static MTLCPUCacheMode ImageMemoryLocationToCPUCacheMode(ImageMemoryLocation memoryLocation);

    private:
        uint32 width = 0;
        uint32 height = 0;
        uint32 depth = 0;
        ImageFormat format = ImageFormat::Undefined;

        uint32 levelCount = 1;
        uint32 layerCount = 1;
        ImageSampling sampling = ImageSampling::x1;

        id<MTLTexture> texture = nil;
        #if !defined(__OBJC__)
            using MTLPixelFormat = std::uintptr_t;
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

        bool swapchainImage = false;
        MetalImage(const MetalDevice &device, const SwapchainImageCreateInfo &createInfo);
        [[nodiscard]] static ImageFormat SwapchainPixelFormatToImageFormat(MTLPixelFormat format);

    };

}
