//
// Created by Nikolay Kanchevski on 6.12.23.
//

#include "Image.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    float32 ImageFormatToChannelMemorySize(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined:                break;

            case ImageFormat::R8_Int:
            case ImageFormat::R8_UInt:
            case ImageFormat::R8_Norm:
            case ImageFormat::R8_UNorm:
            case ImageFormat::R8_SRGB:
            case ImageFormat::R8G8_Int:
            case ImageFormat::R8G8_UInt:
            case ImageFormat::R8G8_Norm:
            case ImageFormat::R8G8_UNorm:
            case ImageFormat::R8G8_SRGB:
            case ImageFormat::R8G8B8_Int:
            case ImageFormat::R8G8B8_UInt:
            case ImageFormat::R8G8B8_Norm:
            case ImageFormat::R8G8B8_UNorm:
            case ImageFormat::R8G8B8_SRGB:
            case ImageFormat::R8G8B8A8_Int:
            case ImageFormat::R8G8B8A8_UInt:
            case ImageFormat::R8G8B8A8_Norm:
            case ImageFormat::R8G8B8A8_UNorm:
            case ImageFormat::R8G8B8A8_SRGB:
            case ImageFormat::B8G8R8A8_UNorm:
            case ImageFormat::B8G8R8A8_SRGB:            return 1;

            case ImageFormat::R16_Int:
            case ImageFormat::R16_UInt:
            case ImageFormat::R16_Norm:
            case ImageFormat::R16_UNorm:
            case ImageFormat::R16_Float:
            case ImageFormat::R16G16_Int:
            case ImageFormat::R16G16_UInt:
            case ImageFormat::R16G16_Norm:
            case ImageFormat::R16G16_UNorm:
            case ImageFormat::R16G16_Float:
            case ImageFormat::R16G16B16_Int:
            case ImageFormat::R16G16B16_UInt:
            case ImageFormat::R16G16B16_Norm:
            case ImageFormat::R16G16B16_UNorm:
            case ImageFormat::R16G16B16_Float:
            case ImageFormat::R16G16B16A16_Int:
            case ImageFormat::R16G16B16A16_UInt:
            case ImageFormat::R16G16B16A16_Norm:
            case ImageFormat::R16G16B16A16_UNorm:
            case ImageFormat::R16G16B16A16_Float:       return 2;

            case ImageFormat::R32_Int:
            case ImageFormat::R32_UInt:
            case ImageFormat::R32_Float:
            case ImageFormat::R32G32_Int:
            case ImageFormat::R32G32_UInt:
            case ImageFormat::R32G32_Float:
            case ImageFormat::R32G32B32_Int:
            case ImageFormat::R32G32B32_UInt:
            case ImageFormat::R32G32B32_Float:
            case ImageFormat::R32G32B32A32_Int:
            case ImageFormat::R32G32B32A32_UInt:
            case ImageFormat::R32G32B32A32_Float:       return 4;

            case ImageFormat::R64_Int:
            case ImageFormat::R64_UInt:
            case ImageFormat::R64_Float:
            case ImageFormat::R64G64_Int:
            case ImageFormat::R64G64_UInt:
            case ImageFormat::R64G64_Float:
            case ImageFormat::R64G64B64_Int:
            case ImageFormat::R64G64B64_UInt:
            case ImageFormat::R64G64B64_Float:
            case ImageFormat::R64G64B64A64_Int:
            case ImageFormat::R64G64B64A64_UInt:
            case ImageFormat::R64G64B64A64_Float:       return 8;

            case ImageFormat::D16_UNorm:                return 2;
            case ImageFormat::D32_Float:                return 4;

            case ImageFormat::BC1_RGB_UNorm:
            case ImageFormat::BC1_RGB_SRGB:
            case ImageFormat::BC1_RGBA_UNorm:
            case ImageFormat::BC1_RGBA_SRGB:            return 0.125f;
            case ImageFormat::BC3_RGBA_UNorm:
            case ImageFormat::BC3_RGBA_SRGB:            return 0.25f;
            case ImageFormat::BC4_R_Norm:
            case ImageFormat::BC4_R_UNorm:              return 0.125f;
            case ImageFormat::BC5_RG_Norm:
            case ImageFormat::BC5_RG_UNorm:
            case ImageFormat::BC6_HDR_RGB_Float:
            case ImageFormat::BC6_HDR_RGB_UFloat:
            case ImageFormat::BC7_RGB_UNorm:
            case ImageFormat::BC7_RGB_SRGB:
            case ImageFormat::BC7_RGBA_UNorm:
            case ImageFormat::BC7_RGBA_SRGB:            return 0.25f;

            case ImageFormat::ASTC_4x4_UNorm:
            case ImageFormat::ASTC_4x4_SRGB:            return 1;
            case ImageFormat::ASTC_8x8_UNorm:
            case ImageFormat::ASTC_8x8_SRGB:            return 2;
        }

        return 0;
    }

    float32 ImageFormatToPixelMemorySize(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined:                break;

            case ImageFormat::R8_Int:
            case ImageFormat::R8_UInt:
            case ImageFormat::R8_Norm:
            case ImageFormat::R8_UNorm:
            case ImageFormat::R8_SRGB:                  return 1;
            case ImageFormat::R8G8_Int:
            case ImageFormat::R8G8_UInt:
            case ImageFormat::R8G8_Norm:
            case ImageFormat::R8G8_UNorm:
            case ImageFormat::R8G8_SRGB:                return 2;
            case ImageFormat::R8G8B8_Int:
            case ImageFormat::R8G8B8_UInt:
            case ImageFormat::R8G8B8_Norm:
            case ImageFormat::R8G8B8_UNorm:
            case ImageFormat::R8G8B8_SRGB:              return 3;
            case ImageFormat::R8G8B8A8_Int:
            case ImageFormat::R8G8B8A8_UInt:
            case ImageFormat::R8G8B8A8_Norm:
            case ImageFormat::R8G8B8A8_UNorm:
            case ImageFormat::R8G8B8A8_SRGB:
            case ImageFormat::B8G8R8A8_UNorm:
            case ImageFormat::B8G8R8A8_SRGB:            return 4;

            case ImageFormat::R16_Int:
            case ImageFormat::R16_UInt:
            case ImageFormat::R16_Norm:
            case ImageFormat::R16_UNorm:
            case ImageFormat::R16_Float:                return 2;
            case ImageFormat::R16G16_Int:
            case ImageFormat::R16G16_UInt:
            case ImageFormat::R16G16_Norm:
            case ImageFormat::R16G16_UNorm:
            case ImageFormat::R16G16_Float:             return 4;
            case ImageFormat::R16G16B16_Int:
            case ImageFormat::R16G16B16_UInt:
            case ImageFormat::R16G16B16_Norm:
            case ImageFormat::R16G16B16_UNorm:
            case ImageFormat::R16G16B16_Float:          return 6;
            case ImageFormat::R16G16B16A16_Int:
            case ImageFormat::R16G16B16A16_UInt:
            case ImageFormat::R16G16B16A16_Norm:
            case ImageFormat::R16G16B16A16_UNorm:
            case ImageFormat::R16G16B16A16_Float:       return 8;

            case ImageFormat::R32_Int:
            case ImageFormat::R32_UInt:
            case ImageFormat::R32_Float:                return 4;
            case ImageFormat::R32G32_Int:
            case ImageFormat::R32G32_UInt:
            case ImageFormat::R32G32_Float:             return 8;
            case ImageFormat::R32G32B32_Int:
            case ImageFormat::R32G32B32_UInt:
            case ImageFormat::R32G32B32_Float:          return 12;
            case ImageFormat::R32G32B32A32_Int:
            case ImageFormat::R32G32B32A32_UInt:
            case ImageFormat::R32G32B32A32_Float:       return 16;

            case ImageFormat::R64_Int:
            case ImageFormat::R64_UInt:
            case ImageFormat::R64_Float:                return 8;
            case ImageFormat::R64G64_Int:
            case ImageFormat::R64G64_UInt:
            case ImageFormat::R64G64_Float:             return 16;
            case ImageFormat::R64G64B64_Int:
            case ImageFormat::R64G64B64_UInt:
            case ImageFormat::R64G64B64_Float:          return 24;
            case ImageFormat::R64G64B64A64_Int:
            case ImageFormat::R64G64B64A64_UInt:
            case ImageFormat::R64G64B64A64_Float:       return 32;

            case ImageFormat::D16_UNorm:                return 2;
            case ImageFormat::D32_Float:                return 4;

            case ImageFormat::BC1_RGB_UNorm:
            case ImageFormat::BC1_RGB_SRGB:
            case ImageFormat::BC1_RGBA_UNorm:
            case ImageFormat::BC1_RGBA_SRGB:            return 0.5f;
            case ImageFormat::BC3_RGBA_UNorm:
            case ImageFormat::BC3_RGBA_SRGB:            return 1;
            case ImageFormat::BC4_R_Norm:
            case ImageFormat::BC4_R_UNorm:              return 0.5f;
            case ImageFormat::BC5_RG_Norm:
            case ImageFormat::BC5_RG_UNorm:
            case ImageFormat::BC6_HDR_RGB_Float:
            case ImageFormat::BC6_HDR_RGB_UFloat:       return 1;
            case ImageFormat::BC7_RGB_UNorm:
            case ImageFormat::BC7_RGB_SRGB:
            case ImageFormat::BC7_RGBA_UNorm:
            case ImageFormat::BC7_RGBA_SRGB:            return 1;

            case ImageFormat::ASTC_4x4_UNorm:
            case ImageFormat::ASTC_4x4_SRGB:            return 1;
            case ImageFormat::ASTC_8x8_UNorm:
            case ImageFormat::ASTC_8x8_SRGB:            return 2;
        }

        return 0;
    }

    uint8 ImageFormatToChannelCount(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined:                break;

            case ImageFormat::R8_Int:
            case ImageFormat::R8_UInt:
            case ImageFormat::R8_Norm:
            case ImageFormat::R8_UNorm:
            case ImageFormat::R8_SRGB:
            case ImageFormat::R16_Int:
            case ImageFormat::R16_UInt:
            case ImageFormat::R16_Norm:
            case ImageFormat::R16_UNorm:
            case ImageFormat::R16_Float:
            case ImageFormat::R32_Int:
            case ImageFormat::R32_UInt:
            case ImageFormat::R32_Float:
            case ImageFormat::R64_Int:
            case ImageFormat::R64_UInt:
            case ImageFormat::R64_Float:                return 1;

            case ImageFormat::R8G8_Int:
            case ImageFormat::R8G8_UInt:
            case ImageFormat::R8G8_Norm:
            case ImageFormat::R8G8_UNorm:
            case ImageFormat::R8G8_SRGB:
            case ImageFormat::R16G16_Int:
            case ImageFormat::R16G16_UInt:
            case ImageFormat::R16G16_Norm:
            case ImageFormat::R16G16_UNorm:
            case ImageFormat::R16G16_Float:
            case ImageFormat::R32G32_Int:
            case ImageFormat::R32G32_UInt:
            case ImageFormat::R32G32_Float:
            case ImageFormat::R64G64_Int:
            case ImageFormat::R64G64_UInt:
            case ImageFormat::R64G64_Float:             return 2;

            case ImageFormat::R8G8B8_Int:
            case ImageFormat::R8G8B8_UInt:
            case ImageFormat::R8G8B8_Norm:
            case ImageFormat::R8G8B8_UNorm:
            case ImageFormat::R8G8B8_SRGB:
            case ImageFormat::R16G16B16_Int:
            case ImageFormat::R16G16B16_UInt:
            case ImageFormat::R16G16B16_Norm:
            case ImageFormat::R16G16B16_UNorm:
            case ImageFormat::R16G16B16_Float:
            case ImageFormat::R32G32B32_Int:
            case ImageFormat::R32G32B32_UInt:
            case ImageFormat::R32G32B32_Float:
            case ImageFormat::R64G64B64_Int:
            case ImageFormat::R64G64B64_UInt:
            case ImageFormat::R64G64B64_Float:          return 3;

            case ImageFormat::R8G8B8A8_Int:
            case ImageFormat::R8G8B8A8_UInt:
            case ImageFormat::R8G8B8A8_Norm:
            case ImageFormat::R8G8B8A8_UNorm:
            case ImageFormat::R8G8B8A8_SRGB:
            case ImageFormat::B8G8R8A8_UNorm:
            case ImageFormat::B8G8R8A8_SRGB:
            case ImageFormat::R16G16B16A16_Int:
            case ImageFormat::R16G16B16A16_UInt:
            case ImageFormat::R16G16B16A16_Norm:
            case ImageFormat::R16G16B16A16_UNorm:
            case ImageFormat::R16G16B16A16_Float:
            case ImageFormat::R32G32B32A32_Int:
            case ImageFormat::R32G32B32A32_UInt:
            case ImageFormat::R32G32B32A32_Float:
            case ImageFormat::R64G64B64A64_Int:
            case ImageFormat::R64G64B64A64_UInt:
            case ImageFormat::R64G64B64A64_Float:       return 4;

            case ImageFormat::D16_UNorm:
            case ImageFormat::D32_Float:                return 1;

            case ImageFormat::BC1_RGBA_UNorm:
            case ImageFormat::BC1_RGBA_SRGB:            return 4;
            case ImageFormat::BC1_RGB_UNorm:
            case ImageFormat::BC1_RGB_SRGB:             return 3;
            case ImageFormat::BC3_RGBA_UNorm:
            case ImageFormat::BC3_RGBA_SRGB:            return 4;
            case ImageFormat::BC4_R_UNorm:
            case ImageFormat::BC4_R_Norm:               return 1;
            case ImageFormat::BC5_RG_Norm:
            case ImageFormat::BC5_RG_UNorm:             return 2;
            case ImageFormat::BC6_HDR_RGB_Float:
            case ImageFormat::BC6_HDR_RGB_UFloat:       return 3;
            case ImageFormat::BC7_RGB_UNorm:
            case ImageFormat::BC7_RGB_SRGB:
            case ImageFormat::BC7_RGBA_UNorm:
            case ImageFormat::BC7_RGBA_SRGB:            return 4;

            case ImageFormat::ASTC_4x4_UNorm:
            case ImageFormat::ASTC_4x4_SRGB:
            case ImageFormat::ASTC_8x8_UNorm:
            case ImageFormat::ASTC_8x8_SRGB:            return 1;
        }

        return 0;
    }

    uint8 ImageFormatToBlockSize(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined:                break;

            case ImageFormat::R8_Int:
            case ImageFormat::R8_UInt:
            case ImageFormat::R8_Norm:
            case ImageFormat::R8_UNorm:
            case ImageFormat::R8_SRGB:
            case ImageFormat::R8G8_Int:
            case ImageFormat::R8G8_UInt:
            case ImageFormat::R8G8_Norm:
            case ImageFormat::R8G8_UNorm:
            case ImageFormat::R8G8_SRGB:
            case ImageFormat::R8G8B8_Int:
            case ImageFormat::R8G8B8_UInt:
            case ImageFormat::R8G8B8_Norm:
            case ImageFormat::R8G8B8_UNorm:
            case ImageFormat::R8G8B8_SRGB:
            case ImageFormat::R8G8B8A8_Int:
            case ImageFormat::R8G8B8A8_UInt:
            case ImageFormat::R8G8B8A8_Norm:
            case ImageFormat::R8G8B8A8_UNorm:
            case ImageFormat::R8G8B8A8_SRGB:
            case ImageFormat::B8G8R8A8_UNorm:
            case ImageFormat::B8G8R8A8_SRGB:

            case ImageFormat::R16_Int:
            case ImageFormat::R16_UInt:
            case ImageFormat::R16_Norm:
            case ImageFormat::R16_UNorm:
            case ImageFormat::R16_Float:
            case ImageFormat::R16G16_Int:
            case ImageFormat::R16G16_UInt:
            case ImageFormat::R16G16_Norm:
            case ImageFormat::R16G16_UNorm:
            case ImageFormat::R16G16_Float:
            case ImageFormat::R16G16B16_Int:
            case ImageFormat::R16G16B16_UInt:
            case ImageFormat::R16G16B16_Norm:
            case ImageFormat::R16G16B16_UNorm:
            case ImageFormat::R16G16B16_Float:
            case ImageFormat::R16G16B16A16_Int:
            case ImageFormat::R16G16B16A16_UInt:
            case ImageFormat::R16G16B16A16_Norm:
            case ImageFormat::R16G16B16A16_UNorm:
            case ImageFormat::R16G16B16A16_Float:

            case ImageFormat::R32_Int:
            case ImageFormat::R32_UInt:
            case ImageFormat::R32_Float:
            case ImageFormat::R32G32_Int:
            case ImageFormat::R32G32_UInt:
            case ImageFormat::R32G32_Float:
            case ImageFormat::R32G32B32_Int:
            case ImageFormat::R32G32B32_UInt:
            case ImageFormat::R32G32B32_Float:
            case ImageFormat::R32G32B32A32_Int:
            case ImageFormat::R32G32B32A32_UInt:
            case ImageFormat::R32G32B32A32_Float:

            case ImageFormat::R64_Int:
            case ImageFormat::R64_UInt:
            case ImageFormat::R64_Float:
            case ImageFormat::R64G64_Int:
            case ImageFormat::R64G64_UInt:
            case ImageFormat::R64G64_Float:
            case ImageFormat::R64G64B64_Int:
            case ImageFormat::R64G64B64_UInt:
            case ImageFormat::R64G64B64_Float:
            case ImageFormat::R64G64B64A64_Int:
            case ImageFormat::R64G64B64A64_UInt:
            case ImageFormat::R64G64B64A64_Float:

            case ImageFormat::D16_UNorm:
            case ImageFormat::D32_Float:                return 1;

            case ImageFormat::BC1_RGB_UNorm:
            case ImageFormat::BC1_RGB_SRGB:
            case ImageFormat::BC1_RGBA_UNorm:
            case ImageFormat::BC1_RGBA_SRGB:
            case ImageFormat::BC3_RGBA_UNorm:
            case ImageFormat::BC3_RGBA_SRGB:
            case ImageFormat::BC4_R_Norm:
            case ImageFormat::BC4_R_UNorm:
            case ImageFormat::BC5_RG_Norm:
            case ImageFormat::BC5_RG_UNorm:
            case ImageFormat::BC6_HDR_RGB_Float:
            case ImageFormat::BC6_HDR_RGB_UFloat:
            case ImageFormat::BC7_RGB_UNorm:
            case ImageFormat::BC7_RGB_SRGB:
            case ImageFormat::BC7_RGBA_UNorm:
            case ImageFormat::BC7_RGBA_SRGB:            return 4;

            case ImageFormat::ASTC_4x4_UNorm:
            case ImageFormat::ASTC_4x4_SRGB:            return 4;
            case ImageFormat::ASTC_8x8_UNorm:
            case ImageFormat::ASTC_8x8_SRGB:            return 8;
        }

        return 0;
    }

    /* --- CONSTRUCTORS --- */

    Image::Image(const ImageCreateInfo &createInfo)
    {
        SR_ERROR_IF(createInfo.width == 0, "Width of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.height == 0, "Height of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.depth == 0, "Depth of image [{0}] must not be [0]!", createInfo.name);
        SR_ERROR_IF(createInfo.width % ImageFormatToBlockSize(createInfo.format) != 0, "Width of image [{0}] must be divisible by its format's block size!", createInfo.name);
        SR_ERROR_IF(createInfo.height != 1 && createInfo.height % ImageFormatToBlockSize(createInfo.format) != 0, "Height of image [{0}] must be divisible by its format's block size!", createInfo.name);
        SR_ERROR_IF(createInfo.depth != 1 && createInfo.depth % ImageFormatToBlockSize(createInfo.format) != 0, "Depth of image [{0}] must be divisible by its format's block size!", createInfo.name);

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