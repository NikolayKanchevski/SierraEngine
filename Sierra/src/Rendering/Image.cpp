//
// Created by Nikolay Kanchevski on 6.12.23.
//

#include "Image.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    float32 ImageFormatToChannelMemorySize(const ImageFormat format)
    {
        return ImageFormatToPixelMemorySize(format) / static_cast<float32>(ImageFormatToChannelCount(format));
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

    Image::Image(const ImageCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create image, as specified name must not be empty"));
        SR_THROW_IF(createInfo.width <= 0, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified width must be greater than [{0}]", createInfo.name)));
        SR_THROW_IF(createInfo.height <= 0, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified height must be greater than [{0}]", createInfo.name)));
        SR_THROW_IF(createInfo.depth <= 0, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified depth must be greater than [{0}]", createInfo.name)));

        SR_THROW_IF(createInfo.width % ImageFormatToBlockSize(createInfo.format) != 0, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified width [{1}] must be divisible by the format's block size [{2}]", createInfo.name, createInfo.width, ImageFormatToBlockSize(createInfo.format))));
        SR_THROW_IF(createInfo.height > 1 && createInfo.height % ImageFormatToBlockSize(createInfo.format) != 0, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified height [{1}] must either be divisible by the format's block size [{2}], or be set to [1]", createInfo.name, createInfo.height, ImageFormatToBlockSize(createInfo.format))));
        SR_THROW_IF(createInfo.depth > 1 && createInfo.depth % ImageFormatToBlockSize(createInfo.format) != 0, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified depth [{1}] must either be divisible by the format's block size [{2}], or be set to [1]", createInfo.name, createInfo.depth, ImageFormatToBlockSize(createInfo.format))));

        SR_THROW_IF(createInfo.format == ImageFormat::Undefined, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified format must not be [ImageFormat::Undefined]", createInfo.name)));
        SR_THROW_IF(createInfo.levelCount <= 0, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified level count must be greater than [{0}]", createInfo.name)));
        SR_THROW_IF(createInfo.layerCount <= 0, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified layer count must be greater than [{0}]", createInfo.name)));

        SR_THROW_IF(createInfo.type == ImageType::Volume && createInfo.layerCount != 1, InvalidConfigurationError(SR_FORMAT("Cannot create image [{0}], as specified layer count must be [1] if specified type is [ImageType::Volume]", createInfo.name)));
        SR_THROW_IF(createInfo.type == ImageType::Cube && createInfo.width != createInfo.height, InvalidConfigurationError(SR_FORMAT("Cannot create image [{0}], as specified width and height must be equal if specified type is [ImageType::Cube]", createInfo.name)));
        SR_THROW_IF(createInfo.type == ImageType::Cube && createInfo.layerCount % 6 != 0, InvalidConfigurationError(SR_FORMAT("Cannot create image [{0}], as specified layer count must be a number divisible by 6 if specified type is [ImageType::Cube]", createInfo.name)));

        SR_THROW_IF(createInfo.usage == ImageUsage::Undefined, InvalidValueError(SR_FORMAT("Cannot create image [{0}], as specified usage must not be [ImageUsage::Undefined]", createInfo.name)));
        SR_THROW_IF(createInfo.usage & ImageUsage::ColorAttachment && createInfo.usage & ImageUsage::DepthAttachment, InvalidConfigurationError(SR_FORMAT("Cannot create image [{0}], as specified usage cannot include both [ImageUsage::ColorAttachment] & [ImageUsage::DepthAttachment]", createInfo.name)));
        SR_THROW_IF(createInfo.usage & ImageUsage::Filter && !(createInfo.usage & ImageUsage::Sample), InvalidConfigurationError(SR_FORMAT("Cannot create image [{0}], as specified usage cannot include [ImageUsage::Filtered] unless [ImageUsage::Sample] is present as well", createInfo.name)));
        SR_THROW_IF(createInfo.usage & ImageUsage::ResolverAttachment && createInfo.sampling == ImageSampling::x1, InvalidConfigurationError(SR_FORMAT("Cannot create image [{0}], as specified usage cannot include [ImageUsage::ResolverAttachment] unless specified sampling differs from [ImageSampling::x1]", createInfo.name)));
        SR_THROW_IF(createInfo.type != ImageType::Plane && createInfo.sampling != ImageSampling::x1, InvalidConfigurationError(SR_FORMAT("Cannot create image [{0}], as specified Sampling must be [ImageSampling::x1] if its type is not [ImageType::Plane]", createInfo.name)));
    }

}