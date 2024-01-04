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
        BGRA
    };

    enum class ImageMemoryType : uint8
    {
        Int8,
        UInt8,
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

    enum class ImageMemoryLocation : uint8
    {
        Host,
        Device,
        Auto
    };

    enum class ImageUsage : uint16
    {
        Undefined                   = 0x0000,
        SourceTransfer              = 0x0001,
        DestinationTransfer         = 0x0002,
        Storage                     = 0x0004,
        Sampled                     = 0x0008,
        Filtered                    = 0x0010,
        ColorAttachment             = 0x0020,
        DepthAttachment             = 0x0040,
        InputAttachment             = 0x0080,
        ResolveAttachment           = 0x0100,
        TransientAttachment         = 0x0200
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

    struct ImageFormat
    {
        ImageChannels channels = ImageChannels::RGBA;
        ImageMemoryType memoryType = ImageMemoryType::UNorm16;
    };

    struct ImageCreateInfo
    {
        const std::string &name = "Image";
        uint32 width = 0;
        uint32 height = 0;

        ImageType type = ImageType::Plane;
        uint32 layerCount = 1;
        bool enableMipMapping = false;

        ImageFormat format = { };
        ImageMemoryLocation memoryLocation = ImageMemoryLocation::Auto;

        ImageUsage usage = ImageUsage::Undefined;
        ImageSampling sampling = ImageSampling::x1;
    };

    class SIERRA_API Image : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetWidth() const { return width; }
        [[nodiscard]] inline uint32 GetHeight() const { return height; }
        [[nodiscard]] inline uint64 GetMemorySize() const { return memorySize; }

        [[nodiscard]] inline uint32 GetMipLevels() const { return mipLevels; }
        [[nodiscard]] inline uint32 GetLayerCount() const { return layerCount; }

        [[nodiscard]] inline ImageSampling GetSampling() const { return sampling; };
        [[nodiscard]] inline ImageFormat GetFormat() const { return format; }
        [[nodiscard]] inline ImageUsage GetUsage() const { return usage; }

    protected:
        explicit Image(const ImageCreateInfo &createInfo);

    private:
        uint32 width = 0;
        uint32 height = 0;
        uint64 memorySize = 0;

        uint32 mipLevels = 1;
        uint32 layerCount = 1;

        ImageSampling sampling = ImageSampling::x1;
        ImageFormat format = { };
        ImageUsage usage = ImageUsage::Undefined;

    };

}
