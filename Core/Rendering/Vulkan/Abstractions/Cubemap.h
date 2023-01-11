//
// Created by Nikolay Kanchevski on 29.12.22.
//

#pragma once

#include <memory>
#include <vulkan/vulkan.h>

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
        const char* filePaths[6];

        CubemapType cubemapType = CUBEMAP_TYPE_NONE;
        bool mipMappingEnabled = false;

        SamplerCreateInfo samplerCreateInfo{};
    };

    class Cubemap
    {
    public:
        /* --- CONSTRUCTORS --- */
        Cubemap(const CubemapCreateInfo &createInfo);
        static std::unique_ptr<Cubemap> Create(CubemapCreateInfo createInfo);

        /* --- POLLING METHODS --- */

        /* --- SETTER METHODS --- */

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::unique_ptr<Image>& GetImage() { return image; }
        [[nodiscard]] std::unique_ptr<Sampler>& GetSampler() { return sampler; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        Cubemap(const Cubemap &) = delete;
        Cubemap &operator=(const Cubemap &) = delete;

    private:
        int width;
        int height;

        bool mipMappingEnabled;
        int colorChannelsCount;
        CubemapType cubemapType = CUBEMAP_TYPE_NONE;

        uint32_t layerSize;
        uint64_t memorySize;

        std::unique_ptr<Image> image;
        std::unique_ptr<Sampler> sampler;
    };

}
