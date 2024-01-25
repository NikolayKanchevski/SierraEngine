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

    ImageFormat Device::GetSupportedImageFormat(const ImageChannels preferredChannels, const ImageMemoryType preferredMemoryType, const ImageUsage usage) const
    {
        // NOTE: Though looking complex and heavy, realistically, the function should return almost immediately, and a format is pretty much guaranteed to be found
        std::vector<ImageChannels> channelsToTry;
        std::vector<ImageMemoryType> memoryTypesToTry;
        
        switch (preferredChannels)
        {
            case ImageChannels::R:          channelsToTry = { ImageChannels::R, ImageChannels::RG, ImageChannels::RGB, ImageChannels::RGBA }; break;
            case ImageChannels::RG:         channelsToTry = { ImageChannels::RG, ImageChannels::RGB, ImageChannels::RGBA }; break;
            case ImageChannels::RGB:        channelsToTry = { ImageChannels::RGB, ImageChannels::RGBA }; break;
            case ImageChannels::RGBA:       channelsToTry = { ImageChannels::RGBA }; break;
            case ImageChannels::BGRA:       channelsToTry = { ImageChannels::BGRA }; break;
            case ImageChannels::D:          channelsToTry = { ImageChannels::D }; break;
        }

        switch (preferredMemoryType)
        {
            case ImageMemoryType::Int8:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Int8,
                    ImageMemoryType::Int16,
                    ImageMemoryType::Int32,
                    ImageMemoryType::Int64,
                    ImageMemoryType::Float16,
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
                    ImageMemoryType::Float16,
                    ImageMemoryType::Float32,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::SRGB8:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::SRGB8,
                    ImageMemoryType::UNorm16,
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
                    ImageMemoryType::Float16,
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
                    ImageMemoryType::Norm16
                };
                break;
            }
            case ImageMemoryType::UNorm16:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::UNorm16
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
                    ImageMemoryType::Float32,
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
                    ImageMemoryType::Float32,
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
                    ImageMemoryType::Int64,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::UInt64:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::UInt64,
                    ImageMemoryType::Float64
                };
                break;
            }
            case ImageMemoryType::Float64:
            {
                memoryTypesToTry =
                {
                    ImageMemoryType::Float64,
                    ImageMemoryType::Int64
                };
                break;
            }
        }

        for (const auto channels : channelsToTry)
        {
            for (const auto memoryType : memoryTypesToTry)
            {
                const ImageFormat format = { .channels = channels, .memoryType = memoryType };
                if (IsImageConfigurationSupported(format, usage)) return format;
            }
        }

        SR_ERROR("No working image format supported for specified configuration!");
        return { };
    }

}