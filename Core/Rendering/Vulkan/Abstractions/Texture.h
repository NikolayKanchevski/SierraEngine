//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Image.h"
#include "Sampler.h"
#include "../VulkanTypes.h"

#define TOTAL_TEXTURE_TYPES_COUNT 4
#define TEXTURE_TYPE_TO_BINDING(textureType)(textureType + 2)

#define BINDLESS_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::DIFFUSE)
#define DIFFUSE_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::DIFFUSE)
#define SPECULAR_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::SPECULAR)
#define NORMAL_MAP_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::NORMAL_MAP)
#define HEIGHT_MAP_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::HEIGHT_MAP)

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    enum TextureType
    {
        UNDEFINED_TEXTURE = -1,
        DIFFUSE = 0,
        SPECULAR = 1,
        NORMAL_MAP = 2,
        HEIGHT_MAP = 3
    };

    struct TextureCreateInfo
    {
        String filePath;
        TextureType textureType = TextureType::UNDEFINED_TEXTURE;

        ImageFormat imageFormat = ImageFormat::R8G8B8A8_SRGB;

        bool mipMappingEnabled = false;
        SamplerCreateInfo samplerCreateInfo{};
    };

    class Texture
    {
    public:
        /* --- CONSTRUCTORS --- */
        Texture(stbi_uc *stbImage, uint width, uint height, uint givenColorChannelsCount, TextureCreateInfo &createInfo);
        static SharedPtr<Texture> Create(TextureCreateInfo createInfo, bool setDefaultTexture = false);

        /* --- SETTER METHODS --- */
        void Dispose();
        static void DisposePool();

        static void DestroyDefaultTextures();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetWidth() const
        { return this->image->GetWidth(); }

        [[nodiscard]] inline uint GetHeight() const
        { return this->image->GetHeight(); }

        [[nodiscard]] inline uint GetDepth() const
        { return this->image->GetDepth(); }

        [[nodiscard]] inline ImageFormat GetImageFormat() const
        { return this->image->GetFormat(); }

        [[nodiscard]] inline TextureType GetTextureType() const
        { return this->textureType; }

        [[nodiscard]] inline uint GetMipMapLevels() const
        { return this->image->GetMipMapLevels(); }

        [[nodiscard]] inline bool GetMipMappingEnabled() const
        { return this->mipMappingEnabled; }

        [[nodiscard]] inline uint64 GetMemorySize() const
        { return this->memorySize; }

        [[nodiscard]] inline VkSampler GetVulkanSampler() const
        { return sampler->GetVulkanSampler(); }

        [[nodiscard]] inline UniquePtr<Image>& GetImage()
        { return this->image; }

        [[nodiscard]] inline UniquePtr<Sampler>& GetSampler()
        { return this->sampler; }

        [[nodiscard]] inline String GetFilePath() const
        { return filePath; }

        [[nodiscard]] ImTextureID GetImGuiTextureID();

        [[nodiscard]] static inline SharedPtr<Texture>& GetDefaultTexture(TextureType textureType)
        { return defaultTextures[textureType]; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Texture);

    private:
        String filePath = "";

        TextureType textureType;
        uint colorChannelsCount;

        uint64 memorySize;
        UniquePtr<Sampler> sampler;

        UniquePtr<Image> image;
        bool mipMappingEnabled = false;

        VkDescriptorSet imGuiDescriptorSet;
        bool imGuiDescriptorSetCreated = false;

        bool isDefault = false;

        inline static SharedPtr<Texture> defaultTextures[TOTAL_TEXTURE_TYPES_COUNT];
        inline static std::unordered_map<Hash, SharedPtr<Texture>> texturePool;
    };

}