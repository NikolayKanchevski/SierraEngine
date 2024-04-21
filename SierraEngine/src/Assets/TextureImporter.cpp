//
// Created by Nikolay Kanchevski on 20.02.24.
//

#include "TextureImporter.h"

#include "Transcoders/KTXTranscoder.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    TextureImporter::TextureImporter(const TextureImporterCreateInfo &createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    std::optional<ImportedTexture> TextureImporter::Import(const Sierra::RenderingContext &renderingContext, const std::string_view name, const SerializedTexture &serializedTexture, const std::span<const uint8> blob) const
    {
        const ImageTranscoderCreateInfo transcoderCreateInfo = { };
        const ImageTranscodeInfo transcodeInfo = { .compressedMemory = blob };

        std::optional<TranscodedImage> transcodedImage;
        switch (serializedTexture.index.compressorType)
        {
            case ImageSupercompressorType::Undefined:
            {
                APP_WARNING("Cannot import texture [{0}], which was incorrectly serialized without being compressed!", name);
                return std::nullopt;
            }
            case ImageSupercompressorType::KTX:
            {
                if (transcodedImage = KTXTranscoder(transcoderCreateInfo).Transcode(transcodeInfo); !transcodedImage.has_value())
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
            .type = transcodedImage.value().layerCount == 6 ? Sierra::ImageType::Cube : Sierra::ImageType::Plane,
            .format = imageFormat.value(),
            .levelCount = static_cast<uint32>(transcodedImage.value().levelMemories.size()),
            .layerCount = transcodedImage.value().layerCount,
            .usage = IMAGE_USAGE,
            .memoryLocation = Sierra::ImageMemoryLocation::GPU
        });

        // Create staging buffers to hold image data for every level
        std::vector<std::unique_ptr<Sierra::Buffer>> levelBuffers(transcodedImage.value().levelMemories.size());
        for (uint32 i = 0; i < transcodedImage.value().levelMemories.size(); i++)
        {
            levelBuffers[i] = renderingContext.CreateBuffer({
                .name = "Staging Buffer of Texture [" + std::string(name) + "] level [" + std::to_string(i) + "]",
                .memorySize = transcodedImage.value().levelMemories[i].size(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });

            levelBuffers[i]->CopyFromMemory(transcodedImage.value().levelMemories[i].data());
        }

        // Construct texture
        TextureAsset textureAsset = { };
        textureAsset.type = serializedTexture.index.type;
        textureAsset.image = std::move(image);

        ImportedTexture importedTexture
        {
            .texture = std::move(textureAsset),
            .levelBuffers = std::move(levelBuffers)
        };

        return importedTexture;
    }

}
