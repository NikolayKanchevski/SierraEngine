//
// Created by Nikolay Kanchevski on 10.10.25.
//

#include "ImageAccessor.h"

namespace SierraEngine
{

    namespace ImageAccessor
    {
        namespace
        {
            // NOTE: These are necessary, as bit_cast doesn't work on Android

            uint32 ConvertFloatBitsToUInt(const float32 x)
            {
                return *reinterpret_cast<const uint32*>(&x);
            }

            float32 ConvertUIntBitsToFloat(const uint32 value)
            {
                return *reinterpret_cast<const float32*>(&value);
            }

            /* === Reference: https://stackoverflow.com/a/60047308 === */

            float32 Float16ToFloat32(const uint16 value)
            {
                const uint32 e = (value & 0x7C00) >> 10;
                const uint32 m = (value & 0x03FF) << 13;
                const uint32 v = ConvertFloatBitsToUInt(static_cast<float32>(m)) >> 23;
                return ConvertUIntBitsToFloat((value & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000)));
            }

            uint16 Float32ToFloat16(const float32 value)
            {
                const uint32 b = ConvertFloatBitsToUInt(value) + 0x00001000;
                const uint32 e = (b & 0x7F800000) >> 23;
                const uint32 m = b & 0x007FFFFF;
                return (b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) &  0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF;
            }

            template<NumericType Numeric>
            [[nodiscard]] Color64 ReadPixel(const void* pixel, const uint8 channelCount)
            {
                Color64 color = { 0.0f, 0.0f, 0.0f, 0.0f };
                for (uint8 i = 0; i < channelCount; i++)
                {
                    constexpr float64 DIVISOR = FloatingPointType<Numeric> ? 1.0 : static_cast<float64>(std::numeric_limits<Numeric>::max());
                    color[i] = static_cast<float64>(reinterpret_cast<const Numeric*>(pixel)[i]) / DIVISOR;
                }

                return color;
            }

