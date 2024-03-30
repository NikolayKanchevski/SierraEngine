//
// Created by Nikolay Kanchevski on 7.12.23.
//

#pragma once

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
        [[nodiscard]] inline const id<MTLTexture> GetMetalTexture() const { return texture; }

        /* --- DESTRUCTOR --- */
        ~MetalImage() override;

        /* --- CONVERSIONS --- */
        static MTLTextureType ImageSettingsToTextureType(ImageSampling sampling, uint32 layerCount);
        static MTLPixelFormat ImageFormatToPixelFormat(ImageFormat format);
        static MTLTextureUsage ImageUsageToTextureUsage(ImageUsage usage);
        static NSUInteger ImageSamplingToUInteger(ImageSampling sampling);
        static MTLStorageMode ImageMemoryLocationToStorageMode(ImageMemoryLocation memoryLocation);
        static MTLCPUCacheMode ImageMemoryLocationToCPUCacheMode(ImageMemoryLocation memoryLocation);

    private:
        id<MTLTexture> texture = nil;

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
