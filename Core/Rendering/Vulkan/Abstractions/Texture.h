//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Image.h"
#include "Sampler.h"
#include "../VulkanTypes.h"

#define BINDLESS_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::DIFFUSE)

#define TEXTURE_TYPE_TO_BINDING(textureType)(static_cast<uint>(textureType) + 2)
#define DIFFUSE_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::DIFFUSE)
#define SPECULAR_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::SPECULAR)
#define NORMAL_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::NORMAL_MAP)
#define HEIGHT_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TextureType::HEIGHT_MAP)

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    enum class TextureType
    {
        UNDEFINED = -1,
        DIFFUSE = 0,
        SPECULAR = 1,
        NORMAL_MAP = 2,
        HEIGHT_MAP = 3,
        TOTAL_COUNT = 4
    };

    struct TextureCreateInfo
    {
        String filePath;
        TextureType textureType = TextureType::UNDEFINED;
        ImageFormat imageFormat = ImageFormat::R8G8B8A8_UNORM;

        bool enableMipMapping = false;
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
        [[nodiscard]] inline uint GetWidth() const { return image->GetWidth(); }
        [[nodiscard]] inline uint GetHeight() const { return image->GetHeight(); }
        [[nodiscard]] inline uint GetDepth() const { return image->GetDepth(); }

        [[nodiscard]] inline ImageFormat GetImageFormat() const { return image->GetFormat(); }
        [[nodiscard]] inline TextureType GetTextureType() const { return textureType; }

        [[nodiscard]] inline bool GetMipMappingEnabled() const { return mipMappingEnabled; }
        [[nodiscard]] inline uint GetMipMapLevels() const { return image->GetMipMapLevels(); }

        [[nodiscard]] inline uint64 GetMemorySize() const{ return memorySize; }
        [[nodiscard]] inline UniquePtr<Sampler>& GetSampler() { return sampler; }
        [[nodiscard]] inline UniquePtr<Image>& GetImage() { return image; }

        [[nodiscard]] inline String GetFilePath() const { return filePath; }
        [[nodiscard]] ImTextureID GetImGuiTextureID();

        [[nodiscard]] static inline SharedPtr<Texture>& GetDefaultTexture(const TextureType textureType) { return defaultTextures[static_cast<uint>(textureType)]; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Texture);

    private:
        String filePath;

        TextureType textureType;
        uint colorChannelsCount;

        uint64 memorySize;
        UniquePtr<Sampler> sampler;

        UniquePtr<Image> image;
        bool mipMappingEnabled = false;

        VkDescriptorSet imGuiDescriptorSet;
        bool imGuiDescriptorSetCreated = false;

        bool isDefault = false;

        inline static SharedPtr<Texture> defaultTextures[static_cast<uint>(TextureType::TOTAL_COUNT)];
        inline static std::unordered_map<Hash, SharedPtr<Texture>> texturePool;
    };

}