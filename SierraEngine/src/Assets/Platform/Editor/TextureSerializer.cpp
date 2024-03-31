//
// Created by Nikolay Kanchevski on 1.03.24.
//

#include "TextureSerializer.h"

#include "Supercompressors/KTXSupercompressor.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    TextureSerializer::TextureSerializer(const TextureSerializerCreateInfo &createInfo)
        : version(createInfo.version), maximumTextureDimensions(createInfo.maximumTextureDimensions)
    {

    }

    /* --- POLLING METHODS --- */

    std::optional<std::pair<SerializedTexture, void*>> TextureSerializer::Serialize(const std::filesystem::path &filePath, const TextureSerializeInfo &serializeInfo)
    {
        // Define supercompression settings
        const ImageSupercompressorCreateInfo compressorCreateInfo = { .maximumImageDimensions = maximumTextureDimensions };
        const ImageSupercompressorSupercompressInfo compressInfo
        {
            .filePaths = { { filePath } },
            .normalMap = serializeInfo.type == TextureType::Normal,
            .compressionLevel = serializeInfo.compressionLevel,
            .qualityLevel = serializeInfo.qualityLevel
        };

        void* compressedImageMemory = nullptr;
        uint64 compressedImageMemorySize = 0;

        // Supercompress image
        switch (serializeInfo.compressorType)
        {
            case ImageSupercompressorType::Undefined:
            {
                APP_WARNING("Cannot serialize texture [{0}] using a compressor of type [ImageCompressorType::Undefined]!", filePath.string());
                return std::nullopt;
            }
            case ImageSupercompressorType::KTX:
            {
                if (!KTXSupercompressor(compressorCreateInfo).Supercompress(compressInfo, compressedImageMemory, compressedImageMemorySize))
                {
                    APP_WARNING("Could not serialize texture [{0}], as an error occurred while KTX compressing it!", filePath.string());
                    return std::nullopt;
                }
            }
        }

        // Set up asset data
        SerializedTexture serializedTexture = { };
        serializedTexture.version = version;
        serializedTexture.type = serializeInfo.type;
        serializedTexture.filtering = serializeInfo.filtering;
        serializedTexture.compressorType = serializeInfo.compressorType;
        serializedTexture.contentMemorySize = compressedImageMemorySize;

        return std::make_pair(serializedTexture, compressedImageMemory);
    }

}