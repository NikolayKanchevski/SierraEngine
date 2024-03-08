//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "Asset.h"

#include "ImageSupercompressor.h"

namespace SierraEngine
{

    enum class TextureType : uint8
    {
        Undefined,
        Diffuse,
        Specular,
        Normal,
        Height
    };

    enum class TextureFiltering : bool
    {
        Pixelated,
        Smooth
    };

    class TextureAsset final : public Asset
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline TextureType GetType() const { return type; }
        [[nodiscard]] inline TextureFiltering GetFiltering() const { return filtering; }
        [[nodiscard]] inline const std::unique_ptr<Sierra::Image>& GetImage() const { return image; }

        /* --- MOVE SEMANTICS --- */
        TextureAsset(TextureAsset&& other) = default;
        TextureAsset &operator=(TextureAsset&& other) = default;

        /* --- DESTRUCTOR --- */
        ~TextureAsset() = default;

    private:
        friend class TextureImporter;
        inline TextureAsset() : Asset(AssetType::Texture) { }

        TextureType type = TextureType::Undefined;
        TextureFiltering filtering = TextureFiltering::Pixelated;
        std::unique_ptr<Sierra::Image> image = nullptr;

    };

    struct TextureSerializeInfo
    {
        TextureType type = TextureType::Undefined;
        TextureFiltering filtering = TextureFiltering::Smooth;

        ImageSupercompressorType compressorType = ImageSupercompressorType::KTX;
        ImageSupercompressionLevel compressionLevel = ImageSupercompressionLevel::Standard;
        ImageSupercompressionQualityLevel qualityLevel = ImageSupercompressionQualityLevel::Standard;
    };

    struct SerializedTexture : SerializedAsset
    {
        TextureType type = TextureType::Undefined;
        TextureFiltering filtering = TextureFiltering::Smooth;
        ImageSupercompressorType compressorType = ImageSupercompressorType::Undefined;

        uint64 contentMemorySize = 0;
        // void* content = rest of file
    };

}
