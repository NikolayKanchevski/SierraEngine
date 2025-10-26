//
// Created by Nikolay Kanchevski on 19.11.24.
//

#include "TextureSerializer.h"
#include "TextureImporter.h"

#include "Compressors/BasisUniversalCompressor.h"

namespace SierraEngine
{

    namespace
    {
        void SerializeRawMemory(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo)
        {
            for (const LoadedImageLevel& level : serializeInfo.levels)
            {
                for (const LoadedImage& layer : level.layers)
                {
                    stream.Write(layer.memory.data(), layer.memory.size());
                }
            }
        }

        void SerializeCompressedMemory(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo)
        {
            std::unique_ptr<ImageCompressor> compressor = nullptr;
            switch (serializeInfo.compression)
            {
                case ImageCompression::BasisUniversal:    { compressor = std::make_unique<BasisUniversalCompressor>(); break; }
                default:                                  break;
            }

            std::optional<CompressedImage> compressedImage = compressor->Compress({ .levels = serializeInfo.levels, .compressionLevel = serializeInfo.compressionLevel, .qualityLevel = serializeInfo.compressionQualityLevel });
            if (!compressedImage.has_value())
            {
                APP_WARNING("Could not compress image contents of texture [{0}], writing raw pixel memory instead", serializeInfo.metadata.name);
                SerializeRawMemory(stream, serializeInfo);
                return;
            }

            stream.WriteMemory(compressedImage->memory);
        }
    }

    /* --- POLLING METHODS --- */

    void TextureSerializer::SerializeBlob(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo) const
    {
        const TextureHeader header
        {
            .width = serializeInfo.levels[0].layers[0].width,
            .height = serializeInfo.levels[0].layers[0].height,
            .levelCount = static_cast<uint32>(serializeInfo.levels.size()),
            .layerCount = static_cast<uint32>(serializeInfo.levels[0].layers.size()),
            .format = serializeInfo.levels[0].layers[0].format,
            .compression = serializeInfo.compression
        };
        stream.Write(header);

        if (serializeInfo.compression != ImageCompression::None)
        {
            SerializeCompressedMemory(stream, serializeInfo);
        }
        else
        {
            SerializeRawMemory(stream, serializeInfo);
        }
    }

    /* --- GETTER METHODS --- */

    size TextureSerializer::GetTextureMemorySize(const TextureSerializeInfo& serializeInfo) const noexcept
    {
        const LoadedImage& image = serializeInfo.levels[0].layers[0];
        const uint8 pixelMemorySize = Sierra::ImageFormatToBlockMemorySize(image.format);

        /* === Reference: https://gaim.umbc.edu/2010/05/27/mip-size/ === */
        const size approximatedMemorySize = (4 * (image.width * image.height * pixelMemorySize) - ((image.width >> (serializeInfo.levels.size() - 1)) * (image.height >> (serializeInfo.levels.size() - 1)) * pixelMemorySize)) / 3;
        return approximatedMemorySize;
    }

}