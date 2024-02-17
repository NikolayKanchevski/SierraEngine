//
// Created by Nikolay Kanchevski on 21.11.23.
//

#include "Device.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Device::Device(const DeviceCreateInfo &createInfo)
    {

    }

    /* -- GETTER METHODS --- */

    std::optional<ImageFormat> Device::GetSupportedImageFormat(const ImageFormat preferredFormat, const ImageUsage usage) const
    {
        // NOTE: Though looking complex and heavy, realistically, the function should return almost immediately, and a format is pretty much guaranteed to be found
        std::vector<ImageChannels> channelsToTry;
        std::vector<ImageMemoryType> memoryTypesToTry;
        
        switch (preferredFormat.channels)
        {
            case ImageChannels::R:          channelsToTry = { ImageChannels::R, ImageChannels::RG, ImageChannels::RGB, ImageChannels::RGBA }; break;
            case ImageChannels::RG:         channelsToTry = { ImageChannels::RG, ImageChannels::RGB, ImageChannels::RGBA }; break;
            case ImageChannels::RGB:        channelsToTry = { ImageChannels::RGB, ImageChannels::RGBA }; break;
            case ImageChannels::RGBA:       channelsToTry = { ImageChannels::RGBA }; break;
            case ImageChannels::BGRA:       channelsToTry = { ImageChannels::BGRA }; break;
            case ImageChannels::D:          channelsToTry = { ImageChannels::D }; break;
        }

        switch (preferredFormat.memoryType)
        {
            case ImageMemoryType::Int8:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Int8,
                    ImageMemoryType::Int16,
                    ImageMemoryType::Int32,
                    ImageMemoryType::Int64,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::UInt8:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::UInt8,
                    ImageMemoryType::UInt16,
                    ImageMemoryType::UInt32,
                    ImageMemoryType::UInt64,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::SRGB8:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::SRGB8
                };
                break;
            }
            case ImageMemoryType::Norm8:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Norm8,
                    ImageMemoryType::Norm16,
                    ImageMemoryType::Float16,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::UNorm8:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::UNorm8,
                    ImageMemoryType::UNorm16,
                    ImageMemoryType::Float16,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::Int16:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Int16,
                    ImageMemoryType::Int32,
                    ImageMemoryType::Int64,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::UInt16:
            {

                memoryTypesToTry =
                {
                    ImageMemoryType::UInt16,
                    ImageMemoryType::UInt32,
                    ImageMemoryType::UInt64,
                    ImageMemoryType::Float16,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::Norm16:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Norm16,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::UNorm16:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::UNorm16,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::Float16:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Float16,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::Int32:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Int32,
                    ImageMemoryType::Int64,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::UInt32:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::UInt32,
                    ImageMemoryType::UInt64,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::Float32:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::Int64:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Int64
                };
                break;
            }
            case ImageMemoryType::UInt64:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::UInt64
                };
                break;
            }
            case ImageMemoryType::Float64:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Float64
                };
                break;
            }
        }

        for (const auto channels : channelsToTry)
        {
            for (const auto memoryType : memoryTypesToTry)
            {
                const ImageFormat format = { .channels = channels, .memoryType = memoryType };
                if (IsImageFormatSupported(format, usage)) return format;
            }
        }

        return std::nullopt;
    }

    ImageSampling Device::GetHighestImageSamplingSupported() const
    {
        if (IsImageSamplingSupported(ImageSampling::x64)) return ImageSampling::x64;
        if (IsImageSamplingSupported(ImageSampling::x32)) return ImageSampling::x32;
        if (IsImageSamplingSupported(ImageSampling::x16)) return ImageSampling::x16;
        if (IsImageSamplingSupported(ImageSampling::x8)) return ImageSampling::x8;
        if (IsImageSamplingSupported(ImageSampling::x4)) return ImageSampling::x4;
        if (IsImageSamplingSupported(ImageSampling::x2)) return ImageSampling::x2;
        return ImageSampling::x1;
    }

    SamplerAnisotropy Device::GetHighestSamplerAnisotropySupported() const
    {
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x64)) return SamplerAnisotropy::x64;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x32)) return SamplerAnisotropy::x32;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x16)) return SamplerAnisotropy::x16;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x8)) return SamplerAnisotropy::x8;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x4)) return SamplerAnisotropy::x4;
        if (IsSamplerAnisotropySupported(SamplerAnisotropy::x2)) return SamplerAnisotropy::x2;
        return SamplerAnisotropy::x1;
    }


}