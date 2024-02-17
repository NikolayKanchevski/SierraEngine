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

    enum class ImageChannels : uint8
    {
        R,
        RG,
        RGB,
        RGBA,
        BGRA,
        D
    };

    [[nodiscard]] constexpr static uint32 ImageChannelsToCount(const ImageChannels imageChannels)
    {
        switch (imageChannels)
        {
            case ImageChannels::R:
            case ImageChannels::D:          return 1;
            case ImageChannels::RG:         return 2;
            case ImageChannels::RGB:        return 3;
            case ImageChannels::RGBA:
            case ImageChannels::BGRA:       return 4;
        }
    }

    enum class ImageMemoryType : uint8
    {
        Int8,
        UInt8,
        Norm8,
        UNorm8,
        SRGB8,
        Int16,
        UInt16,
        Norm16,
        UNorm16,
        Float16,
        Int32,
        UInt32,
        Float32,
        Int64,
        UInt64,
        Float64
    };

    [[nodiscard]] constexpr static uint32 ImageMemoryTypeToMemorySize(const ImageMemoryType memoryType)
    {
        switch (memoryType)
        {
            case ImageMemoryType::Int8:
            case ImageMemoryType::UInt8:
            case ImageMemoryType::Norm8:
            case ImageMemoryType::UNorm8:
            case ImageMemoryType::SRGB8:         return 1;
            case ImageMemoryType::Int16:
            case ImageMemoryType::UInt16:
            case ImageMemoryType::Norm16:
            case ImageMemoryType::UNorm16:
            case ImageMemoryType::Float16:       return 2;
            case ImageMemoryType::Int32:
            case ImageMemoryType::UInt32:
            case ImageMemoryType::Float32:       return 4;
            case ImageMemoryType::Int64:
            case ImageMemoryType::UInt64:
            case ImageMemoryType::Float64:       return 8;
        }
    }

    struct ImageFormat
    {
        ImageChannels channels = ImageChannels::RGBA;
        ImageMemoryType memoryType = ImageMemoryType::UNorm8;
    };

    enum class ImageMemoryLocation : uint8
    {
        Host,
        Device,
        Auto
    };

    enum class ImageUsage : uint16
    {
        Undefined               = 0x0000,
        SourceMemory            = 0x0001,
        DestinationMemory       = 0x0002,
        Storage                 = 0x0004,
        Sample                  = 0x0008,
        LinearFilter            = 0x0010,
        ColorAttachment         = 0x0020,
        DepthAttachment         = 0x0040,
        InputAttachment         = 0x0080,
        ResolveAttachment       = 0x0100,
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
        const std::string &name = "Image";
        uint32 width = 0;
        uint32 height = 0;

        ImageType type = ImageType::Plane;
        ImageFormat format = { };

        uint32 mipLevelCount = 1;
        uint32 layerCount = 1;
        ImageUsage usage = ImageUsage::Undefined;

        ImageMemoryLocation memoryLocation = ImageMemoryLocation::Auto;
        ImageSampling sampling = ImageSampling::x1;

    };

    class SIERRA_API Image : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetWidth() const { return width; }
        [[nodiscard]] inline uint32 GetHeight() const { return height; }

        [[nodiscard]] inline uint64 GetMemorySize() const { return static_cast<uint64>(width) * height * GetPixelMemorySize(); }
        [[nodiscard]] inline uint32 GetPixelMemorySize() const { return ImageChannelsToCount(format.channels) * ImageMemoryTypeToMemorySize(format.memoryType); }
        [[nodiscard]] inline ImageFormat GetFormat() const { return format; }

        [[nodiscard]] inline uint32 GetMipLevelCount() const { return mipLevelCount; }
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
        ImageFormat format = { };

        uint32 mipLevelCount = 1;
        uint32 layerCount = 1;
        ImageSampling sampling = ImageSampling::x1;

    };

}
