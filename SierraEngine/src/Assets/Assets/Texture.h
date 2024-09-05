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
        const Sierra::RenderingContext& renderingContext;

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
        [[nodiscard]] uint32 GetWidth() const { return image->GetWidth(); }
        [[nodiscard]] uint32 GetHeight() const { return image->GetHeight(); }
        [[nodiscard]] Sierra::ImageFormat GetFormat() const { return image->GetFormat(); }
        [[nodiscard]] Sierra::SamplerFilter GetFilter() const { return filter; }

        [[nodiscard]] uint32 GetLevelCount() const { return image->GetLevelCount(); }
        [[nodiscard]] uint32 GetLayerCount() const { return image->GetLayerCount(); }

        [[nodiscard]] const Sierra::Image& GetImage() const { return *image; }
        [[nodiscard]] AssetType GetType() const override { return AssetType::Texture; }

        /* --- MOVE SEMANTICS --- */
        Texture(Texture&&) = default;
        Texture& operator=(Texture&&) = default;

        /* --- DESTRUCTOR --- */
        ~Texture() override = default;

    private:
        std::unique_ptr<Sierra::Image> image = nullptr;
        Sierra::SamplerFilter filter = Sierra::SamplerFilter::Nearest;

    };

}