//
// Created by Nikolay Kanchevski on 1.03.24.
//

#include "TextureSerializer.h"

#include "Supercompressors/KTXSupercompressor.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    TextureSerializer::TextureSerializer(const TextureSerializerCreateInfo &createInfo)
        : maxTextureDimensions(createInfo.maxTextureDimensions)
    {

    }

    /* --- POLLING METHODS --- */

    std::optional<std::pair<SerializedTexture, SerializedTextureBlob>> TextureSerializer::Serialize(const Sierra::FileManager &fileManager, const std::initializer_list<std::initializer_list<std::filesystem::path>> &levelFilePaths, const TextureSerializeInfo &serializeInfo)
    {
        // Define supercompression settings
        const ImageSupercompressorCreateInfo compressorCreateInfo = { .maxImageDimensions = maxTextureDimensions };
        const ImageSupercompressInfo compressInfo
        {
            .levelFilePaths = levelFilePaths,
            .normalMap = serializeInfo.type == TextureType::Normal,
            .generateMipMaps = serializeInfo.generateMipMaps,
            .compressionLevel = serializeInfo.compressionLevel,
            .qualityLevel = serializeInfo.qualityLevel
        };

        // Supercompress image
        std::optional<std::vector<uint8>> compressedMemory = std::nullopt;
        switch (serializeInfo.compressorType)
        {
            case ImageSupercompressorType::Undefined:
            {
                APP_WARNING("Cannot serialize texture [{0}] using a compressor of type [ImageCompressorType::Undefined]!", levelFilePaths.begin()->begin()->string());
                return std::nullopt;
            }
            case ImageSupercompressorType::KTX:
            {
                if (compressedMemory = KTXSupercompressor(compressorCreateInfo).Supercompress(fileManager, compressInfo); !compressedMemory.has_value())
                {
                    APP_WARNING("Could not serialize texture [{0}], as an error occurred while KTX compressing it!", levelFilePaths.begin()->begin()->string());
                    return std::nullopt;
                }
            }
        }

        SerializedTexture serializedTexture
        {
            .header = {
                .version = GetVersion()
            },
            .index = {
                .type = serializeInfo.type,
                .compressorType = serializeInfo.compressorType
            }
        };

        return std::make_pair(serializedTexture, std::move(compressedMemory.value()));
    }

}