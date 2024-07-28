//
// Created by Nikolay Kanchevski on 21.11.23.
//

#include "Device.h"

namespace Sierra
{

    /* -- GETTER METHODS --- */

    std::optional<ImageFormat> Device::GetSupportedImageFormat(const ImageFormat preferredFormat, const ImageUsage usage) const
    {
        // NOTE: Though looking complex and heavy, realistically, the function should return almost immediately, and a format is pretty much guaranteed to be found

        // Otherwise format must be used for just rendering, which allows us to make some compromises
        std::vector<ImageFormat> formatsToTry;
        switch (preferredFormat)
        {
            case ImageFormat::Undefined:
            {
                return std::nullopt;
            }
            case ImageFormat::R8_Int:
            {
                formatsToTry = {
                    ImageFormat::R8_Int,
                    ImageFormat::R16_Int,
                    ImageFormat::R8G8_Int,
                    ImageFormat::R8G8B8_Int,
                    ImageFormat::R32_Int,
                    ImageFormat::R16G16_Int,
                    ImageFormat::R8G8B8A8_Int,
                    ImageFormat::R16G16B16_Int,
                    ImageFormat::R64_Int,
                    ImageFormat::R32G32_Int,
                    ImageFormat::R16G16B16A16_Int,
                    ImageFormat::R32G32B32_Int,
                    ImageFormat::R64G64_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R8_UInt:
            {
                formatsToTry = {
                    ImageFormat::R8_UInt,
                    ImageFormat::R16_UInt,
                    ImageFormat::R8G8_UInt,
                    ImageFormat::R8G8B8_UInt,
                    ImageFormat::R32_UInt,
                    ImageFormat::R16G16_UInt,
                    ImageFormat::R8G8B8A8_UInt,
                    ImageFormat::R16G16B16_UInt,
                    ImageFormat::R64_UInt,
                    ImageFormat::R32G32_UInt,
                    ImageFormat::R16G16B16A16_UInt,
                    ImageFormat::R32G32B32_UInt,
                    ImageFormat::R64G64_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R8_Norm:
            {
                formatsToTry = {
                    ImageFormat::R8_Norm,
                    ImageFormat::R16_Norm,
                    ImageFormat::R8G8_Norm,
                    ImageFormat::R8G8B8_Norm,
                    ImageFormat::R16G16_Norm,
                    ImageFormat::R8G8B8A8_Norm,
                    ImageFormat::R16G16B16_Norm,
                    ImageFormat::R16G16B16A16_Norm
                };
                break;
            }
            case ImageFormat::R8_UNorm:
            {
                formatsToTry = {
                    ImageFormat::R8_UNorm,
                    ImageFormat::R16_UNorm,
                    ImageFormat::R8G8_UNorm,
                    ImageFormat::R8G8B8_UNorm,
                    ImageFormat::R16G16_UNorm,
                    ImageFormat::R8G8B8A8_UNorm,
                    ImageFormat::R16G16B16_UNorm,
                    ImageFormat::R16G16B16A16_UNorm
                };
                break;
            }
            case ImageFormat::R8_SRGB:
            {
                formatsToTry = {
                    ImageFormat::R8_SRGB,
                    ImageFormat::R8G8_SRGB,
                    ImageFormat::R8G8B8_SRGB,
                    ImageFormat::R8G8B8A8_SRGB
                };
                break;
            }
            case ImageFormat::R8G8_Int:
            {
                formatsToTry = {
                    ImageFormat::R8G8_Int,
                    ImageFormat::R8G8B8_Int,
                    ImageFormat::R16G16_Int,
                    ImageFormat::R8G8B8A8_Int,
                    ImageFormat::R16G16B16_Int,
                    ImageFormat::R32G32_Int,
                    ImageFormat::R16G16B16A16_Int,
                    ImageFormat::R32G32B32_Int,
                    ImageFormat::R64G64_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R8G8_UInt:
            {
                formatsToTry = {
                    ImageFormat::R8G8_UInt,
                    ImageFormat::R8G8B8_UInt,
                    ImageFormat::R16G16_UInt,
                    ImageFormat::R8G8B8A8_UInt,
                    ImageFormat::R16G16B16_UInt,
                    ImageFormat::R32G32_UInt,
                    ImageFormat::R16G16B16A16_UInt,
                    ImageFormat::R32G32B32_UInt,
                    ImageFormat::R64G64_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R8G8_Norm:
            {
                formatsToTry = {
                    ImageFormat::R8G8_Norm,
                    ImageFormat::R8G8B8_Norm,
                    ImageFormat::R16G16_Norm,
                    ImageFormat::R8G8B8A8_Norm,
                    ImageFormat::R16G16B16_Norm,
                    ImageFormat::R16G16B16A16_Norm
                };
                break;
            }
            case ImageFormat::R8G8_UNorm:
            {
                formatsToTry = {
                    ImageFormat::R8G8_UNorm,
                    ImageFormat::R8G8B8_UNorm,
                    ImageFormat::R16G16_UNorm,
                    ImageFormat::R8G8B8A8_UNorm,
                    ImageFormat::R16G16B16_UNorm,
                    ImageFormat::R16G16B16A16_UNorm
                };
                break;
            }
            case ImageFormat::R8G8_SRGB:
            {
                formatsToTry = {
                    ImageFormat::R8G8_SRGB,
                    ImageFormat::R8G8B8_SRGB,
                    ImageFormat::R8G8B8A8_SRGB
                };
                break;
            }
            case ImageFormat::R8G8B8_Int:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8_Int,
                    ImageFormat::R8G8B8A8_Int,
                    ImageFormat::R16G16B16_Int,
                    ImageFormat::R16G16B16A16_Int,
                    ImageFormat::R32G32B32_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R8G8B8_UInt:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8_UInt,
                    ImageFormat::R8G8B8A8_UInt,
                    ImageFormat::R16G16B16_UInt,
                    ImageFormat::R16G16B16A16_UInt,
                    ImageFormat::R32G32B32_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R8G8B8_Norm:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8_Norm,
                    ImageFormat::R8G8B8A8_Norm,
                    ImageFormat::R16G16B16_Norm,
                    ImageFormat::R16G16B16A16_Norm
                };
                break;
            }
            case ImageFormat::R8G8B8_UNorm:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8_UNorm,
                    ImageFormat::R8G8B8A8_UNorm,
                    ImageFormat::R16G16B16_UNorm,
                    ImageFormat::R16G16B16A16_UNorm
                };
                break;
            }
            case ImageFormat::R8G8B8_SRGB:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8_SRGB,
                    ImageFormat::R8G8B8A8_SRGB
                };
                break;
            }
            case ImageFormat::R8G8B8A8_Int:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8A8_Int,
                    ImageFormat::R16G16B16A16_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R8G8B8A8_UInt:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8A8_UInt,
                    ImageFormat::R16G16B16A16_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R8G8B8A8_Norm:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8A8_Norm,
                    ImageFormat::R16G16B16A16_Norm
                };
                break;
            }
            case ImageFormat::R8G8B8A8_UNorm:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8A8_UNorm,
                    ImageFormat::R16G16B16A16_UNorm
                };
                break;
            }
            case ImageFormat::R8G8B8A8_SRGB:
            {
                formatsToTry = {
                    ImageFormat::R8G8B8A8_SRGB
                };
                break;
            }
            case ImageFormat::R16_Int:
            {
                formatsToTry = {
                    ImageFormat::R16_Int,
                    ImageFormat::R32_Int,
                    ImageFormat::R16G16_Int,
                    ImageFormat::R16G16B16_Int,
                    ImageFormat::R64_Int,
                    ImageFormat::R32G32_Int,
                    ImageFormat::R16G16B16A16_Int,
                    ImageFormat::R32G32B32_Int,
                    ImageFormat::R64G64_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R16_UInt:
            {
                formatsToTry = {
                    ImageFormat::R16_UInt,
                    ImageFormat::R32_UInt,
                    ImageFormat::R16G16_UInt,
                    ImageFormat::R16G16B16_UInt,
                    ImageFormat::R64_UInt,
                    ImageFormat::R32G32_UInt,
                    ImageFormat::R16G16B16A16_UInt,
                    ImageFormat::R32G32B32_UInt,
                    ImageFormat::R64G64_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R16_Norm:
            {
                formatsToTry = {
                    ImageFormat::R16_Norm,
                    ImageFormat::R16G16_Norm,
                    ImageFormat::R16G16B16_Norm,
                    ImageFormat::R16G16B16A16_Norm
                };
                break;
            }
            case ImageFormat::R16_UNorm:
            {
                formatsToTry = {
                    ImageFormat::R16_UNorm,
                    ImageFormat::R16G16_UNorm,
                    ImageFormat::R16G16B16_UNorm,
                    ImageFormat::R16G16B16A16_UNorm
                };
                break;
            }
            case ImageFormat::R16_Float:
            {
                formatsToTry = {
                    ImageFormat::R16_Float,
                    ImageFormat::R32_Float,
                    ImageFormat::R16G16_Float,
                    ImageFormat::R16G16B16_Float,
                    ImageFormat::R64_Float,
                    ImageFormat::R32G32_Float,
                    ImageFormat::R16G16B16A16_Float,
                    ImageFormat::R32G32B32_Float,
                    ImageFormat::R64G64_Float,
                    ImageFormat::R32G32B32A32_Float,
                    ImageFormat::R64G64B64_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R16G16_Int:
            {
                formatsToTry = {
                    ImageFormat::R16G16_Int,
                    ImageFormat::R16G16B16_Int,
                    ImageFormat::R32G32_Int,
                    ImageFormat::R16G16B16A16_Int,
                    ImageFormat::R32G32B32_Int,
                    ImageFormat::R64G64_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R16G16_UInt:
            {
                formatsToTry = {
                    ImageFormat::R16G16_UInt,
                    ImageFormat::R16G16B16_UInt,
                    ImageFormat::R32G32_UInt,
                    ImageFormat::R16G16B16A16_UInt,
                    ImageFormat::R32G32B32_UInt,
                    ImageFormat::R64G64_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R16G16_Norm:
            {
                formatsToTry = {
                    ImageFormat::R16G16_Norm,
                    ImageFormat::R16G16B16_Norm,
                    ImageFormat::R16G16B16A16_Norm
                };
                break;
            }
            case ImageFormat::R16G16_UNorm:
            {
                formatsToTry = {
                    ImageFormat::R16G16_UNorm,
                    ImageFormat::R16G16B16_UNorm,
                    ImageFormat::R16G16B16A16_UNorm
                };
                break;
            }
            case ImageFormat::R16G16_Float:
            {
                formatsToTry = {
                    ImageFormat::R16G16_Float,
                    ImageFormat::R16G16B16_Float,
                    ImageFormat::R32G32_Float,
                    ImageFormat::R16G16B16A16_Float,
                    ImageFormat::R32G32B32_Float,
                    ImageFormat::R64G64_Float,
                    ImageFormat::R32G32B32A32_Float,
                    ImageFormat::R64G64B64_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R16G16B16_Int:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16_Int,
                    ImageFormat::R16G16B16A16_Int,
                    ImageFormat::R32G32B32_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R16G16B16_UInt:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16_UInt,
                    ImageFormat::R16G16B16A16_UInt,
                    ImageFormat::R32G32B32_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R16G16B16_Norm:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16_Norm,
                    ImageFormat::R16G16B16A16_Norm
                };
                break;
            }
            case ImageFormat::R16G16B16_UNorm:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16_UNorm,
                    ImageFormat::R16G16B16A16_UNorm
                };
                break;
            }
            case ImageFormat::R16G16B16_Float:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16_Float,
                    ImageFormat::R16G16B16A16_Float,
                    ImageFormat::R32G32B32_Float,
                    ImageFormat::R32G32B32A32_Float,
                    ImageFormat::R64G64B64_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R16G16B16A16_Int:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16A16_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R16G16B16A16_UInt:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16A16_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R16G16B16A16_Norm:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16A16_Norm
                };
                break;
            }
            case ImageFormat::R16G16B16A16_UNorm:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16A16_UNorm
                };
                break;
            }
            case ImageFormat::R16G16B16A16_Float:
            {
                formatsToTry = {
                    ImageFormat::R16G16B16A16_Float,
                    ImageFormat::R32G32B32A32_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R32_Int:
            {
                formatsToTry = {
                    ImageFormat::R32_Int,
                    ImageFormat::R64_Int,
                    ImageFormat::R32G32_Int,
                    ImageFormat::R32G32B32_Int,
                    ImageFormat::R64G64_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R32_UInt:
            {
                formatsToTry = {
                    ImageFormat::R32_UInt,
                    ImageFormat::R64_UInt,
                    ImageFormat::R32G32_UInt,
                    ImageFormat::R32G32B32_UInt,
                    ImageFormat::R64G64_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R32_Float:
            {
                formatsToTry = {
                    ImageFormat::R32_Float,
                    ImageFormat::R64_Float,
                    ImageFormat::R32G32_Float,
                    ImageFormat::R32G32B32_Float,
                    ImageFormat::R64G64_Float,
                    ImageFormat::R32G32B32A32_Float,
                    ImageFormat::R64G64B64_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R32G32_Int:
            {
                formatsToTry = {
                    ImageFormat::R32G32_Int,
                    ImageFormat::R32G32B32_Int,
                    ImageFormat::R64G64_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R32G32_UInt:
            {
                formatsToTry = {
                    ImageFormat::R32G32_UInt,
                    ImageFormat::R32G32B32_UInt,
                    ImageFormat::R64G64_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R32G32_Float:
            {
                formatsToTry = {
                    ImageFormat::R32G32_Float,
                    ImageFormat::R32G32B32_Float,
                    ImageFormat::R64G64_Float,
                    ImageFormat::R32G32B32A32_Float,
                    ImageFormat::R64G64B64_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R32G32B32_Int:
            {
                formatsToTry = {
                    ImageFormat::R32G32B32_Int,
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R32G32B32_UInt:
            {
                formatsToTry = {
                    ImageFormat::R32G32B32_UInt,
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R32G32B32_Float:
            {
                formatsToTry = {
                    ImageFormat::R32G32B32_Float,
                    ImageFormat::R32G32B32A32_Float,
                    ImageFormat::R64G64B64_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R32G32B32A32_Int:
            {
                formatsToTry = {
                    ImageFormat::R32G32B32A32_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R32G32B32A32_UInt:
            {
                formatsToTry = {
                    ImageFormat::R32G32B32A32_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R32G32B32A32_Float:
            {
                formatsToTry = {
                    ImageFormat::R32G32B32A32_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R64_Int:
            {
                formatsToTry = {
                    ImageFormat::R64_Int,
                    ImageFormat::R64G64_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R64_UInt:
            {
                formatsToTry = {
                    ImageFormat::R64_UInt,
                    ImageFormat::R64G64_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R64_Float:
            {
                formatsToTry = {
                    ImageFormat::R64_Float,
                    ImageFormat::R64G64_Float,
                    ImageFormat::R64G64B64_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R64G64_Int:
            {
                formatsToTry = {
                    ImageFormat::R64G64_Int,
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R64G64_UInt:
            {
                formatsToTry = {
                    ImageFormat::R64G64_UInt,
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R64G64_Float:
            {
                formatsToTry = {
                    ImageFormat::R64G64_Float,
                    ImageFormat::R64G64B64_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::R64G64B64_Int:
            {
                formatsToTry = {
                    ImageFormat::R64G64B64_Int,
                    ImageFormat::R64G64B64A64_Int
                };
                break;
            }
            case ImageFormat::R64G64B64_UInt:
            {
                formatsToTry = {
                    ImageFormat::R64G64B64_UInt,
                    ImageFormat::R64G64B64A64_UInt
                };
                break;
            }
            case ImageFormat::R64G64B64_Float:
            {
                formatsToTry = {
                    ImageFormat::R64G64B64_Float,
                    ImageFormat::R64G64B64A64_Float
                };
                break;
            }
            case ImageFormat::D16_UNorm:
            {
                formatsToTry = {
                    ImageFormat::D16_UNorm,
                    ImageFormat::D32_Float
                };
                break;
            }
            case ImageFormat::R64G64B64A64_Int:
            case ImageFormat::R64G64B64A64_UInt:
            case ImageFormat::R64G64B64A64_Float:
            case ImageFormat::D32_Float:
            case ImageFormat::B8G8R8A8_UNorm:
            case ImageFormat::B8G8R8A8_SRGB:
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
            case ImageFormat::BC7_RGBA_SRGB:
            case ImageFormat::ASTC_4x4_UNorm:
            case ImageFormat::ASTC_4x4_SRGB:
            case ImageFormat::ASTC_8x8_UNorm:
            case ImageFormat::ASTC_8x8_SRGB:
            {
                formatsToTry = { preferredFormat };
            }
        }
        for (const ImageFormat format : formatsToTry)
        {
            if (IsImageFormatSupported(format, usage))
            {
                // If image is to be used for memory transfers, we must make sure suitable format has same channel memory size, or risk undefined behaviour and memory faults
                if ((usage & ImageUsage::SourceMemory || usage & ImageUsage::DestinationMemory || usage & ImageUsage::Storage) && static_cast<uint32>(ImageFormatToPixelMemorySize(preferredFormat) / ImageFormatToChannelCount(preferredFormat)) != static_cast<uint32>(ImageFormatToPixelMemorySize(format) / ImageFormatToChannelCount(format))) continue;
                return format;
            }
        }

        return std::nullopt;
    }

    ImageSampling Device::GetHighestImageSamplingSupported() const
    {
        if (IsImageSamplingSupported(ImageSampling::x64))       return ImageSampling::x64;
        if (IsImageSamplingSupported(ImageSampling::x32))       return ImageSampling::x32;
        if (IsImageSamplingSupported(ImageSampling::x16))       return ImageSampling::x16;
        if (IsImageSamplingSupported(ImageSampling::x8))        return ImageSampling::x8;
        if (IsImageSamplingSupported(ImageSampling::x4))        return ImageSampling::x4;
        if (IsImageSamplingSupported(ImageSampling::x2))        return ImageSampling::x2;
        return ImageSampling::x1;
    }

    SamplerAnisotropy Device::GetHighestSamplerAnisotropySupported() const
    {
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x64))       return SamplerAnisotropy::x64;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x32))       return SamplerAnisotropy::x32;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x16))       return SamplerAnisotropy::x16;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x8))        return SamplerAnisotropy::x8;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x4))        return SamplerAnisotropy::x4;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x2))        return SamplerAnisotropy::x2;
        return SamplerAnisotropy::x1;
    }


}