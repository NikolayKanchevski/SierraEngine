//
// Created by Nikolay Kanchevski on 5.07.24.
//

#pragma once

#include "../Asset.h"

namespace SierraEngine
{

    enum class TextureType : uint8
    {
        Undefined,
        Albedo,
        Specular,
        Normal,
        Opacity,
        Roughness,
        Metallic,
        Displacement,
        Emission,
        Occlusion,
        Shadow,
        Environment
    };

    struct TextureCreateInfo
    {
        std::string_view name = "Texture";
        const Sierra::Device& device;

        uint32 width = 0;
        uint32 height = 0;

        Sierra::ImageType imageType = Sierra::ImageType::Plane;
        Sierra::ImageFormat preferredFormat = Sierra::ImageFormat::Undefined;
        Sierra::SamplerFilter preferredFilter = Sierra::SamplerFilter::Nearest;

        uint32 levelCount = 1;
        uint32 layerCount = 1;
    };

    class SIERRA_ENGINE_API Texture final : public Asset
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Texture(const TextureCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetWidth() const noexcept { return image->GetWidth(); }
        [[nodiscard]] uint32 GetHeight() const noexcept { return image->GetHeight(); }
        [[nodiscard]] Sierra::ImageFormat GetFormat() const noexcept { return image->GetFormat(); }
        [[nodiscard]] Sierra::SamplerFilter GetFilter() const noexcept { return filter; }

        [[nodiscard]] uint32 GetLevelCount() const noexcept { return image->GetLevelCount(); }
        [[nodiscard]] uint32 GetLayerCount() const noexcept { return image->GetLayerCount(); }

        [[nodiscard]] const Sierra::Image& GetImage() const noexcept { return *image; }
        [[nodiscard]] AssetType GetType() const noexcept override { return AssetType::Texture; }

        /* --- COPY SEMANTICS --- */
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        /* --- MOVE SEMANTICS --- */
        Texture(Texture&&) noexcept = default;
        Texture& operator=(Texture&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Texture() noexcept override = default;

    private:
        std::unique_ptr<Sierra::Image> image = nullptr;
        Sierra::SamplerFilter filter = Sierra::SamplerFilter::Nearest;

    };

}