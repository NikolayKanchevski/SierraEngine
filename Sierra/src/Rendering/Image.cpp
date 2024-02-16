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
        SR_ERROR_IF(createInfo.width == 0 || createInfo.height == 0, "Width and height of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.layerCount == 0, "Layer count of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.mipLevelCount == 0, "Mip level count of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.mipLevelCount > static_cast<uint32>(glm::floor(std::log2(glm::max(createInfo.width, createInfo.height)))) + 1, "Cannot create the [{0}] mip levels of image [{1}], as its dimensions do not allow it! Maximum mip level count is calculate like so: floor(log2(max(width, height))) + 1.", createInfo.mipLevelCount, createInfo.name);
        SR_ERROR_IF(createInfo.usage == ImageUsage::Undefined, "Usage of image [{0}] must not be [ImageUsage::Undefined]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::ColorAttachment && createInfo.usage & ImageUsage::DepthAttachment, "Usage of image [{0}] must not include both [ImageUsage::ColorAttachment] & [ImageUsage::DepthAttachment]!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::LinearFilter && !(createInfo.usage & ImageUsage::Sample), "Usage of image [{0}] must also include [ImageUsage::Sampled] if [ImageUsage::Filtered] is present!", createInfo.name);
        SR_ERROR_IF(createInfo.usage & ImageUsage::ResolveAttachment && createInfo.sampling != ImageSampling::x1, "Image [{0}], which includes [ImageUsage::ResolveAttachment] must be created with sampling of [ImageSampling::x1]!", createInfo.name);
    }

}
