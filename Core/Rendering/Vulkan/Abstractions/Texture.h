//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Image.h"
#include "Sampler.h"
#include "../VulkanTypes.h"

#define TOTAL_TEXTURE_TYPES_COUNT 3
#define TEXTURE_TYPE_TO_BINDING(textureType)(textureType + 2)

#define BINDLESS_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TEXTURE_TYPE_DIFFUSE)
#define DIFFUSE_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TEXTURE_TYPE_DIFFUSE)
#define SPECULAR_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TEXTURE_TYPE_SPECULAR)
#define HEIGHT_MAP_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TEXTURE_TYPE_HEIGHT_MAP)

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    class DescriptorSetLayout;
    class DescriptorPool;
    class DescriptorSet;

    typedef enum TextureType
    {
        TEXTURE_TYPE_NONE = -1,
        TEXTURE_TYPE_DIFFUSE = 0,
        TEXTURE_TYPE_SPECULAR = 1,
        TEXTURE_TYPE_HEIGHT_MAP = 2
    } TextureType;

    struct TextureCreateInfo
    {
        String filePath;

        String name;
        TextureType textureType = TEXTURE_TYPE_NONE;
        // TODO: Fix mip mapping
        bool mipMappingEnabled = false;

        SamplerCreateInfo samplerCreateInfo{};
    };

    class Texture
    {
    public:
        /* --- CONSTRUCTORS --- */
        Texture(stbi_uc *stbImage, uint width, uint height, uint givenColorChannelsCount, const TextureCreateInfo& textureCreateInfo);
        static SharedPtr<Texture> Create(TextureCreateInfo textureCreateInfo, bool setDefaultTexture = false);

        /* --- PROPERTIES --- */
        String name;

        /* --- SETTER METHODS --- */
        void Dispose();
        static void DisposePool();

        static void DestroyDefaultTextures();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline String GetName() const
        { return this->name; }

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

        [[nodiscard]] ImTextureID GetImGuiTextureID();

        [[nodiscard]] static inline SharedPtr<Texture>& GetDefaultTexture(TextureType textureType)
        { return defaultTextures[textureType]; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;

    private:
        String filePath = "";

        TextureType textureType;
        uint colorChannelsCount;

        uint64 memorySize;
        uint mipMapLevels = 1;

        UniquePtr<Sampler> sampler;

        UniquePtr<Image> image;
        bool mipMappingEnabled = false;

        VkDescriptorSet imGuiDescriptorSet;
        bool imGuiDescriptorSetCreated = false;

        bool isDefault = false;

        inline static SharedPtr<Texture> defaultTextures[TOTAL_TEXTURE_TYPES_COUNT];
        inline static std::unordered_map<String, SharedPtr<Texture>> texturePool;

        void GenerateMipMaps();
    };

}