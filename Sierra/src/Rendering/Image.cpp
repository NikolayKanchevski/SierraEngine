//
// Created by Nikolay Kanchevski on 6.12.23.
//

#include "Image.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Image::Image(const ImageCreateInfo &createInfo)
        : width(createInfo.width), height(createInfo.height), layerCount(createInfo.layerCount), format(createInfo.format), usage(createInfo.usage)
    {
        SR_ERROR_IF(createInfo.width == 0 || createInfo.height == 0, "Width and height of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.layerCount == 0, "Layer count of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage == ImageUsage::Undefined, "Usage of image [{0}] must not be [ImageUsage::Undefined]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::ColorAttachment && createInfo.usage & ImageUsage::DepthAttachment, "Usage of image [{0}] must not include both [ImageUsage::ColorAttachment] & [ImageUsage::DepthAttachment]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::Filtered && !(createInfo.usage & ImageUsage::Sampled), "Usage of image [{0}] must also include [ImageUsage::Sampled] if [ImageUsage::Filtered] is present!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::ResolveAttachment && createInfo.sampling != ImageSampling::x1, "Image [{0}], which includes [ImageUsage::ResolveAttachment] must be created with sampling of [ImageSampling::x1]!", createInfo.name);

        // Calculate memory size
        memorySize = createInfo.width * createInfo.height;
        switch (createInfo.format.channels)
        {
            case ImageChannels::R:
            {
                memorySize *= 1;
                break;
            }
            case ImageChannels::RG:
            {
                memorySize *= 2;
                break;
            }
            case ImageChannels::RGB:
            {
                memorySize *= 3;
                break;
            }
            case ImageChannels::RGBA:
            case ImageChannels::BGRA:
            {
                memorySize *= 4;
                break;
            }
        }
        switch (createInfo.format.memoryType)
        {
            case ImageMemoryType::Int8:
            case ImageMemoryType::UInt8:
            case ImageMemoryType::UNorm8:
            case ImageMemoryType::SRGB8:
            {
                memorySize *= 1;
                break;
            }
            case ImageMemoryType::Int16:
            case ImageMemoryType::UInt16:
            case ImageMemoryType::Float16:
            case ImageMemoryType::Norm16:
            case ImageMemoryType::UNorm16:
            {
                memorySize *= 2;
                break;
            }
            case ImageMemoryType::Int32:
            case ImageMemoryType::UInt32:
            case ImageMemoryType::Float32:
            {
                memorySize *= 4;
                break;
            }
            case ImageMemoryType::Int64:
            case ImageMemoryType::UInt64:
            case ImageMemoryType::Float64:
            {
                memorySize *= 8;
                break;
            }
        }

        // Calculate mip levels
        if (createInfo.enableMipMapping) mipLevels = static_cast<uint32>(std::floor(std::log2(std::max(createInfo.width, createInfo.height)))) + 1;
    }

}