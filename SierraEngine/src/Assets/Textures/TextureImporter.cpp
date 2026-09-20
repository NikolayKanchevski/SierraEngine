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
        [[nodiscard]] ImportedTextureBuffer ImportRawBlob(const TextureProperties& header, const Sierra::ImageFormat format, Sierra::Stream& stream)
        {
            ImportedTextureBuffer buffer = { };

            if (header.format == format)
            {
                buffer.memory = stream.ReadToEnd();
                return buffer;
            }

            /* === Reference: https://gaim.umbc.edu/2010/05/27/mip-size/ === */
            const uint8 pixelMemorySize = Sierra::ImageFormatToBlockMemorySize(header.format);
            const size approximatedMemorySize = (4 * (header.width * header.height * pixelMemorySize) - ((header.width >> (header.levelCount - 1)) * (header.height >> (header.levelCount - 1)) * pixelMemorySize)) / 3;
            Sierra::MemoryWriteStream writeStream(approximatedMemorySize);

            const ImageConverter converter = { };
            for (size level = 0; level < header.levelCount; level++)
            {
                const uint32 width = glm::max(1U, header.width >> level);
                const uint32 height = glm::max(1U, header.height >> level);

                const size layerMemorySize = width * height * pixelMemorySize;
                for (size layer = 0; layer < header.layerCount; layer++)
                {
                    const LoadedImage image = LoadedImage({ .width = width, .height = height, .format = header.format, .memory = stream.Read(layerMemorySize) });

                    const LoadedImage convertedImage = converter.Convert({ .image = image, .format = format });
                    writeStream.WriteMemory(convertedImage.memory);
                }
            }

            buffer.memory = writeStream.Release();
            return buffer;
        }
    }

    std::optional<ImportedTextureBuffer> TextureImporter::ImportBlob(Sierra::Stream& blob, const Sierra::ImageFormat format, const TextureProperties& properties) const
    {
        std::unique_ptr<ImageTranscoder> transcoder = nullptr;
        switch (properties.compression)
        {
            case TextureCompression::None:              { return ImportRawBlob(properties, format, blob); }
            case TextureCompression::BasisUniversal:    { transcoder = std::make_unique<BasisUniversalTranscoder>(); break; }
        }

        std::optional<TranscodedImage> transcodedImage = transcoder->Transcode({ .memory = blob.ReadToEnd(), .format = format });
        if (!transcodedImage.has_value())
        {
            APP_WARNING("Could import texture, as transcoding compressed contents into desired format failed");
            return std::nullopt;
        }

        ImportedTextureBuffer buffer = { };
        buffer.memory = std::move(transcodedImage->memory);

        return buffer;
    }

}