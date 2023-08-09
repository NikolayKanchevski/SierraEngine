//
// Created by Nikolay Kanchevski on 30.09.22.
//

#pragma once

#include "Image.h"
#include "Sampler.h"
#include "../Types.h"

namespace Sierra::Rendering
{

    struct BinaryTextureCreateInfo
    {
        const void* data = nullptr;
        uint32 width;
        uint32 height;

        ImageChannels channels = ImageChannels::RGBA;
        ImageMemoryType memoryType = ImageMemoryType::UINT8_NORM;

        bool enableSmoothFiltering = true;
        bool generateMipMaps = false;
    };

    struct TextureCreateInfo
    {
        FilePath filePath;
        ImageMemoryType memoryType = ImageMemoryType::UINT8_NORM;

        bool enableSmoothFiltering = true;
        bool generateMipMaps = false;
    };

    class Texture
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Texture(const BinaryTextureCreateInfo &createInfo);
        static SharedPtr<Texture> Load(const BinaryTextureCreateInfo &createInfo);
        static SharedPtr<Texture> Create(const TextureCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetWidth() const { return image->GetWidth(); }
        [[nodiscard]] inline uint32 GetHeight() const { return image->GetHeight(); }

        [[nodiscard]] inline ImageFormat GetImageFormat() const { return image->GetFormat(); }
        [[nodiscard]] inline bool IsMipMappingEnabled() const { return image->GetMipLevels() > 1; }
        [[nodiscard]] inline uint32 GetMipMapLevels() const { return image->GetMipLevels(); }

        [[nodiscard]] inline UniquePtr<Image>& GetImage() { return image; }
        [[nodiscard]] inline UniquePtr<Sampler>& GetSampler() { return sampler; }
        [[nodiscard]] ImTextureID GetImGuiTextureID();

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Texture);

    private:
        UniquePtr<Image> image;
        UniquePtr<Sampler> sampler;

        void GenerateMipMaps() const;
        ImTextureID imGuiDescriptorSet = nullptr;
    };

}