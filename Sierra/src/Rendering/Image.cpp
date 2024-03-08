//
// Created by Nikolay Kanchevski on 6.12.23.
//

#include "Image.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Image::Image(const ImageCreateInfo &createInfo)
        : width(createInfo.width), height(createInfo.height), format(createInfo.format), mipLevelCount(createInfo.mipLevelCount), layerCount(createInfo.layerCount), sampling(createInfo.sampling)
    {
        SR_ERROR_IF(createInfo.width == 0, "Width of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.height == 0, "Height of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.format == ImageFormat::Undefined, "Format of image [{0}] must not be [ImageFormat::Undefined]!");
        SR_ERROR_IF(createInfo.mipLevelCount == 0, "Mip level count of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.layerCount == 0, "Layer count of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage == ImageUsage::Undefined, "Usage of image [{0}] must not be [ImageUsage::Undefined]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::ColorAttachment && createInfo.usage & ImageUsage::DepthAttachment, "Usage of image [{0}] must not include both [ImageUsage::ColorAttachment] & [ImageUsage::DepthAttachment]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::Filter && !(createInfo.usage & ImageUsage::Sample), "Usage of image [{0}] must also include [ImageUsage::Sampled] if [ImageUsage::Filtered] is present!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::ResolveAttachment && createInfo.sampling != ImageSampling::x1, "Image [{0}], which includes [ImageUsage::ResolveAttachment] must be created with sampling of [ImageSampling::x1]!", createInfo.name);
    }

}