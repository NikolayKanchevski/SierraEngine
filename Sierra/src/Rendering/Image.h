 //
// Created by Nikolay Kanchevski on 6.12.23.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    enum class ImageType : uint8
    {
        Line,
        Plane,
        Volume,
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

        B8G8R8A8_UNorm,
        B8G8R8A8_SRGB,

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

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API float32 ImageFormatToChannelMemorySize(ImageFormat format);
    [[nodiscard]] SIERRA_API float32 ImageFormatToPixelMemorySize(ImageFormat format);
    [[nodiscard]] SIERRA_API uint8 ImageFormatToChannelCount(ImageFormat format);
    [[nodiscard]] SIERRA_API uint8 ImageFormatToBlockSize(ImageFormat format);

    enum class ImageMemoryLocation : bool
    {
        CPU,
        GPU
    };

    enum class ImageUsage : uint16
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

    enum class ImageComponentSwizzle : uint8
    {
        Identity,
        Zero,
        One,
        Red,
        Green,
        Blue,
        Alpha
    };

    struct ImageCreateInfo
    {
        std::string_view name = "Image";
        uint32 width = 0;
        uint32 height = 1;
        uint32 depth = 1;

        ImageType type = ImageType::Plane;
        ImageFormat format = ImageFormat::Undefined;

        uint32 levelCount = 1;
        uint32 layerCount = 1;
        ImageUsage usage = ImageUsage::Undefined;

        ImageComponentSwizzle redSwizzle = ImageComponentSwizzle::Identity;
        ImageComponentSwizzle greenSwizzle = ImageComponentSwizzle::Identity;
        ImageComponentSwizzle blueSwizzle = ImageComponentSwizzle::Identity;
        ImageComponentSwizzle alphaSwizzle = ImageComponentSwizzle::Identity;

        ImageSampling sampling = ImageSampling::x1;
        ImageMemoryLocation memoryLocation = ImageMemoryLocation::CPU;
    };

    class SIERRA_API Image : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetWidth() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetHeight() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetDepth() const noexcept = 0;

        [[nodiscard]] virtual ImageFormat GetFormat() const noexcept = 0;
        [[nodiscard]] size GetMemorySize() const noexcept { return static_cast<size>(static_cast<float32>(GetWidth() * GetHeight() * GetDepth() * GetLayerCount()) * Sierra::ImageFormatToPixelMemorySize(GetFormat())); }

        [[nodiscard]] virtual uint32 GetLevelCount() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetLayerCount() const noexcept = 0;
        [[nodiscard]] virtual ImageSampling GetSampling() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        /* --- MOVE SEMANTICS --- */
        Image(Image&&) = delete;
        Image& operator=(Image&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Image() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Image(const ImageCreateInfo& createInfo);

    };

}
