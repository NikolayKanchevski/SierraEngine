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
        [[nodiscard]] inline ImageSampling GetSampling() const override { return sampling; }
        [[nodiscard]] inline const MTL::Texture* GetMetalTexture() const { return texture; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

        /* --- CONVERSIONS --- */
        static MTL::TextureType ImageSettingsToTextureType(ImageSampling sampling, uint32 layerCount);
        static MTL::PixelFormat ImageFormatToPixelFormat(ImageFormat format);
        static MTL::TextureUsage ImageUsageToTextureUsage(ImageUsage usage);
        static NS::UInteger ImageSamplingToUInteger(ImageSampling sampling);
        static MTL::StorageMode ImageMemoryLocationToStorageMode(ImageMemoryLocation memoryLocation);
        static MTL::CPUCacheMode ImageMemoryLocationToCPUCacheMode(ImageMemoryLocation memoryLocation);

    private:
        MTL::Texture* texture = nullptr;
        ImageSampling sampling = ImageSampling::x1;

    };

}
