//
// Created by Nikolay Kanchevski on 29.12.22.
//

#pragma once

#include "Texture.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    typedef enum CubemapType
    {
        CUBEMAP_TYPE_NONE = -1,
        CUBEMAP_TYPE_SKYBOX = 0
    } CubemapType;

    struct CubemapCreateInfo
    {
        String filePaths[6];

        CubemapType cubemapType = CUBEMAP_TYPE_NONE;
        bool mipMappingEnabled = false;

        SamplerCreateInfo samplerCreateInfo{};
    };

    class Cubemap
    {
    public:
        /* --- CONSTRUCTORS --- */
        Cubemap(const CubemapCreateInfo &createInfo);
        static UniquePtr<Cubemap> Create(CubemapCreateInfo createInfo);

        /* --- POLLING METHODS --- */

        /* --- SETTER METHODS --- */

        /* --- GETTER METHODS --- */
        [[nodiscard]] UniquePtr<Image>& GetImage() { return image; }
        [[nodiscard]] UniquePtr<Sampler>& GetSampler() { return sampler; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Cubemap);

    private:
        int width;
        int height;

        bool mipMappingEnabled;
        int colorChannelsCount;
        CubemapType cubemapType = CUBEMAP_TYPE_NONE;

        uint layerSize;
        uint64 memorySize;

        UniquePtr<Image> image;
        UniquePtr<Sampler> sampler;
    };

}
