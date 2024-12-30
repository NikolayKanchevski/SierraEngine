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

    enum class TextureFilter : bool
    {
        Pixelated,
        Smooth
    };

    struct TextureCreateInfo
    {
        Sierra::SamplerFilter filter = Sierra::SamplerFilter::Nearest;
        std::unique_ptr<Sierra::Image>&& image = nullptr;
    };

    class SIERRA_ENGINE_API TextureAsset final : public Asset
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TextureAsset(const TextureCreateInfo& createInfo);

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
        TextureAsset(const TextureAsset&) = delete;
        TextureAsset& operator=(const TextureAsset&) = delete;

        /* --- MOVE SEMANTICS --- */
        TextureAsset(TextureAsset&&) noexcept = default;
        TextureAsset& operator=(TextureAsset&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~TextureAsset() noexcept override = default;

    private:
        std::unique_ptr<Sierra::Image> image = nullptr;
        Sierra::SamplerFilter filter = Sierra::SamplerFilter::Nearest;

    };

}