//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <string>
#include <stb_image.h>
#include "Descriptors.h"
#include "Sampler.h"
#include "Image.h"
#include "../../../../Engine/Classes/Math.h"

typedef enum TextureType
{
    TEXTURE_TYPE_NONE = 0,
    TEXTURE_TYPE_DIFFUSE = 1,
    TEXTURE_TYPE_SPECULAR = 2,
    TEXTURE_TYPE_HEIGHTMAP = 3,
    TEXTURE_TYPE_NORMAL = 4
} TextureType;

#define DIFFUSE_TEXTURE_BINDING 1
#define SPECULAR_TEXTURE_BINDING 2
#define TOTAL_TEXTURE_TYPES_COUNT 2

// TODO: Refactor this crap
#define TextureBindingToArrayIndex(binding)(binding - 1)
#define TextureTypeToArrayIndex(textureType)(textureType - 1)
#define TextureTypeToBinding(textureType)(textureType)

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    class DescriptorSetLayout;
    class DescriptorPool;
    class DescriptorSet;

    struct TextureCreateInfo
    {
        std::string filePath = "";

        std::string name = "";
        TextureType textureType = TEXTURE_TYPE_NONE;
        bool mipMappingEnabled = false;

        SamplerCreateInfo samplerCreateInfo{};
        float minLod = 0.0f;
        float maxLod = 13.0f;
        float maxAnisotropy = 0.0f;
        bool applyBilinearFiltering = true;
        VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    };

    class Texture
    {
    public:
        /* --- CONSTRUCTORS --- */
        Texture(stbi_uc *stbImage, uint32_t width, uint32_t height, uint32_t givenColorChannelsCount, TextureCreateInfo textureCreateInfo);
        static std::shared_ptr<Texture> Create(TextureCreateInfo textureCreateInfo, bool setDefaultTexture = false);

        /* --- PROPERTIES --- */
        std::string name;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string GetName() const
        { return this->name; }

        [[nodiscard]] inline uint32_t GetWidth() const
        { return this->image->GetWidth(); }

        [[nodiscard]] inline uint32_t GetHeight() const
        { return this->image->GetHeight(); }

        [[nodiscard]] inline uint32_t GetDepth() const
        { return this->image->GetDepth(); }

        [[nodiscard]] inline VkFormat GetImageFormat() const
        { return this->image->GetFormat(); }

        [[nodiscard]] inline TextureType GetTextureType() const
        { return this->textureType; }

        [[nodiscard]] inline uint32_t GetMipMapLevels() const
        { return this->mipMapLevels; }

        [[nodiscard]] inline bool GetMipMappingEnabled() const
        { return this->mipMappingEnabled; }

        [[nodiscard]] inline uint64_t GetMemorySize() const
        { return this->memorySize; }

        [[nodiscard]] inline VkSampler GetVulkanSampler() const
        { return sampler->GetVulkanSampler(); }

        [[nodiscard]] inline std::unique_ptr<Image> &GetImage()
        { return this->image; }

        [[nodiscard]] inline std::unique_ptr<Sampler> &GetSampler()
        { return this->sampler; }

        [[nodiscard]] static inline std::shared_ptr<Texture>& GetDefaultTexture(TextureType textureType)
        { return defaultTextures[TextureTypeToArrayIndex(textureType)]; }

        /* --- SETTER METHODS --- */
        static void DestroyDefaultTextures();

        /* --- DESTRUCTOR --- */
        void Destroy();
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;

        static std::unordered_map<std::string, std::shared_ptr<Texture>> texturePool;
    private:
        std::string filePath = "";

        TextureType textureType;
        uint32_t colorChannelsCount;

        uint64_t memorySize;
        std::unique_ptr<Sampler> sampler;

        uint32_t mipMapLevels = 1;
        bool mipMappingEnabled = false;

        std::unique_ptr<Image> image;
        void GenerateMipMaps();

        static std::shared_ptr<Texture> defaultTextures[TOTAL_TEXTURE_TYPES_COUNT];
    };

}