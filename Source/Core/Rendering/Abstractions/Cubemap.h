//
// Created by Nikolay Kanchevski on 29.12.22.
//

#pragma once

#include "Texture.h"

namespace Sierra::Rendering
{
    struct BinaryCubemapCreateInfo
    {
        void* data[6];
        uint32 width;
        uint32 height;

        ImageChannels channels = ImageChannels::RGBA;
        ImageMemoryType memoryType = ImageMemoryType::UINT8_NORM;
        
        bool enableSmoothFiltering = true;
    };

    struct CubemapCreateInfo
    {
        String filePaths[6];
        ImageMemoryType memoryType = ImageMemoryType::UINT8_NORM;

        bool enableSmoothFiltering = true;
    };

    class Cubemap
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Cubemap(const BinaryCubemapCreateInfo &createInfo);
        static SharedPtr<Cubemap> Load(const BinaryCubemapCreateInfo &createInfo);
        static SharedPtr<Cubemap> Create(const CubemapCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetWidth() const { return image->GetWidth(); }
        [[nodiscard]] inline uint32 GetHeight() const { return image->GetHeight(); }

        [[nodiscard]] inline UniquePtr<Image>& GetImage() { return image; }
        [[nodiscard]] inline UniquePtr<Sampler>& GetSampler() { return sampler; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Cubemap);

    private:
        UniquePtr<Image> image;
        UniquePtr<Sampler> sampler;

    };

}
