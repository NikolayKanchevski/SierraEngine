//
// Created by Nikolay Kanchevski on 19.11.24.
//

#include "TextureSerializer.h"

#include "Compressors/BasisUniversalCompressor.h"

namespace SierraEngine
{

    namespace
    {
        void SerializeRawMemory(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo)
        {
            const TextureDetails details
            {
                .width = serializeInfo.levels[0].layers[0].GetWidth(),
                .height = serializeInfo.levels[0].layers[0].GetHeight(),
                .levelCount = static_cast<uint32>(serializeInfo.levels.size()),
                .layerCount = static_cast<uint32>(serializeInfo.levels[0].layers.size()),
                .format = serializeInfo.levels[0].layers[0].GetFormat()
            };
            stream.Write(details);

            for (const ImageLevel& level : serializeInfo.levels)
            {
                for (const Image& layer : level.layers)
                {
                    stream.Write(layer.GetMemory(), layer.GetMemorySize());
                }
            }
        }

        void SerializeCompressedMemory(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo)
        {
            std::unique_ptr<ImageCompressor> compressor = nullptr;
            switch (serializeInfo.compression)
            {
                case TextureCompression::BasisUniversal:    { compressor = std::make_unique<BasisUniversalCompressor>(); break; }
                default:                                    break;
            }

            std::optional<CompressedImage> compressedImage = compressor->Compress({ .levels = serializeInfo.levels, .compressionLevel = serializeInfo.compressionLevel, .qualityLevel = serializeInfo.qualityLevel });
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

    void TextureSerializer::SerializeMemory(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo) const
    {
        const TextureHeader header
        {
            .compression = serializeInfo.compression
        };
        stream.Write(header);

        if (serializeInfo.compression == TextureCompression::None)
        {
            SerializeRawMemory(stream, serializeInfo);
        }
        else
        {
            SerializeCompressedMemory(stream, serializeInfo);
        }
    }

}