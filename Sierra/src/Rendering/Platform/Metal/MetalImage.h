//
// Created by Nikolay Kanchevski on 7.12.23.
//

#pragma once

#include "../../Image.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalImage : public Image, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalImage(const MetalDevice &device, const ImageCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const MTL::Texture* GetMetalTexture() const { return texture; }

        /* --- DESTRUCTOR --- */
        ~MetalImage() override;

        /* --- CONVERSIONS --- */
        static MTL::TextureType ImageSettingsToTextureType(ImageSampling sampling, uint32 layerCount);
        static MTL::PixelFormat ImageFormatToPixelFormat(ImageFormat format);
        static MTL::TextureUsage ImageUsageToTextureUsage(ImageUsage usage);
        static NS::UInteger ImageSamplingToUInteger(ImageSampling sampling);
        static MTL::StorageMode ImageMemoryLocationToStorageMode(ImageMemoryLocation memoryLocation);
        static MTL::CPUCacheMode ImageMemoryLocationToCPUCacheMode(ImageMemoryLocation memoryLocation);

    private:
        MTL::Texture* texture = nullptr;

        friend class MetalSwapchain;
        struct SwapchainImageCreateInfo
        {
            const std::string &name = "Swapchain Image";
            MTL::Texture* texture = nullptr;

            uint32 width = 0;
            uint32 height = 0;
            MTL::PixelFormat format = MTL::PixelFormatInvalid;
        };

        bool swapchainImage = false;
        MetalImage(const MetalDevice &device, const SwapchainImageCreateInfo &createInfo);
        [[nodiscard]] static ImageFormat SwapchainPixelFormatToImageFormat(MTL::PixelFormat format);

    };

}
