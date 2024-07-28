//
// Created by Nikolay Kanchevski on 5.07.24.
//

#include "Texture.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Texture::Texture(const TextureCreateInfo &createInfo)
        : filter(createInfo.preferredFilter)
    {
        Sierra::ImageFormat format = createInfo.preferredFormat;
        Sierra::ImageUsage usage = Sierra::ImageUsage::DestinationMemory | Sierra::ImageUsage::Sample | (createInfo.preferredFilter == Sierra::SamplerFilter::Linear ? Sierra::ImageUsage::Filter : Sierra::ImageUsage::Undefined);

        if (std::optional<Sierra::ImageFormat> supportedFormat = createInfo.renderingContext.GetDevice().GetSupportedImageFormat(format, usage))
        {
            if (!supportedFormat.has_value() && createInfo.preferredFilter == Sierra::SamplerFilter::Linear)
            {
                filter = Sierra::SamplerFilter::Nearest;
                usage &= ~Sierra::ImageUsage::Filter;

                supportedFormat = createInfo.renderingContext.GetDevice().GetSupportedImageFormat(format, usage);
            }

            APP_ERROR_IF(!supportedFormat.has_value(), "Cannot create texture [{0}], as no suitable image format is supported for it!", createInfo.name);
        }

        image = createInfo.renderingContext.CreateImage({
            .name = fmt::format("Image of texture [{0}]", createInfo.name),
            .width = createInfo.width,
            .height = createInfo.height,
            .type = createInfo.imageType,
            .format = format,
            .levelCount = createInfo.levelCount,
            .layerCount = createInfo.layerCount,
            .usage = usage,
            .memoryLocation = Sierra::ImageMemoryLocation::GPU
        });
    }

}