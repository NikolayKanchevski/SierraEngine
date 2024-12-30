//
// Created by Nikolay Kanchevski on 6.11.24.
//

#include "TextureImporter.h"

#include "ImageConverter.h"
#include "Transcoders/BasisUniversalTranscoder.h"

namespace SierraEngine
{

    namespace
    {
        std::optional<TextureDetails> ImportRawMemory(Sierra::Stream& stream, const TextureImportInfo& importInfo, std::vector<uint8>& memory)
        {
            TextureDetails details = stream.Read<TextureDetails>();
            if (details.format == importInfo.format)
            {
                memory = stream.ReadToEnd();
                return details;
            }

            /* === Reference: https://gaim.umbc.edu/2010/05/27/mip-size/ === */
            const uint8 pixelMemorySize = ImageFormatToBlockMemorySize(details.format);
            const size approximatedMemorySize = (4 * (details.width * details.height * pixelMemorySize) - ((details.width >> (details.levelCount - 1)) * (details.height >> (details.levelCount - 1)) * pixelMemorySize)) / 3;
            Sierra::MemoryWriteStream writeStream(approximatedMemorySize);

            const ImageConverter converter = { };
            for (size level = 0; level < details.levelCount; level++)
            {
                const uint32 width = glm::max(1U, details.width >> level);
                const uint32 height = glm::max(1U, details.height >> level);

                const size layerMemorySize = width * height * pixelMemorySize;
                for (size layer = 0; layer < details.layerCount; layer++)
                {
                    const Image image = Image({ .width = width, .height = height, .format = details.format, .memory = stream.Read(layerMemorySize) });

                    Image convertedImage = converter.Convert({ .image = image, .format = importInfo.format });
                    writeStream.WriteMemory(convertedImage.Release());
                }
            }

            memory = writeStream.Release();
            return details;
        }

        std::optional<TextureDetails> ImportCompressedMemory(Sierra::Stream& stream, const TextureCompression compression, const TextureImportInfo& importInfo, std::vector<uint8>& memory)
        {
            std::unique_ptr<ImageTranscoder> transcoder = nullptr;
            switch (compression)
            {
                case TextureCompression::BasisUniversal:    { transcoder = std::make_unique<BasisUniversalTranscoder>(); break; }
                default:                                    break;
            }

            std::optional<TranscodedImage> transcodedImage = transcoder->Transcode({ .memory = stream.ReadToEnd(), .format = importInfo.format });
            if (!transcodedImage.has_value())
            {
                APP_WARNING("Could import texture, as transcoding compressed contents into desired format failed");
                return std::nullopt;
            }

            TextureDetails details
            {
                .width = transcodedImage->width,
                .height = transcodedImage->height,
                .levelCount = transcodedImage->levelCount,
                .layerCount = transcodedImage->layerCount,
                .format = importInfo.format
            };

            memory = std::move(transcodedImage->memory);
            return details;
        }
    }

    std::optional<TextureDetails> TextureImporter::ImportMemory(Sierra::Stream& stream, const TextureImportInfo& importInfo, std::vector<uint8>& memory) const
    {
        const TextureSerializer::TextureHeader header = stream.Read<TextureSerializer::TextureHeader>();
        if (header.compression == TextureCompression::None)
        {
            return ImportRawMemory(stream, importInfo, memory);
        }

        return ImportCompressedMemory(stream, header.compression, importInfo, memory);
    }

}