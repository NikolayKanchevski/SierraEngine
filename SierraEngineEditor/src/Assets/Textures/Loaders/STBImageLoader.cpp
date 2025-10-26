//
// Created by Nikolay Kanchevski on 11.11.24.
//

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_TGA
#include <stb_image.h>

#include "STBImageLoader.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    std::optional<LoadedImage> STBImageLoader::Load(const ImageLoadInfo& loadInfo) const noexcept
    {
        int width, height, channelCount;
        if (stbi_info_from_memory(reinterpret_cast<const stbi_uc*>(loadInfo.memory.data()), static_cast<int>(loadInfo.memory.size_bytes()), &width, &height, &channelCount) != 1)
        {
            return std::nullopt;
        }

        const bool is16Bit = static_cast<bool>(stbi_is_16_bit_from_memory(reinterpret_cast<const stbi_uc*>(loadInfo.memory.data()), static_cast<int>(loadInfo.memory.size_bytes())));

        int channelLoadCount = channelCount;
        switch (loadInfo.loadChannels)
        {
            case ImageLoadChannels::R:          { channelLoadCount = 1; break; }
            case ImageLoadChannels::RG:         { channelLoadCount = 2; break; }
            case ImageLoadChannels::RGB:        { channelLoadCount = 3; break; }
            case ImageLoadChannels::RGBA:       { channelLoadCount = 4; break; }
            default:                            break;
        }

        const std::unique_ptr<void, std::function<void(void*)>> rawMemory = { is16Bit ? reinterpret_cast<void*>(stbi_load_16_from_memory(reinterpret_cast<const stbi_uc*>(loadInfo.memory.data()), static_cast<int>(loadInfo.memory.size_bytes()), &width, &height, &channelCount, channelLoadCount)) : reinterpret_cast<void*>(stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(loadInfo.memory.data()), static_cast<int>(loadInfo.memory.size_bytes()), &width, &height, &channelCount, channelLoadCount)), stbi_image_free };
        if (rawMemory == nullptr)
        {
            APP_WARNING("Failed to STB load image! Error: {0}.", stbi_failure_reason());
            return std::nullopt;
        }

        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        switch (channelLoadCount)
        {
            case 1:         { format = is16Bit ? Sierra::ImageFormat::R16_UNorm          : Sierra::ImageFormat::R8_UNorm;       break; }
            case 2:         { format = is16Bit ? Sierra::ImageFormat::R16G16_UNorm       : Sierra::ImageFormat::R8G8_UNorm;     break; }
            case 3:         { format = is16Bit ? Sierra::ImageFormat::R16G16B16_UNorm    : Sierra::ImageFormat::R8G8B8_UNorm;   break; }
            case 4:         { format = is16Bit ? Sierra::ImageFormat::R16G16B16A16_UNorm : Sierra::ImageFormat::R8G8B8A8_UNorm; break; }
            default:        break;
        }

        std::vector<uint8> memory(width * height * channelLoadCount * (is16Bit + 1));
        std::memcpy(memory.data(), rawMemory.get(), memory.size());

        LoadedImage image
        {
            .width = static_cast<uint32>(width),
            .height = static_cast<uint32>(height),
            .format = format,
            .memory = std::move(memory)
        };

        return image;
    }

}