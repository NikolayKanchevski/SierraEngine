//
// Created by Nikolay Kanchevski on 19.11.24.
//

#include "TextureSerializer.h"

#include "Compressors/BasisUniversalCompressor.h"

namespace SierraEngine
{

    namespace
    {
        [[nodiscard]] Sierra::StreamResult SerializeRawBlob(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo)
        {
            for (const LoadedImageLevel& level : serializeInfo.levels)
            {
                for (const LoadedImage& layer : level.layers)
                {
                    if (const Sierra::StreamResult result = stream.Write(layer.buffer); result != Sierra::StreamResult::Success)
                    {
                        return result;
                    }
                }
            }

            return Sierra::StreamResult::Success;
        }

        [[nodiscard]] Sierra::StreamResult SerializeCompressedBlob(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo)
        {
            std::unique_ptr<ImageCompressor> compressor = nullptr;
            switch (serializeInfo.compressionSettings.compression)
            {
                case TextureCompression::BasisUniversal:  { compressor = std::make_unique<BasisUniversalCompressor>(); break; }
                default:                                  break;
            }

            std::optional<CompressedImage> compressedImage = compressor->Compress({ .levels = serializeInfo.levels, .compressionLevel = serializeInfo.compressionSettings.aggressiveness, .qualityLevel = serializeInfo.compressionSettings.quality });
            if (!compressedImage.has_value())
            {
                APP_WARNING("Could not compress image contents of texture [{0}], writing raw pixel buffer instead", serializeInfo.metadata.name);
                return Sierra::StreamResult::Unknown;
            }

            return stream.Write(compressedImage->buffer);
        }
    }

    /* --- POLLING METHODS --- */

    Sierra::StreamResult TextureSerializer::SerializeBlob(Sierra::Stream& blob, const TextureSerializeInfo& serializeInfo) const
    {
        if (serializeInfo.compressionSettings.compression != TextureCompression::None)
        {
            return SerializeCompressedBlob(blob, serializeInfo);
        }

        return SerializeRawBlob(blob, serializeInfo);
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