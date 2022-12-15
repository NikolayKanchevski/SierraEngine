//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include <string>
#include <memory>
#include <stb_image.h>
#include <unordered_map>
#include <vulkan/vulkan.h>

#include "Image.h"
#include "Sampler.h"
#include "../VulkanTypes.h"

#define TOTAL_TEXTURE_TYPES_COUNT 3
#define TEXTURE_TYPE_TO_BINDING(textureType)(textureType + 2)

#define BINDLESS_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TEXTURE_TYPE_DIFFUSE)
#define DIFFUSE_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TEXTURE_TYPE_DIFFUSE)
#define SPECULAR_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TEXTURE_TYPE_SPECULAR)
#define HEIGHT_MAP_TEXTURE_BINDING TEXTURE_TYPE_TO_BINDING(TEXTURE_TYPE_HEIGHT_MAP)

typedef enum TextureType
{
    TEXTURE_TYPE_NONE = -1,
    TEXTURE_TYPE_DIFFUSE = 0,
    TEXTURE_TYPE_SPECULAR = 1,
    TEXTURE_TYPE_HEIGHT_MAP = 2
} TextureType;

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    class DescriptorSetLayout;
    class DescriptorPool;
    class DescriptorSet;

    struct TextureCreateInfo
    {
        std::string filePath;

        std::string name;
        TextureType textureType = TEXTURE_TYPE_NONE;
        bool mipMappingEnabled = false;

        SamplerCreateInfo samplerCreateInfo{};
    };

    class Texture
    {
    public:
        /* --- CONSTRUCTORS --- */
        Texture(stbi_uc *stbImage, uint32_t width, uint32_t height, uint32_t givenColorChannelsCount, const TextureCreateInfo& textureCreateInfo);
        static std::shared_ptr<Texture> Create(TextureCreateInfo textureCreateInfo, bool setDefaultTexture = false);

        /* --- PROPERTIES --- */
        std::string name;

        /* --- SETTER METHODS --- */
        void Dispose();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string GetName() const
        { return this->name; }

        [[nodiscard]] inline uint32_t GetWidth() const
        { return this->image->GetWidth(); }

        [[nodiscard]] inline uint32_t GetHeight() const
        { return this->image->GetHeight(); }

        [[nodiscard]] inline uint32_t GetDepth() const
        { return this->image->GetDepth(); }

        [[nodiscard]] inline ImageFormat GetImageFormat() const
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
        { return defaultTextures[textureType]; }

        /* --- SETTER METHODS --- */
        void DrawToImGui();
        static void DestroyDefaultTextures();

        /* --- DESTRUCTOR --- */
        void Destroy();
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;

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

        VkDescriptorSet imGuiDescriptorSet;
        bool imGuiDescriptorSetCreated = false;

        inline static std::shared_ptr<Texture> defaultTextures[TOTAL_TEXTURE_TYPES_COUNT];
        inline static std::unordered_map<std::string, std::shared_ptr<Texture>> texturePool;
    };

}