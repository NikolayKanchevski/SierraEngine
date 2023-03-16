//
// Created by Nikolay Kanchevski on 29.12.22.
//

#pragma once

#include "Texture.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    enum CubemapType
    {
        UNDEFINED_CUBEMAP = -1,
        SKYBOX = 0
    };

    struct CubemapCreateInfo
    {
        String filePaths[6];
        CubemapType cubemapType = CubemapType::UNDEFINED_CUBEMAP;

        ImageFormat imageFormat = ImageFormat::R8G8B8A8_SRGB;
        bool mipMappingEnabled = false;

        SamplerCreateInfo samplerCreateInfo{};
    };

    class Cubemap
    {
    public:
        /* --- CONSTRUCTORS --- */
        Cubemap(const CubemapCreateInfo &createInfo);
        static UniquePtr<Cubemap> Create(CubemapCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] UniquePtr<Image>& GetImage() { return image; }
        [[nodiscard]] UniquePtr<Sampler>& GetSampler() { return sampler; }
        [[nodiscard]] inline String GetFilePath(const uint index = 0) const { return filePaths[index]; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Cubemap);

    private:
        String filePaths[6];

        CubemapType cubemapType;
        int colorChannelsCount;

        int width;
        int height;

        UniquePtr<Image> image;
        bool mipMappingEnabled;

        uint64 memorySize;
        UniquePtr<Sampler> sampler;
    };

}
