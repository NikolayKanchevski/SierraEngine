//
// Created by Nikolay Kanchevski on 5.07.24.
//

#include "Texture.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Texture::Texture(const TextureCreateInfo& createInfo)
        : filter(createInfo.preferredFilter)
    {
        Sierra::ImageFormat format = createInfo.preferredFormat;
        Sierra::ImageUsage usage = Sierra::ImageUsage::DestinationMemory | Sierra::ImageUsage::Sample | (createInfo.preferredFilter == Sierra::SamplerFilter::Linear ? Sierra::ImageUsage::Filter : Sierra::ImageUsage::Undefined);

        if (std::optional<Sierra::ImageFormat> supportedFormat = createInfo.device.GetSupportedImageFormat(format, usage))
        {
            if (!supportedFormat.has_value() && createInfo.preferredFilter == Sierra::SamplerFilter::Linear)
            {
                filter = Sierra::SamplerFilter::Nearest;
                usage &= ~Sierra::ImageUsage::Filter;

                supportedFormat = createInfo.device.GetSupportedImageFormat(format, usage);
            }

            APP_THROW_IF(!supportedFormat.has_value(), Sierra::UnsupportedFeatureError(SR_FORMAT("Cannot create texture [{0}], device [{1}] supports no suitable image format for it", createInfo.name, createInfo.device.GetName())));
        }

        image = createInfo.device.CreateImage({
            .name = SR_FORMAT("Image of texture [{0}]", createInfo.name),
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