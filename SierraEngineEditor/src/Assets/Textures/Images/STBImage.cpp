//
// Created by Nikolay Kanchevski on 20.07.24.
//

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_PIC
#define STBI_NO_PNM
#include <stb_image.h>

#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include <stb_image_resize2.h>

#include "STBImage.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    STBImage::STBImage(const ImageCreateInfo& createInfo)
        : Image(createInfo)
    {
        int imageWidth, imageHeight, imageChannelCount;
        APP_ERROR_IF(stbi_info_from_memory(reinterpret_cast<const stbi_uc*>(createInfo.compressedMemory.data()), static_cast<int>(createInfo.compressedMemory.size_bytes()), &imageWidth, &imageHeight, &imageChannelCount) != 1, "Failed to retrieve image properties using STB! Reason {0}.", stbi_failure_reason());
        const bool is16Bit = static_cast<bool>(stbi_is_16_bit_from_memory(reinterpret_cast<const stbi_uc*>(createInfo.compressedMemory.data()), static_cast<int>(createInfo.compressedMemory.size_bytes())));

        int imageChannelLoadCount;
        switch (createInfo.loadChannels)
        {
            case ImageLoadChannels::All:        { imageChannelLoadCount = imageChannelCount; break; }
            case ImageLoadChannels::R:          { imageChannelLoadCount = 1;                 break; }
            case ImageLoadChannels::RG:         { imageChannelLoadCount = 2;                 break; }
            case ImageLoadChannels::RGB:        { imageChannelLoadCount = 3;                 break; }
            case ImageLoadChannels::RGBA:       { imageChannelLoadCount = 4;                 break; }
        }

        memory = { is16Bit ? reinterpret_cast<void*>(stbi_load_16_from_memory(reinterpret_cast<const stbi_uc*>(createInfo.compressedMemory.data()), static_cast<int>(createInfo.compressedMemory.size_bytes()), &imageWidth, &imageHeight, &imageChannelCount, imageChannelLoadCount)) : reinterpret_cast<void*>(stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(createInfo.compressedMemory.data()), static_cast<int>(createInfo.compressedMemory.size_bytes()), &imageWidth, &imageHeight, &imageChannelCount, imageChannelLoadCount)), stbi_image_free };
        APP_ERROR_IF(memory == nullptr, "Failed to decompress image using STB! Reason: {0}.", stbi_failure_reason());

        width = static_cast<uint32>(imageWidth);
        height = static_cast<uint32>(imageHeight);

        switch (imageChannelLoadCount)
        {
            case 1:         { format = is16Bit ? Sierra::ImageFormat::R16_UNorm          : Sierra::ImageFormat::R8_UNorm;       break; }
            case 2:         { format = is16Bit ? Sierra::ImageFormat::R16G16_UNorm       : Sierra::ImageFormat::R8G8_UNorm;     break; }
            case 3:         { format = is16Bit ? Sierra::ImageFormat::R16G16B16_UNorm    : Sierra::ImageFormat::R8G8B8_UNorm;   break; }
            case 4:         { format = is16Bit ? Sierra::ImageFormat::R16G16B16A16_UNorm : Sierra::ImageFormat::R8G8B8A8_UNorm; break; }
            default:        break;
        }
    }

    /* --- POLLING METHODS --- */

    void STBImage::Resize(const uint32 givenWidth, const uint32 givenHeight)
    {
        if (givenWidth == 0 || givenHeight == 0) return;

        stbir_pixel_layout pixelLayout = STBIR_1CHANNEL;
        switch (Sierra::ImageFormatToChannelCount(format))
        {
            case 1:         { pixelLayout = STBIR_1CHANNEL; break; }
            case 2:         { pixelLayout = STBIR_2CHANNEL; break; }
            case 3:         { pixelLayout = STBIR_RGB;      break; }
            case 4:         { pixelLayout = STBIR_RGBA;     break; }
            default:        break;
        }

        stbir_datatype dataType = STBIR_TYPE_UINT8;
        switch (static_cast<uint8>(Sierra::ImageFormatToChannelMemorySize(format)))
        {
            case 1:         { dataType = STBIR_TYPE_UINT8; break; }
            case 2:         { dataType = STBIR_TYPE_UINT16; break; }
            default:        break;
        }

        std::unique_ptr<void, void(*)(void*)> resizedMemory = { reinterpret_cast<void*>(stbi__malloc(givenWidth * givenHeight * static_cast<uint8>(Sierra::ImageFormatToPixelMemorySize(format)))), stbi_image_free };
        stbir_resize(memory.get(), static_cast<int>(width), static_cast<int>(height), 0, resizedMemory.get(), static_cast<int>(givenWidth), static_cast<int>(givenHeight), 0, pixelLayout, dataType, STBIR_EDGE_ZERO, STBIR_FILTER_DEFAULT);

        memory = std::move(resizedMemory);
        width = givenWidth;
        height = givenHeight;
    }

    void STBImage::Crop(const Vector2UInt bottomLeft, const Vector2UInt topRight)
    {
        if (bottomLeft.x >= topRight.x || bottomLeft.y >= topRight.y || topRight.x >= width || topRight.y >= height) return;

        const uint32 newWidth = topRight.x - bottomLeft.x;
        const uint32 newHeight = topRight.y - bottomLeft.y;

        const uint8 channelMemorySize = static_cast<uint8>(Sierra::ImageFormatToChannelMemorySize(format));
        const uint8 channelCount = Sierra::ImageFormatToChannelCount(format);

        void* bottomLeftPixel = reinterpret_cast<uint8*>(memory.get()) + (bottomLeft.y * newWidth + bottomLeft.x) * channelMemorySize * channelCount;
        void* topRightPixel = reinterpret_cast<uint8*>(memory.get()) + (topRight.y * newWidth + topRight.x) * channelMemorySize * channelCount;

        std::unique_ptr<void, void(*)(void*)> croppedMemory = { reinterpret_cast<void*>(stbi__malloc(newWidth * newHeight * static_cast<uint8>(Sierra::ImageFormatToPixelMemorySize(format)))), stbi_image_free };
        std::memcpy(croppedMemory.get(), bottomLeftPixel, std::uintptr_t(topRightPixel) - std::uintptr_t(bottomLeftPixel));

        memory = std::move(croppedMemory);
        width = newWidth;
        height = newHeight;
    }

    /* --- SETTER METHODS --- */

    bool STBImage::SetPixel(const Vector2UInt coordinate, const Color color)
    {
        if (coordinate.x >= width || coordinate.y >= height) return false;

        const uint8 channelMemorySize = static_cast<uint8>(Sierra::ImageFormatToChannelMemorySize(format));
        const uint8 channelCount = Sierra::ImageFormatToChannelCount(format);
        void* pixel = reinterpret_cast<uint8*>(memory.get()) + (coordinate.y * width + coordinate.x) * channelMemorySize * channelCount;

        Color c;
        GetPixelColor<uint8>(pixel, channelCount, c);

        switch (channelMemorySize)
        {
            case 1:         { SetPixelMemory<uint8>(pixel, channelCount, color);  break; }
            case 2:         { SetPixelMemory<uint16>(pixel, channelCount, color); break; }
            default:        return false;
        }

        return true;
    }

    /* --- GETTER METHODS --- */

    Color STBImage::GetPixel(const Vector2UInt coordinate) const
    {
        Color color = { 0.0f, 0.0f, 0.0f, 0.0f };
        if (coordinate.x >= width || coordinate.y >= height) return color;

        const uint8 channelMemorySize = static_cast<uint8>(Sierra::ImageFormatToChannelMemorySize(format));
        const uint8 channelCount = Sierra::ImageFormatToChannelCount(format);
        const void* pixel = reinterpret_cast<uint8*>(memory.get()) + (coordinate.y * width + coordinate.x) * channelMemorySize * channelCount;

        switch (channelMemorySize)
        {
            case 1:         { GetPixelColor<uint8>(pixel, channelCount, color);  break; }
            case 2:         { GetPixelColor<uint16>(pixel, channelCount, color); break; }
            default:        return color;
        }

        return color;
    }

}