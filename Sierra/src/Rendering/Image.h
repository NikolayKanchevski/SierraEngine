//
// Created by Nikolay Kanchevski on 6.12.23.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    enum class ImageType : bool
    {
        Plane,
        Cube
    };

    enum class ImageFormat : uint16
    {
        Undefined,

        R8_Int,
        R8_UInt,
        R8_Norm,
        R8_UNorm,
        R8_SRGB,
        R8G8_Int,
        R8G8_UInt,
        R8G8_Norm,
        R8G8_UNorm,
        R8G8_SRGB,
        R8G8B8_Int,
        R8G8B8_UInt,
        R8G8B8_Norm,
        R8G8B8_UNorm,
        R8G8B8_SRGB,
        R8G8B8A8_Int,
        R8G8B8A8_UInt,
        R8G8B8A8_Norm,
        R8G8B8A8_UNorm,
        R8G8B8A8_SRGB,

        R16_Int,
        R16_UInt,
        R16_Norm,
        R16_UNorm,
        R16_Float,
        R16G16_Int,
        R16G16_UInt,
        R16G16_Norm,
        R16G16_UNorm,
        R16G16_Float,
        R16G16B16_Int,
        R16G16B16_UInt,
        R16G16B16_Norm,
        R16G16B16_UNorm,
        R16G16B16_Float,
        R16G16B16A16_Int,
        R16G16B16A16_UInt,
        R16G16B16A16_Norm,
        R16G16B16A16_UNorm,
        R16G16B16A16_Float,

        R32_Int,
        R32_UInt,
        R32_Float,
        R32G32_Int,
        R32G32_UInt,
        R32G32_Float,
        R32G32B32_Int,
        R32G32B32_UInt,
        R32G32B32_Float,
        R32G32B32A32_Int,
        R32G32B32A32_UInt,
        R32G32B32A32_Float,

        R64_Int,
        R64_UInt,
        R64_Float,
        R64G64_Int,
        R64G64_UInt,
        R64G64_Float,
        R64G64B64_Int,
        R64G64B64_UInt,
        R64G64B64_Float,
        R64G64B64A64_Int,
        R64G64B64A64_UInt,
        R64G64B64A64_Float,

        D16_UNorm,
        D32_Float,

        B8G8R8A8_UNorm,
        B8G8R8A8_SRGB,

        BC1_RGB_UNorm,
        BC1_RGB_SRGB,
        BC1_RGBA_UNorm,
        BC1_RGBA_SRGB,
        BC3_RGBA_UNorm,
        BC3_RGBA_SRGB,
        BC4_R_Norm,
        BC4_R_UNorm,
        BC5_RG_Norm,
        BC5_RG_UNorm,
        BC6_HDR_RGB_Float,
        BC6_HDR_RGB_UFloat,
        BC7_RGB_UNorm,
        BC7_RGB_SRGB,
        BC7_RGBA_UNorm,
        BC7_RGBA_SRGB,

        ASTC_4x4_UNorm,
        ASTC_4x4_SRGB,
        ASTC_8x8_UNorm,
        ASTC_8x8_SRGB
    };

    [[nodiscard]] constexpr uint8 ImageFormatToChannelCount(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined:                        break;

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
            case ImageFormat::R64_Float:

            case ImageFormat::D16_UNorm:
            case ImageFormat::D32_Float:

            case ImageFormat::BC4_R_UNorm:
            case ImageFormat::BC4_R_Norm:

            case ImageFormat::ASTC_4x4_UNorm:
            case ImageFormat::ASTC_4x4_SRGB:
            case ImageFormat::ASTC_8x8_UNorm:
            case ImageFormat::ASTC_8x8_SRGB:               return 1;

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
            case ImageFormat::R64G64_Float:

            case ImageFormat::BC5_RG_Norm:
            case ImageFormat::BC5_RG_UNorm:                 return 2;

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
            case ImageFormat::R64G64B64_Float:

            case ImageFormat::BC6_HDR_RGB_Float:
            case ImageFormat::BC6_HDR_RGB_UFloat:           return 3;

            case ImageFormat::R8G8B8A8_Int:
            case ImageFormat::R8G8B8A8_UInt:
            case ImageFormat::R8G8B8A8_Norm:
            case ImageFormat::R8G8B8A8_UNorm:
            case ImageFormat::R8G8B8A8_SRGB:
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
            case ImageFormat::R64G64B64A64_Float:

            case ImageFormat::B8G8R8A8_UNorm:
            case ImageFormat::B8G8R8A8_SRGB:

            case ImageFormat::BC1_RGBA_UNorm:
            case ImageFormat::BC1_RGBA_SRGB:
            case ImageFormat::BC1_RGB_UNorm:
            case ImageFormat::BC1_RGB_SRGB:
            case ImageFormat::BC3_RGBA_UNorm:
            case ImageFormat::BC3_RGBA_SRGB:
            case ImageFormat::BC7_RGB_UNorm:
            case ImageFormat::BC7_RGB_SRGB:
            case ImageFormat::BC7_RGBA_UNorm:
            case ImageFormat::BC7_RGBA_SRGB:            return 4;
        }

        return 0;
    }

    [[nodiscard]] constexpr float32 ImageFormatToPixelMemorySize(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined:                        break;

            case ImageFormat::R8_Int:
            case ImageFormat::R8_UInt:
            case ImageFormat::R8_Norm:
            case ImageFormat::R8_UNorm:
            case ImageFormat::R8_SRGB:                          return 1 * 1;
            case ImageFormat::R8G8_Int:
            case ImageFormat::R8G8_UInt:
            case ImageFormat::R8G8_Norm:
            case ImageFormat::R8G8_UNorm:
            case ImageFormat::R8G8_SRGB:                        return 2 * 1;
            case ImageFormat::R8G8B8_Int:
            case ImageFormat::R8G8B8_UInt:
            case ImageFormat::R8G8B8_Norm:
            case ImageFormat::R8G8B8_UNorm:
            case ImageFormat::R8G8B8_SRGB:                      return 3 * 1;
            case ImageFormat::R8G8B8A8_Int:
            case ImageFormat::R8G8B8A8_UInt:
            case ImageFormat::R8G8B8A8_Norm:
            case ImageFormat::R8G8B8A8_UNorm:
            case ImageFormat::R8G8B8A8_SRGB:
            case ImageFormat::B8G8R8A8_UNorm:
            case ImageFormat::B8G8R8A8_SRGB:                    return 4 * 1;

            case ImageFormat::R16_Int:
            case ImageFormat::R16_UInt:
            case ImageFormat::R16_Norm:
            case ImageFormat::R16_UNorm:
            case ImageFormat::R16_Float:
            case ImageFormat::D16_UNorm:                        return 1 * 2;
            case ImageFormat::R16G16_Int:
            case ImageFormat::R16G16_UInt:
            case ImageFormat::R16G16_Norm:
            case ImageFormat::R16G16_UNorm:
            case ImageFormat::R16G16_Float:                     return 2 * 2;
            case ImageFormat::R16G16B16_Int:
            case ImageFormat::R16G16B16_UInt:
            case ImageFormat::R16G16B16_Norm:
            case ImageFormat::R16G16B16_UNorm:
            case ImageFormat::R16G16B16_Float:                  return 3 * 2;
            case ImageFormat::R16G16B16A16_Int:
            case ImageFormat::R16G16B16A16_UInt:
            case ImageFormat::R16G16B16A16_Norm:
            case ImageFormat::R16G16B16A16_UNorm:
            case ImageFormat::R16G16B16A16_Float:               return 4 * 2;

            case ImageFormat::R32_Int:
            case ImageFormat::R32_UInt:
            case ImageFormat::R32_Float:
            case ImageFormat::D32_Float:                        return 1 * 4;
            case ImageFormat::R32G32_Int:
            case ImageFormat::R32G32_UInt:
            case ImageFormat::R32G32_Float:                     return 2 * 4;
            case ImageFormat::R32G32B32_Int:
            case ImageFormat::R32G32B32_UInt:
            case ImageFormat::R32G32B32_Float:                  return 3 * 4;
            case ImageFormat::R32G32B32A32_Int:
            case ImageFormat::R32G32B32A32_UInt:
            case ImageFormat::R32G32B32A32_Float:               return 4 * 4;

            case ImageFormat::R64_Int:
            case ImageFormat::R64_UInt:
            case ImageFormat::R64_Float:                        return 1 * 8;
            case ImageFormat::R64G64_Int:
            case ImageFormat::R64G64_UInt:
            case ImageFormat::R64G64_Float:                     return 2 * 8;
            case ImageFormat::R64G64B64_Int:
            case ImageFormat::R64G64B64_UInt:
            case ImageFormat::R64G64B64_Float:                  return 3 * 8;
            case ImageFormat::R64G64B64A64_Int:
            case ImageFormat::R64G64B64A64_UInt:
            case ImageFormat::R64G64B64A64_Float:               return 4 * 8;

            case ImageFormat::BC1_RGB_UNorm:
            case ImageFormat::BC1_RGB_SRGB:
            case ImageFormat::BC1_RGBA_UNorm:
            case ImageFormat::BC1_RGBA_SRGB:
            case ImageFormat::BC4_R_Norm:
            case ImageFormat::BC4_R_UNorm:                      return 0.5;
            case ImageFormat::BC3_RGBA_UNorm:
            case ImageFormat::BC3_RGBA_SRGB:
            case ImageFormat::BC5_RG_Norm:
            case ImageFormat::BC5_RG_UNorm:
            case ImageFormat::BC6_HDR_RGB_Float:
            case ImageFormat::BC6_HDR_RGB_UFloat:
            case ImageFormat::BC7_RGB_UNorm:
            case ImageFormat::BC7_RGB_SRGB:
            case ImageFormat::BC7_RGBA_UNorm:
            case ImageFormat::BC7_RGBA_SRGB:                    return 1;

            case ImageFormat::ASTC_4x4_UNorm:
            case ImageFormat::ASTC_4x4_SRGB:                    return 1;
            case ImageFormat::ASTC_8x8_UNorm:
            case ImageFormat::ASTC_8x8_SRGB:                    return 2;
        }

        return 0;
    }
    
    [[nodiscard]] constexpr uint8 ImageFormatToBlockSize(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined:                        return 0;

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
            case ImageFormat::BC7_RGBA_SRGB:                    return 4;

            case ImageFormat::ASTC_4x4_UNorm:
            case ImageFormat::ASTC_4x4_SRGB:                    return 4;
            case ImageFormat::ASTC_8x8_UNorm:
            case ImageFormat::ASTC_8x8_SRGB:                    return 8;

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
            case ImageFormat::D32_Float:
            case ImageFormat::B8G8R8A8_UNorm:
            case ImageFormat::B8G8R8A8_SRGB:                    return 1;
        }
    }

    enum class ImageMemoryLocation : bool
    {
        CPU,
        GPU
    };

    enum class ImageUsage
    {
        Undefined               = 0x0000,
        SourceMemory            = 0x0001,
        DestinationMemory       = 0x0002,
        Storage                 = 0x0004,
        Sample                  = 0x0008,
        Filter                  = 0x0010,
        ColorAttachment         = 0x0020,
        DepthAttachment         = 0x0040,
        InputAttachment         = 0x0080,
        ResolverAttachment      = 0x0100,
        TransientAttachment     = 0x0200
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(ImageUsage);

    enum class ImageSampling : uint8
    {
        x1,
        x2,
        x4,
        x8,
        x16,
        x32,
        x64
    };

    struct ImageCreateInfo
    {
        std::string_view name = "Image";
        uint32 width = 0;
        uint32 height = 0;

        ImageType type = ImageType::Plane;
        ImageFormat format = ImageFormat::Undefined;

        uint32 levelCount = 1;
        uint32 layerCount = 1;
        ImageUsage usage = ImageUsage::Undefined;

        ImageMemoryLocation memoryLocation = ImageMemoryLocation::CPU;
        ImageSampling sampling = ImageSampling::x1;
    };

    class SIERRA_API Image : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetWidth() const { return width; }
        [[nodiscard]] inline uint32 GetHeight() const { return height; }
        [[nodiscard]] inline ImageFormat GetFormat() const { return format; }

        [[nodiscard]] inline float32 GetPixelMemorySize() const { return ImageFormatToPixelMemorySize(format); }
        [[nodiscard]] inline uint64 GetLayerMemorySize() const { return static_cast<uint64>(width) * height * GetPixelMemorySize(); }
        [[nodiscard]] inline uint64 GetMemorySize() const { return GetLayerMemorySize() * layerCount; }

        [[nodiscard]] inline uint32 GetLevelCount() const { return levelCount; }
        [[nodiscard]] inline uint32 GetLayerCount() const { return layerCount; }
        [[nodiscard]] inline ImageSampling GetSampling() const { return sampling; };

        /* --- OPERATORS --- */
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Image() = default;

    protected:
        explicit Image(const ImageCreateInfo &createInfo);

    private:
        uint32 width = 0;
        uint32 height = 0;
        ImageFormat format = ImageFormat::Undefined;

        uint32 levelCount = 1;
        uint32 layerCount = 1;
        ImageSampling sampling = ImageSampling::x1;

    };

}
