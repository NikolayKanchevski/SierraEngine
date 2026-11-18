//
// Created by Nikolay Kanchevski on 13.10.25.
//

#include "ImagePreviewer.h"

#include "../../../../Sierra/src/srpch.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    std::optional<ImagePreview> ImagePreviewer::Preview(const DrawFrame& drawFrame, const ImagePreviewInfo& previewInfo)
    {
        constexpr Sierra::ImageUsage USAGE = Sierra::ImageUsage::DestinationMemory | Sierra::ImageUsage::Sample;
        const std::optional<Sierra::ImageFormat> format = drawFrame.device.GetSupportedImageFormat(previewInfo.image.format, USAGE);

        if (!format.has_value())
        {
            APP_WARNING("Device [{0}] cannot create image preview, as no suitable format is supported", drawFrame.device.GetName());
            return std::nullopt;
        }

        const ImageConverter converter = { };
        const LoadedImage convertedImage = converter.Convert({ .image = previewInfo.image, .format = format.value() });

        std::unique_ptr<Sierra::Image> image = drawFrame.device.CreateImage({
            .name = "Image Preview",
            .width = previewInfo.image.width,
            .height = previewInfo.image.height,
            .format = format.value(),
            .usage = USAGE
        });
        drawFrame.commandBuffer.SynchronizeImageUsage(*image, { .nextUsage = Sierra::ImageCommandUsage::MemoryWrite });

        std::unique_ptr<Sierra::Buffer> stagingBuffer = drawFrame.device.CreateBuffer({
            .name = "Image Preview Staging Buffer",
            .memorySize = convertedImage.buffer.size(),
            .usage = Sierra::BufferUsage::SourceMemory,
            .memoryLocation = Sierra::BufferMemoryLocation::RAM
        });
        stagingBuffer->Write(0, convertedImage.buffer);

        drawFrame.commandBuffer.CopyBufferToImage(*stagingBuffer, *image, { .pixelRange = {image->GetWidth(), image->GetHeight(), image->GetDepth() } });
        drawFrame.destructionScheduler.QueueResource(std::move(stagingBuffer));

        drawFrame.commandBuffer.SynchronizeImageUsage(*image, { .previousUsage = Sierra::ImageCommandUsage::MemoryWrite, .nextUsage = Sierra::ImageCommandUsage::GraphicsRead });

        ImagePreview preview
        {
            .image = std::move(image)
        };

        return preview;
    }

}
