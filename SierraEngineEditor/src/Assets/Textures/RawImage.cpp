//
// Created by Nikolay Kanchevski on 20.07.24.
//

#include "RawImage.h"

namespace SierraEngine
{

    namespace
    {
        // NOTE: This is necessary, as bit_cast doesn't work on Android

        uint32 ConvertFloatBitsToUInt(const float32 x)
        {
            return *reinterpret_cast<const uint32*>(&x);
        }

        float32 ConvertUIntBitsToFloat(const uint32 x)
        {
            return *reinterpret_cast<const float32*>(&x);
        }

        float32 Float16ToFloat32(const uint16 x)
        {
            const uint32 e = (x & 0x7C00) >> 10;
            const uint32 m = (x & 0x03FF) << 13;
            const uint32 v = ConvertFloatBitsToUInt(static_cast<float32>(m)) >> 23;
            return ConvertUIntBitsToFloat((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000)));
        }

        uint16 Float32ToFloat16(const float32 x)
        {
            const uint32 b = ConvertFloatBitsToUInt(x) + 0x00001000;
            const uint32 e = (b & 0x7F800000) >> 23;
            const uint32 m = b & 0x007FFFFF;
            return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) &  0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF;
        }

        template<typename T> requires std::is_arithmetic_v<T>
        void SetPixelMemory(void* pixelPointer, const uint8 channelCount, const Color64 color)
        {
            constexpr float64 MULTIPLIER = std::is_floating_point_v<T> ? 1.0 : static_cast<float64>(std::numeric_limits<T>::max());
            for (uint8 i = 0; i < channelCount; i++)
            {
                *(reinterpret_cast<T*>(pixelPointer) + i) = static_cast<T>(static_cast<float64>(color[i]) * MULTIPLIER);
            }
        }

        template<typename T> requires std::is_arithmetic_v<T>
        [[nodiscard]] Color64 GetPixelFromMemory(const void* pixelPointer, const uint8 channelCount)
        {
            Color64 color = { 0.0f, 0.0f, 0.0f, 0.0f };
            constexpr float64 DIVISOR = std::is_floating_point_v<T> ? 1.0 : static_cast<float64>(std::numeric_limits<T>::max());
            for (uint8 i = 0; i < channelCount; i++) color[i] = static_cast<float64>(reinterpret_cast<const T*>(pixelPointer)[i]) / DIVISOR;
            return color;
        }
    }

    /* --- CONSTRUCTORS --- */

    RawImage::RawImage(const RawImageCreateInfo& createInfo)
    {
        APP_ERROR_IF(createInfo.compressedMemory.empty(), "Cannot create an image with empty compressed memory!");
    }

    /* --- SETTER METHODS --- */

    bool RawImage::SetPixel(const Vector2UInt pixelCoordinate, const Color64 color)
    {
        APP_THROW_IF(pixelCoordinate.x >= GetWidth(), Sierra::ValueOutOfRangeError("Cannot set pixel at invalid position at the horizontal axis of image", pixelCoordinate.x, 0U, GetWidth()));
        APP_THROW_IF(pixelCoordinate.y >= GetHeight(), Sierra::ValueOutOfRangeError("Cannot set pixel at invalid position at the vertical axis of image", pixelCoordinate.x, 0U, GetWidth()));

        const Sierra::ImageFormat format = GetFormat();
        const uint8 channelMemorySize = static_cast<uint8>(Sierra::ImageFormatToChannelMemorySize(format));
        const uint8 channelCount = Sierra::ImageFormatToChannelCount(format);
        void* pixel = reinterpret_cast<uint8*>(GetMemory()) + (pixelCoordinate.y * GetWidth() + pixelCoordinate.x) * channelMemorySize * channelCount;

        switch (format)
        {
            case Sierra::ImageFormat::R8_Int:
            case Sierra::ImageFormat::R8G8_Int:
            case Sierra::ImageFormat::R8G8B8_Int:
            case Sierra::ImageFormat::R8G8B8A8_Int:
            {
                SetPixelMemory<int8>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R16_Int:
            case Sierra::ImageFormat::R16G16_Int:
            case Sierra::ImageFormat::R16G16B16_Int:
            case Sierra::ImageFormat::R16G16B16A16_Int:
            {
                SetPixelMemory<int16>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R32_Int:
            case Sierra::ImageFormat::R32G32_Int:
            case Sierra::ImageFormat::R32G32B32_Int:
            case Sierra::ImageFormat::R32G32B32A32_Int:
            {
                SetPixelMemory<int32>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R64_Int:
            case Sierra::ImageFormat::R64G64_Int:
            case Sierra::ImageFormat::R64G64B64_Int:
            case Sierra::ImageFormat::R64G64B64A64_Int:
            {
                SetPixelMemory<int64>(pixel, channelCount, color);
                break;
            }

            case Sierra::ImageFormat::R8_UInt:
            case Sierra::ImageFormat::R8G8_UInt:
            case Sierra::ImageFormat::R8G8B8_UInt:
            case Sierra::ImageFormat::R8G8B8A8_UInt:
            {
                SetPixelMemory<uint8>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R16_UInt:
            case Sierra::ImageFormat::R16G16_UInt:
            case Sierra::ImageFormat::R16G16B16_UInt:
            case Sierra::ImageFormat::R16G16B16A16_UInt:
            {
                SetPixelMemory<uint16>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R32_UInt:
            case Sierra::ImageFormat::R32G32_UInt:
            case Sierra::ImageFormat::R32G32B32_UInt:
            case Sierra::ImageFormat::R32G32B32A32_UInt:
            {
                SetPixelMemory<uint32>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R64_UInt:
            case Sierra::ImageFormat::R64G64_UInt:
            case Sierra::ImageFormat::R64G64B64_UInt:
            case Sierra::ImageFormat::R64G64B64A64_UInt:
            {
                SetPixelMemory<uint64>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R8_Norm:
            case Sierra::ImageFormat::R8G8_Norm:
            case Sierra::ImageFormat::R8G8B8_Norm:
            case Sierra::ImageFormat::R8G8B8A8_Norm:
            {
                SetPixelMemory<int8>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R16_Norm:
            case Sierra::ImageFormat::R16G16_Norm:
            case Sierra::ImageFormat::R16G16B16_Norm:
            case Sierra::ImageFormat::R16G16B16A16_Norm:
            {
                SetPixelMemory<int16>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R8_UNorm:
            case Sierra::ImageFormat::R8G8_UNorm:
            case Sierra::ImageFormat::R8G8B8_UNorm:
            case Sierra::ImageFormat::R8G8B8A8_UNorm:
            case Sierra::ImageFormat::B8G8R8A8_UNorm:
            {
                SetPixelMemory<uint8>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R16_UNorm:
            case Sierra::ImageFormat::R16G16_UNorm:
            case Sierra::ImageFormat::R16G16B16_UNorm:
            case Sierra::ImageFormat::R16G16B16A16_UNorm:
            {
                SetPixelMemory<uint16>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R16_Float:
            case Sierra::ImageFormat::R16G16_Float:
            case Sierra::ImageFormat::R16G16B16_Float:
            case Sierra::ImageFormat::R16G16B16A16_Float:
            {
                // Manually handle float16, as such type does not exist (we represent it using an uint16)
                for (uint8 i = 0; i < channelCount; i++)
                {
                    *(reinterpret_cast<uint16*>(pixel) + i) = Float32ToFloat16(static_cast<float32>(color[i]));
                }
                break;
            }
            case Sierra::ImageFormat::R32_Float:
            case Sierra::ImageFormat::R32G32_Float:
            case Sierra::ImageFormat::R32G32B32_Float:
            case Sierra::ImageFormat::R32G32B32A32_Float:
            {
                SetPixelMemory<float32>(pixel, channelCount, color);
                break;
            }
            case Sierra::ImageFormat::R64_Float:
            case Sierra::ImageFormat::R64G64_Float:
            case Sierra::ImageFormat::R64G64B64_Float:
            case Sierra::ImageFormat::R64G64B64A64_Float:
            {
                SetPixelMemory<float64>(pixel, channelCount, color);
                break;
            }
            default:
            {
                return false;
            }
        }

        return true;
    }

    /* --- GETTER METHODS --- */

    Color64 RawImage::GetPixel(const Vector2UInt pixelCoordinate) const
    {
        APP_THROW_IF(pixelCoordinate.x >= GetWidth(), Sierra::ValueOutOfRangeError("Cannot get pixel at invalid position at the horizontal axis of image", pixelCoordinate.x, 0U, GetWidth()));
        APP_THROW_IF(pixelCoordinate.y >= GetHeight(), Sierra::ValueOutOfRangeError("Cannot get pixel at invalid position at the vertical axis of image", pixelCoordinate.x, 0U, GetWidth()));

        const Sierra::ImageFormat format = GetFormat();
        const uint8 channelMemorySize = static_cast<uint8>(Sierra::ImageFormatToChannelMemorySize(format));
        const uint8 channelCount = Sierra::ImageFormatToChannelCount(format);
        const void* pixel = reinterpret_cast<const uint8*>(GetMemory()) + (pixelCoordinate.y * GetWidth() + pixelCoordinate.x) * channelMemorySize * channelCount;

        switch (format)
        {
            case Sierra::ImageFormat::R8_Int:
            case Sierra::ImageFormat::R8G8_Int:
            case Sierra::ImageFormat::R8G8B8_Int:
            case Sierra::ImageFormat::R8G8B8A8_Int:
            {
                return GetPixelFromMemory<int8>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_Int:
            case Sierra::ImageFormat::R16G16_Int:
            case Sierra::ImageFormat::R16G16B16_Int:
            case Sierra::ImageFormat::R16G16B16A16_Int:
            {
                return GetPixelFromMemory<int16>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R32_Int:
            case Sierra::ImageFormat::R32G32_Int:
            case Sierra::ImageFormat::R32G32B32_Int:
            case Sierra::ImageFormat::R32G32B32A32_Int:
            {
                return GetPixelFromMemory<int32>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R64_Int:
            case Sierra::ImageFormat::R64G64_Int:
            case Sierra::ImageFormat::R64G64B64_Int:
            case Sierra::ImageFormat::R64G64B64A64_Int:
            {
                return GetPixelFromMemory<int64>(pixel, channelCount);
            }

            case Sierra::ImageFormat::R8_UInt:
            case Sierra::ImageFormat::R8G8_UInt:
            case Sierra::ImageFormat::R8G8B8_UInt:
            case Sierra::ImageFormat::R8G8B8A8_UInt:
            {
                return GetPixelFromMemory<uint8>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_UInt:
            case Sierra::ImageFormat::R16G16_UInt:
            case Sierra::ImageFormat::R16G16B16_UInt:
            case Sierra::ImageFormat::R16G16B16A16_UInt:
            {
                return GetPixelFromMemory<uint16>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R32_UInt:
            case Sierra::ImageFormat::R32G32_UInt:
            case Sierra::ImageFormat::R32G32B32_UInt:
            case Sierra::ImageFormat::R32G32B32A32_UInt:
            {
                return GetPixelFromMemory<uint32>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R64_UInt:
            case Sierra::ImageFormat::R64G64_UInt:
            case Sierra::ImageFormat::R64G64B64_UInt:
            case Sierra::ImageFormat::R64G64B64A64_UInt:
            {
                return GetPixelFromMemory<uint64>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R8_Norm:
            case Sierra::ImageFormat::R8G8_Norm:
            case Sierra::ImageFormat::R8G8B8_Norm:
            case Sierra::ImageFormat::R8G8B8A8_Norm:
            {
                return GetPixelFromMemory<int8>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_Norm:
            case Sierra::ImageFormat::R16G16_Norm:
            case Sierra::ImageFormat::R16G16B16_Norm:
            case Sierra::ImageFormat::R16G16B16A16_Norm:
            {
                return GetPixelFromMemory<int16>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R8_UNorm:
            case Sierra::ImageFormat::R8G8_UNorm:
            case Sierra::ImageFormat::R8G8B8_UNorm:
            case Sierra::ImageFormat::R8G8B8A8_UNorm:
            case Sierra::ImageFormat::B8G8R8A8_UNorm:
            {
                return GetPixelFromMemory<uint8>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_UNorm:
            case Sierra::ImageFormat::R16G16_UNorm:
            case Sierra::ImageFormat::R16G16B16_UNorm:
            case Sierra::ImageFormat::R16G16B16A16_UNorm:
            {
                return GetPixelFromMemory<uint16>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_Float:
            case Sierra::ImageFormat::R16G16_Float:
            case Sierra::ImageFormat::R16G16B16_Float:
            case Sierra::ImageFormat::R16G16B16A16_Float:
            {
                // Manually handle float16, as such type does not exist (we represent it using an uint16)
                Color64 color = { 0.0f, 0.0f, 0.0f, 0.0f };
                for (uint8 i = 0; i < channelCount; i++) color[i] = static_cast<float64>(Float16ToFloat32(reinterpret_cast<const uint16*>(pixel)[i]));
                return color;
            }
            case Sierra::ImageFormat::R32_Float:
            case Sierra::ImageFormat::R32G32_Float:
            case Sierra::ImageFormat::R32G32B32_Float:
            case Sierra::ImageFormat::R32G32B32A32_Float:
            {
                return GetPixelFromMemory<float32>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R64_Float:
            case Sierra::ImageFormat::R64G64_Float:
            case Sierra::ImageFormat::R64G64B64_Float:
            case Sierra::ImageFormat::R64G64B64A64_Float:
            {
                return GetPixelFromMemory<float64>(pixel, channelCount);
            }
            default:
            {
//                return false;
            }
        }

//        return true;
        return {};
    }

}