//
// Created by Nikolay Kanchevski on 13.10.25.
//

#include "ImagePreviewer.h"

#include "../../../../Sierra/src/srpch.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    std::optional<SierraEngine::ImagePreview> ImagePreviewer::Preview(const ImagePreviewInfo& previewInfo)
    {
        const Sierra::Device& device = previewInfo.renderingContext.GetDevice();

        Sierra::CommandBuffer& commandBuffer = previewInfo.commandBuffer;
        Sierra::DestructionScheduler& destructionScheduler = previewInfo.renderingContext.GetDestructionScheduler();

        constexpr Sierra::ImageUsage USAGE = Sierra::ImageUsage::DestinationMemory | Sierra::ImageUsage::Sample;
        const std::optional<Sierra::ImageFormat> format = device.GetSupportedImageFormat(previewInfo.image.format, USAGE);

        if (!format.has_value())
        {
            APP_WARNING("Device [{0}] cannot create image preview, as no suitable format is supported", device.GetName());
            return std::nullopt;
        }


        const ImageConverter converter = { };
        const LoadedImage convertedImage = converter.Convert({previewInfo.image, format.value() });

        std::unique_ptr<Sierra::Image> image = device.CreateImage({
            .name = "Image Preview",
            .width = previewInfo.image.width,
            .height = previewInfo.image.height,
            .format = format.value(),
            .usage = USAGE
        });
        commandBuffer.SynchronizeImageUsage(*image, { .nextUsage = Sierra::ImageCommandUsage::MemoryWrite });

        std::unique_ptr<Sierra::Buffer> stagingBuffer = device.CreateBuffer({
            .name = "Image Preview Staging Buffer",
            .memorySize = convertedImage.memory.size(),
            .usage = Sierra::BufferUsage::SourceMemory,
            .memoryLocation = Sierra::BufferMemoryLocation::RAM
        });
        stagingBuffer->Write(convertedImage.memory.data(), 0, 0, convertedImage.memory.size());

        commandBuffer.CopyBufferToImage(*stagingBuffer, *image, { .pixelRange = {image->GetWidth(), image->GetHeight(), image->GetDepth() } });
        destructionScheduler.QueueResource(std::move(stagingBuffer));

        commandBuffer.SynchronizeImageUsage(*image, { .previousUsage = Sierra::ImageCommandUsage::MemoryWrite, .nextUsage = Sierra::ImageCommandUsage::GraphicsRead });

        ImagePreview preview
        {
            .image = std::move(image)
        };

        return preview;
    }

}