            template<NumericType Numeric>
            void WritePixel(void* pixel, const uint8 channelCount, const Color64 color)
            {
                for (uint8 i = 0; i < channelCount; i++)
                {
                    constexpr float64 MULTIPLIER = FloatingPointType<Numeric> ? 1.0 : static_cast<float64>(std::numeric_limits<Numeric>::max());
                    reinterpret_cast<Numeric*>(pixel)[i] = static_cast<Numeric>(static_cast<float64>(color[i]) * MULTIPLIER);
                }
            }
        }
    }

    Color64 ImageAccessor::ReadPixel(const LoadedImage& image, const Vector2UInt coordinate)
    {
        APP_THROW_IF(coordinate.x >= image.width, Sierra::ValueOutOfRangeError("Cannot get pixel at invalid position at the horizontal axis of image", coordinate.x, 0U, image.width));
        APP_THROW_IF(coordinate.y >= image.height, Sierra::ValueOutOfRangeError("Cannot get pixel at invalid position at the vertical axis of image", coordinate.y, 0U, image.height));

        const uint8 channelCount = Sierra::ImageFormatToChannelCount(image.format);
        const uint8 channelMemorySize = static_cast<uint8>(Sierra::ImageFormatToChannelMemorySize(image.format));

        const uint8* pixel = image.memory.data() + (coordinate.y * image.width + coordinate.x) * channelCount * channelMemorySize;
        switch (image.format)
        {
            case Sierra::ImageFormat::R8_Int:
            case Sierra::ImageFormat::R8G8_Int:
            case Sierra::ImageFormat::R8G8B8_Int:
            case Sierra::ImageFormat::R8G8B8A8_Int:
            {
                return ReadPixel<int8>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_Int:
            case Sierra::ImageFormat::R16G16_Int:
            case Sierra::ImageFormat::R16G16B16_Int:
            case Sierra::ImageFormat::R16G16B16A16_Int:
            {
                return ReadPixel<int16>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R32_Int:
            case Sierra::ImageFormat::R32G32_Int:
            case Sierra::ImageFormat::R32G32B32_Int:
            case Sierra::ImageFormat::R32G32B32A32_Int:
            {
                return ReadPixel<int32>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R64_Int:
            case Sierra::ImageFormat::R64G64_Int:
            case Sierra::ImageFormat::R64G64B64_Int:
            case Sierra::ImageFormat::R64G64B64A64_Int:
            {
                return ReadPixel<int64>(pixel, channelCount);
            }

            case Sierra::ImageFormat::R8_UInt:
            case Sierra::ImageFormat::R8G8_UInt:
            case Sierra::ImageFormat::R8G8B8_UInt:
            case Sierra::ImageFormat::R8G8B8A8_UInt:
            {
                return ReadPixel<uint8>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_UInt:
            case Sierra::ImageFormat::R16G16_UInt:
            case Sierra::ImageFormat::R16G16B16_UInt:
            case Sierra::ImageFormat::R16G16B16A16_UInt:
            {
                return ReadPixel<uint16>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R32_UInt:
            case Sierra::ImageFormat::R32G32_UInt:
            case Sierra::ImageFormat::R32G32B32_UInt:
            case Sierra::ImageFormat::R32G32B32A32_UInt:
            {
                return ReadPixel<uint32>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R64_UInt:
            case Sierra::ImageFormat::R64G64_UInt:
            case Sierra::ImageFormat::R64G64B64_UInt:
            case Sierra::ImageFormat::R64G64B64A64_UInt:
            {
                return ReadPixel<uint64>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R8_Norm:
            case Sierra::ImageFormat::R8G8_Norm:
            case Sierra::ImageFormat::R8G8B8_Norm:
            case Sierra::ImageFormat::R8G8B8A8_Norm:
            {
                return ReadPixel<int8>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_Norm:
            case Sierra::ImageFormat::R16G16_Norm:
            case Sierra::ImageFormat::R16G16B16_Norm:
            case Sierra::ImageFormat::R16G16B16A16_Norm:
            {
                return ReadPixel<int16>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R8_UNorm:
            case Sierra::ImageFormat::R8G8_UNorm:
            case Sierra::ImageFormat::R8G8B8_UNorm:
            case Sierra::ImageFormat::R8G8B8A8_UNorm:
            case Sierra::ImageFormat::B8G8R8A8_UNorm:
            {
                return ReadPixel<uint8>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_UNorm:
            case Sierra::ImageFormat::R16G16_UNorm:
            case Sierra::ImageFormat::R16G16B16_UNorm:
            case Sierra::ImageFormat::R16G16B16A16_UNorm:
            {
                return ReadPixel<uint16>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R16_Float:
            case Sierra::ImageFormat::R16G16_Float:
            case Sierra::ImageFormat::R16G16B16_Float:
            case Sierra::ImageFormat::R16G16B16A16_Float:
            {
                // Manually handle float16, as such type does not exist (we represent it using an uint16)
                Color64 color = { 0.0f, 0.0f, 0.0f, 0.0f };
                for (uint8 i = 0; i < channelCount; i++)
                {
                    color[i] = Float16ToFloat32(reinterpret_cast<const uint16*>(pixel)[i]);
                }
                return color;
            }
            case Sierra::ImageFormat::R32_Float:
            case Sierra::ImageFormat::R32G32_Float:
            case Sierra::ImageFormat::R32G32B32_Float:
            case Sierra::ImageFormat::R32G32B32A32_Float:
            {
                return ReadPixel<float32>(pixel, channelCount);
            }
            case Sierra::ImageFormat::R64_Float:
            case Sierra::ImageFormat::R64G64_Float:
            case Sierra::ImageFormat::R64G64B64_Float:
            case Sierra::ImageFormat::R64G64B64A64_Float:
            {
                return ReadPixel<float64>(pixel, channelCount);
            }
            default:
            {
                break;
            }
        }

        APP_WARNING("Could not get pixel [{0}, {1}] of image, as it is not of a basic format", coordinate.x, coordinate.y);
        return { };
    }

    void ImageAccessor::WritePixel(LoadedImage& image, const Vector2UInt coordinate, const Color64 color)
    {
        APP_THROW_IF(coordinate.x >= image.width, Sierra::ValueOutOfRangeError("Cannot set pixel at invalid position at the horizontal axis of image", coordinate.x, 0U, image.width));
        APP_THROW_IF(coordinate.y >= image.height, Sierra::ValueOutOfRangeError("Cannot set pixel at invalid position at the vertical axis of image", coordinate.y, 0U, image.height));

        const uint8 channelCount = Sierra::ImageFormatToChannelCount(image.format);
        const uint8 channelMemorySize = static_cast<uint8>(Sierra::ImageFormatToChannelMemorySize(image.format));

        uint8* pixel = image.memory.data() + (coordinate.y * image.width + coordinate.x) * channelCount * channelMemorySize;
        switch (image.format)
        {
            case Sierra::ImageFormat::R8_Int:
            case Sierra::ImageFormat::R8G8_Int:
            case Sierra::ImageFormat::R8G8B8_Int:
            case Sierra::ImageFormat::R8G8B8A8_Int:
            {
                WritePixel<int8>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R16_Int:
            case Sierra::ImageFormat::R16G16_Int:
            case Sierra::ImageFormat::R16G16B16_Int:
            case Sierra::ImageFormat::R16G16B16A16_Int:
            {
                WritePixel<int16>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R32_Int:
            case Sierra::ImageFormat::R32G32_Int:
            case Sierra::ImageFormat::R32G32B32_Int:
            case Sierra::ImageFormat::R32G32B32A32_Int:
            {
                WritePixel<int32>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R64_Int:
            case Sierra::ImageFormat::R64G64_Int:
            case Sierra::ImageFormat::R64G64B64_Int:
            case Sierra::ImageFormat::R64G64B64A64_Int:
            {
                WritePixel<int64>(pixel, channelCount, color);
                return;
            }

            case Sierra::ImageFormat::R8_UInt:
            case Sierra::ImageFormat::R8G8_UInt:
            case Sierra::ImageFormat::R8G8B8_UInt:
            case Sierra::ImageFormat::R8G8B8A8_UInt:
            {
                WritePixel<uint8>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R16_UInt:
            case Sierra::ImageFormat::R16G16_UInt:
            case Sierra::ImageFormat::R16G16B16_UInt:
            case Sierra::ImageFormat::R16G16B16A16_UInt:
            {
                WritePixel<uint16>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R32_UInt:
            case Sierra::ImageFormat::R32G32_UInt:
            case Sierra::ImageFormat::R32G32B32_UInt:
            case Sierra::ImageFormat::R32G32B32A32_UInt:
            {
                WritePixel<uint32>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R64_UInt:
            case Sierra::ImageFormat::R64G64_UInt:
            case Sierra::ImageFormat::R64G64B64_UInt:
            case Sierra::ImageFormat::R64G64B64A64_UInt:
            {
                WritePixel<uint64>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R8_Norm:
            case Sierra::ImageFormat::R8G8_Norm:
            case Sierra::ImageFormat::R8G8B8_Norm:
            case Sierra::ImageFormat::R8G8B8A8_Norm:
            {
                WritePixel<int8>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R16_Norm:
            case Sierra::ImageFormat::R16G16_Norm:
            case Sierra::ImageFormat::R16G16B16_Norm:
            case Sierra::ImageFormat::R16G16B16A16_Norm:
            {
                WritePixel<int16>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R8_UNorm:
            case Sierra::ImageFormat::R8G8_UNorm:
            case Sierra::ImageFormat::R8G8B8_UNorm:
            case Sierra::ImageFormat::R8G8B8A8_UNorm:
            case Sierra::ImageFormat::B8G8R8A8_UNorm:
            {
                WritePixel<uint8>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R16_UNorm:
            case Sierra::ImageFormat::R16G16_UNorm:
            case Sierra::ImageFormat::R16G16B16_UNorm:
            case Sierra::ImageFormat::R16G16B16A16_UNorm:
            {
                WritePixel<uint16>(pixel, channelCount, color);
                return;
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
                return;
            }
            case Sierra::ImageFormat::R32_Float:
            case Sierra::ImageFormat::R32G32_Float:
            case Sierra::ImageFormat::R32G32B32_Float:
            case Sierra::ImageFormat::R32G32B32A32_Float:
            {
                WritePixel<float32>(pixel, channelCount, color);
                return;
            }
            case Sierra::ImageFormat::R64_Float:
            case Sierra::ImageFormat::R64G64_Float:
            case Sierra::ImageFormat::R64G64B64_Float:
            case Sierra::ImageFormat::R64G64B64A64_Float:
            {
                WritePixel<float64>(pixel, channelCount, color);
                return;
            }
            default:
            {
                break;
            }
        }

        APP_WARNING("Could not set pixel [{0}, {1}] of image, as it is not of a basic format", coordinate.x, coordinate.y);
    }
}
