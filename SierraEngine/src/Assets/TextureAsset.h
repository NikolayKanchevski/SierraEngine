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
        Height,
        Skybox
    };

    class TextureAsset final : public Asset
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline TextureType GetType() const { return type; }
        [[nodiscard]] inline const std::unique_ptr<Sierra::Image>& GetImage() const { return image; }

        /* --- MOVE SEMANTICS --- */
        TextureAsset(TextureAsset&& other) = default;
        TextureAsset& operator=(TextureAsset&& other) = default;

        /* --- DESTRUCTOR --- */
        ~TextureAsset() = default;

    private:
        friend class TextureImporter;
        inline TextureAsset() = default;

        TextureType type = TextureType::Undefined;
        std::unique_ptr<Sierra::Image> image = nullptr;

    };

    struct TextureSerializeInfo
    {
        TextureType type = TextureType::Undefined;
        bool generateMipMaps = false;

        ImageSupercompressorType compressorType = ImageSupercompressorType::KTX;
        ImageSupercompressionLevel compressionLevel = ImageSupercompressionLevel::Standard;
        ImageSupercompressionQualityLevel qualityLevel = ImageSupercompressionQualityLevel::Standard;
    };

    struct SerializedTextureIndex
    {
        TextureType type = TextureType::Undefined;
        ImageSupercompressorType compressorType = ImageSupercompressorType::Undefined;
    };

    struct SerializedTexture
    {
        SerializedAssetHeader header = { };
        SerializedTextureIndex index = { };
    };
    using SerializedTextureBlob = std::vector<uint8>;

}
