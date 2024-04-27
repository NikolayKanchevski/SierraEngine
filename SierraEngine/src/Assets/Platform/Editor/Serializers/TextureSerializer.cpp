//
// Created by Nikolay Kanchevski on 1.03.24.
//

#include "TextureSerializer.h"

#include "Processors/Supercompressors/KTXSupercompressor.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    TextureSerializer::TextureSerializer(const TextureSerializerCreateInfo &createInfo)
        : maxTextureDimensions(createInfo.maxTextureDimensions)
    {

    }

    /* --- POLLING METHODS --- */

    std::optional<std::vector<uint8>> TextureSerializer::Serialize(const TextureSerializeInfo &serializeInfo)
    {
        // Define supercompression settings
        const ImageSupercompressorCreateInfo compressorCreateInfo = { .maxImageDimensions = maxTextureDimensions };
        const ImageSupercompressInfo compressInfo
        {
            .fileManager = serializeInfo.fileManager,
            .levelFilePaths = serializeInfo.levelFilePaths,
            .normalMap = serializeInfo.type == TextureType::Normal,
            .generateMipMaps = serializeInfo.generateMipMaps,
            .compressionLevel = serializeInfo.compressionLevel,
            .qualityLevel = serializeInfo.qualityLevel,
        };

        // Supercompress image
        std::optional<std::vector<uint8>> compressedMemory = std::nullopt;
        switch (serializeInfo.compressorType)
        {
            case ImageSupercompressorType::Undefined:
            {
                APP_WARNING("Cannot serialize texture [{0}] using a compressor of type [ImageCompressorType::Undefined]!", serializeInfo.levelFilePaths.begin()->begin()->string());
                return std::nullopt;
            }
            case ImageSupercompressorType::KTX:
            {
                if (compressedMemory = KTXSupercompressor(compressorCreateInfo).Supercompress(compressInfo); !compressedMemory.has_value())
                {
                    APP_WARNING("Could not serialize texture [{0}], as an error occurred while KTX compressing it!", serializeInfo.levelFilePaths.begin()->begin()->string());
                    return std::nullopt;
                }
            }
        }

        const SerializedTexture serializedTexture
        {
            .header = {
                .version = GetVersion()
            },
            .index = {
                .type = serializeInfo.type,
                .compressorType = serializeInfo.compressorType
            }
        };

        // Copy data to a dedicated blob
        std::vector<uint8> blob(sizeof(SerializedTexture) + compressedMemory->size());
        std::memcpy(blob.data(), &serializedTexture, sizeof(SerializedTexture));
        std::memcpy(blob.data() + sizeof(SerializedTexture), compressedMemory.value().data(), compressedMemory.value().size());

        return blob;
    }

}