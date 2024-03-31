//
// Created by Nikolay Kanchevski on 20.02.24.
//

#include "TextureImporter.h"

#include "Transcoders/KTXTranscoder.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    TextureImporter::TextureImporter(const TextureImporterCreateInfo &createInfo)
        : version(createInfo.version)
    {

    }

    /* --- POLLING METHODS --- */

    std::optional<std::pair<TextureAsset, std::unique_ptr<Sierra::Buffer>>> TextureImporter::Import(const Sierra::RenderingContext &renderingContext, const std::string_view name, const std::pair<const SerializedTexture, const void*> &serializedTexture)
    {
        const ImageTranscoderCreateInfo transcoderCreateInfo
        {

        };

        const ImageTranscodeInfo transcodeInfo
        {
            .compressedMemory = serializedTexture.second,
            .compressedMemorySize = serializedTexture.first.contentMemorySize
        };

        void* transcodedMemory = nullptr;
        std::optional<TranscodedImage> transcodedImage;

        switch (serializedTexture.first.compressorType)
        {
            case ImageSupercompressorType::Undefined:
            {
                APP_WARNING("Cannot import texture [{0}], which was incorrectly serialized without being compressed!", name);
                return std::nullopt;
            }
            case ImageSupercompressorType::KTX:
            {
                if (transcodedImage = KTXTranscoder(transcoderCreateInfo).Transcode(transcodeInfo, transcodedMemory); !transcodedImage.has_value())
                {
                    APP_WARNING("Could not import texture [{0}], as an error occurred while KTX transcoding it!", name);
                    return std::nullopt;
                }
                break;
            }
        }

        constexpr Sierra::ImageUsage IMAGE_USAGE = Sierra::ImageUsage::DestinationMemory | Sierra::ImageUsage::Sample | Sierra::ImageUsage::Filter;
        const std::optional<Sierra::ImageFormat> imageFormat = renderingContext.GetDevice().GetSupportedImageFormat(transcodedImage.value().format, IMAGE_USAGE);
        if (!imageFormat.has_value())
        {
            APP_WARNING("Could not import texture, as rendering context [{0}]'s device does not support any suitable image formats!", renderingContext.GetName());
            return std::nullopt;
        }

        // Create image
        std::unique_ptr<Sierra::Image> image = renderingContext.CreateImage({
            .name = name,
            .width = transcodedImage.value().width,
            .height = transcodedImage.value().height,
            .format = imageFormat.value(),
            .usage = IMAGE_USAGE,
            .memoryLocation = Sierra::ImageMemoryLocation::GPU
        });

        // Create staging buffer to hold image data
        std::unique_ptr<Sierra::Buffer> stagingBuffer = renderingContext.CreateBuffer({
            .name = "Staging Buffer of Texture [" + std::string(name) + "]",
            .memorySize = image->GetLayerMemorySize(),
            .usage = Sierra::BufferUsage::SourceMemory,
            .memoryLocation = Sierra::BufferMemoryLocation::CPU
        });

        // Copy raw image data
        stagingBuffer->CopyFromMemory(transcodedMemory);
        std::free(transcodedMemory);

        // Construct texture
        TextureAsset textureAsset = { };
        textureAsset.type = serializedTexture.first.type;
        textureAsset.filtering = serializedTexture.first.filtering;
        textureAsset.image = std::move(image);

        return std::make_pair(std::move(textureAsset), std::move(stagingBuffer));
    }

}
