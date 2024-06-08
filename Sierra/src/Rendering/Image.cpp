//
// Created by Nikolay Kanchevski on 6.12.23.
//

#include "Image.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Image::Image(const ImageCreateInfo &createInfo)
        : width(createInfo.width), height(createInfo.height), depth(createInfo.depth), format(createInfo.format), levelCount(createInfo.levelCount), layerCount(createInfo.layerCount), sampling(createInfo.sampling)
    {
        SR_ERROR_IF(createInfo.width == 0, "Width of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.height == 0, "Height of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.depth == 0, "Depth of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(glm::ceil(static_cast<uint32>(static_cast<float32>(createInfo.width) / ImageFormatToBlockSize(createInfo.format)) * ImageFormatToBlockSize(createInfo.format)) != width, "Width of image [{0}] must be divisible by its format's block size!", createInfo.name);
        SR_ERROR_IF(glm::ceil(static_cast<uint32>(static_cast<float32>(createInfo.height) / ImageFormatToBlockSize(createInfo.format)) * ImageFormatToBlockSize(createInfo.format)) != height, "Width of image [{0}] must be divisible by its format's block size!", createInfo.name);
        SR_ERROR_IF(glm::ceil(static_cast<uint32>(static_cast<float32>(createInfo.depth) / ImageFormatToBlockSize(createInfo.format)) * ImageFormatToBlockSize(createInfo.format)) != depth, "Width of image [{0}] must be divisible by its format's block size!", createInfo.name);

        SR_ERROR_IF(createInfo.format == ImageFormat::Undefined, "Format of image [{0}] must not be [ImageFormat::Undefined]!", createInfo.name);
        SR_ERROR_IF(createInfo.levelCount == 0, "Level count of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.layerCount == 0, "Layer count of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.type == ImageType::Volume && createInfo.layerCount != 1, "Layer count of image [{0}] must be [1] if its type is set to [ImageType::Volume]!", createInfo.name);
        SR_ERROR_IF(createInfo.type == ImageType::Cube && createInfo.width != createInfo.height, "Width and height of image [{0}] must be equal if its type has been set to [ImageType::Cube]!", createInfo.name);
        SR_ERROR_IF(createInfo.type == ImageType::Cube && createInfo.layerCount % 6 != 0, "Layer count of image [{0}] must be a number divisible by 6 if its type has been set to [ImageType::Cube]!", createInfo.name);

        SR_ERROR_IF(createInfo.usage == ImageUsage::Undefined, "Usage of image [{0}] must not be [ImageUsage::Undefined]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::ColorAttachment && createInfo.usage & ImageUsage::DepthAttachment, "Usage of image [{0}] must not include both [ImageUsage::ColorAttachment] & [ImageUsage::DepthAttachment]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::Filter && !(createInfo.usage & ImageUsage::Sample), "Usage of image [{0}] must also include [ImageUsage::Sampled] if [ImageUsage::Filtered] is present!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::ResolverAttachment && createInfo.sampling == ImageSampling::x1, "Image [{0}], which includes [ImageUsage::ResolverAttachment] must be created with sampling, which differs from [ImageSampling::x1]!", createInfo.name);
        SR_ERROR_IF(createInfo.type != ImageType::Plane && createInfo.sampling != ImageSampling::x1, "Sampling of image [{0}] must be [ImageSampling::x1] if its type is not [ImageType::Plane]!", createInfo.name);
    }

}