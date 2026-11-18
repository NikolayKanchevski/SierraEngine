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
        [[nodiscard]] Sierra::StreamResult ImportRawBlob(const TextureProperties& header, const Sierra::ImageFormat format, Sierra::Stream& stream, std::vector<uint8>& outBuffer)
        {
            if (header.format == format)
                return stream.ReadToEnd(outBuffer);

            /* === Reference: https://gaim.umbc.edu/2010/05/27/mip-size/ === */
            const uint8 pixelMemorySize = Sierra::ImageFormatToBlockMemorySize(header.format);
            const size approximatedMemorySize = (4 * (header.width * header.height * pixelMemorySize) - ((header.width >> (header.levelCount - 1)) * (header.height >> (header.levelCount - 1)) * pixelMemorySize)) / 3;
            Sierra::WriteStream blob(approximatedMemorySize);

            const ImageConverter converter = { };
            for (size level = 0; level < header.levelCount; level++)
            {
                const uint32 width = glm::max(1U, header.width >> level);
                const uint32 height = glm::max(1U, header.height >> level);

                const size layerMemorySize = width * height * pixelMemorySize;
                for (size layer = 0; layer < header.layerCount; layer++)
                {
                    std::vector<uint8> layerBuffer = { };
                    layerBuffer.resize(layerMemorySize);

                    if (const Sierra::StreamResult result = stream.Read(layerBuffer); result != Sierra::StreamResult::Success)
                        return result;

                    const LoadedImage image = LoadedImage({ .width = width, .height = height, .format = header.format, .buffer = layerBuffer });
                    const LoadedImage convertedImage = converter.Convert({ .image = image, .format = format });

                    if (const Sierra::StreamResult result = blob.Write(convertedImage.buffer); result != Sierra::StreamResult::Success)
                    {
                        return result;
                    }
                }
            }

            outBuffer = blob.Release();
            return Sierra::StreamResult::Success;
        }
    }

    Sierra::StreamResult TextureImporter::ImportBlob(Sierra::Stream& blob, const Sierra::ImageFormat format, const TextureProperties& properties, std::vector<uint8>& outBuffer) const
    {
        std::unique_ptr<ImageTranscoder> transcoder = nullptr;
        switch (properties.compression)
        {
            case TextureCompression::None:              { return ImportRawBlob(properties, format, blob, outBuffer); }
            case TextureCompression::BasisUniversal:    { transcoder = std::make_unique<BasisUniversalTranscoder>(); break; }
        }

        std::optional<TranscodedImage> transcodedImage = transcoder->Transcode({ .stream = blob, .format = format });
        if (!transcodedImage.has_value())
        {
            APP_WARNING("Could import texture, as transcoding compressed contents into desired format failed");
            return Sierra::StreamResult::Unknown;
        }

        outBuffer = std::move(transcodedImage->buffer);
        return Sierra::StreamResult::Success;
    }

}