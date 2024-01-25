//
// Created by Nikolay Kanchevski on 7.12.23.
//

#include "MetalImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalImage::MetalImage(const MetalDevice &device, const ImageCreateInfo &createInfo)
        : Image(createInfo), MetalResource(createInfo.name)
    {
        SR_ERROR_IF(!device.IsImageSamplingSupported(createInfo.sampling), "[Metal]: Cannot create image [{0}] with unsupported sampling! Make sure to use Device::IsImageSamplingSupported() to query image sampling support.");
        SR_ERROR_IF(!device.IsImageConfigurationSupported(createInfo.format, createInfo.usage), "[Metal]: Cannot create image with unsupported format! Use Device::IsImageConfigurationSupported() to query format support.");

        // Set up texture descriptor
        MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
        textureDescriptor->setTextureType(ImageSettingsToTextureType(GetSampling(), createInfo.layerCount));
        textureDescriptor->setWidth(GetWidth());
        textureDescriptor->setHeight(GetHeight());
        textureDescriptor->setDepth(1);
        textureDescriptor->setMipmapLevelCount(GetMipLevelCount());
        textureDescriptor->setArrayLength(createInfo.layerCount);
        textureDescriptor->setPixelFormat(ImageFormatToPixelFormat(createInfo.format));
        textureDescriptor->setUsage(ImageUsageToTextureUsage(createInfo.usage));
        textureDescriptor->setSampleCount(ImageSamplingToUInteger(createInfo.sampling));
        #if SR_PLATFORM_macOS
            textureDescriptor->setStorageMode(ImageMemoryLocationToStorageMode(createInfo.memoryLocation));
        #else
            textureDescriptor->setStorageMode(createInfo.usage & ImageUsage::TransientAttachment ? MTL::StorageModeMemoryless : ImageMemoryLocationToStorageMode(createInfo.memoryLocation));
        #endif
        textureDescriptor->setCpuCacheMode(ImageMemoryLocationToCPUCacheMode(createInfo.memoryLocation));
        textureDescriptor->setHazardTrackingMode(MTL::HazardTrackingModeUntracked);

        // Allocate texture
        texture = device.GetMetalDevice()->newTexture(textureDescriptor);
        SR_ERROR_IF(texture == nullptr, "[Metal]: Could not create image!");
        device.SetResourceName(texture, GetName());

        textureDescriptor->release();
    }

    MetalImage::MetalImage(const MetalDevice &device, const SwapchainImageCreateInfo &createInfo)
        : Image({ .name = createInfo.name, .width = createInfo.width, .height = createInfo.height, .format = SwapchainPixelFormatToImageFormat(createInfo.format), .memoryLocation = ImageMemoryLocation::Device, .usage = ImageUsage::ColorAttachment }), MetalResource(createInfo.name),
          texture(createInfo.texture), swapchainImage(true)
    {
        device.SetResourceName(texture, GetName());
    }

    /* --- DESTRUCTOR --- */

    MetalImage::~MetalImage()
    {
        if (!swapchainImage) texture->release();
    }

    /* --- PRIVATE METHODS --- */

    ImageFormat MetalImage::SwapchainPixelFormatToImageFormat(const MTL::PixelFormat format)
    {
        switch (format)
        {
            case MTL::PixelFormatBGRA8Unorm:        return { .channels = ImageChannels::BGRA, .memoryType = ImageMemoryType::UNorm8 };
            case MTL::PixelFormatBGRA8Unorm_sRGB:   return { .channels = ImageChannels::BGRA, .memoryType = ImageMemoryType::SRGB8 };
            case MTL::PixelFormatRGBA16Float:       return { .channels = ImageChannels::BGRA, .memoryType = ImageMemoryType::Float16 };
            default:                                break;
        }

        SR_ERROR("[Metal]: Cannot determine image format of invalid swapchain MTLPixelFormat!");
        return { };
    }

    /* --- CONVERSIONS --- */

    MTL::TextureType MetalImage::ImageSettingsToTextureType(const ImageSampling sampling, const uint32 layerCount)
    {
        if (sampling == ImageSampling::x1 && layerCount == 1) return MTL::TextureType::TextureType2D;
        if (sampling == ImageSampling::x1 && layerCount > 1) return MTL::TextureType::TextureType2DArray;
        if (sampling != ImageSampling::x1 && layerCount > 1) return MTL::TextureType::TextureType2DMultisampleArray;

        return MTL::TextureType::TextureType2D;
    }

    MTL::PixelFormat MetalImage::ImageFormatToPixelFormat(const ImageFormat format)
    {
        switch (format.channels)
        {
            case ImageChannels::R:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return MTL::PixelFormatR8Sint;
                    case ImageMemoryType::UInt8:        return MTL::PixelFormatR8Uint;
                    case ImageMemoryType::UNorm8:       return MTL::PixelFormatR8Unorm;
                    case ImageMemoryType::SRGB8:        return MTL::PixelFormatR8Unorm_sRGB;
                    case ImageMemoryType::Int16:        return MTL::PixelFormatR16Sint;
                    case ImageMemoryType::UInt16:       return MTL::PixelFormatR16Uint;
                    case ImageMemoryType::Norm16:       return MTL::PixelFormatR16Snorm;
                    case ImageMemoryType::UNorm16:      return MTL::PixelFormatR16Unorm;
                    case ImageMemoryType::Float16:      return MTL::PixelFormatR16Float;
                    case ImageMemoryType::Int32:
                    case ImageMemoryType::Int64:        return MTL::PixelFormatR32Sint;
                    case ImageMemoryType::UInt32:
                    case ImageMemoryType::UInt64:       return MTL::PixelFormatR32Uint;
                    case ImageMemoryType::Float32:
                    case ImageMemoryType::Float64:      return MTL::PixelFormatR32Float;
                }
            }
            case ImageChannels::RG:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return MTL::PixelFormatRG8Sint;
                    case ImageMemoryType::UInt8:        return MTL::PixelFormatRG8Uint;
                    case ImageMemoryType::UNorm8:       return MTL::PixelFormatRG8Unorm;
                    case ImageMemoryType::SRGB8:        return MTL::PixelFormatRG8Unorm_sRGB;
                    case ImageMemoryType::Int16:        return MTL::PixelFormatRG16Sint;
                    case ImageMemoryType::UInt16:       return MTL::PixelFormatRG16Uint;
                    case ImageMemoryType::Norm16:       return MTL::PixelFormatRG16Snorm;
                    case ImageMemoryType::UNorm16:      return MTL::PixelFormatRG16Unorm;
                    case ImageMemoryType::Float16:      return MTL::PixelFormatRG16Float;
                    case ImageMemoryType::Int32:
                    case ImageMemoryType::Int64:        return MTL::PixelFormatRG32Sint;
                    case ImageMemoryType::UInt32:
                    case ImageMemoryType::UInt64:       return MTL::PixelFormatRG32Uint;
                    case ImageMemoryType::Float32:
                    case ImageMemoryType::Float64:      return MTL::PixelFormatRG32Float;
                }
            }
            case ImageChannels::RGB:
            case ImageChannels::RGBA:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return MTL::PixelFormatRGBA8Sint;
                    case ImageMemoryType::UInt8:        return MTL::PixelFormatRGBA8Uint;
                    case ImageMemoryType::UNorm8:       return MTL::PixelFormatRGBA8Unorm;
                    case ImageMemoryType::SRGB8:        return MTL::PixelFormatRGBA8Unorm_sRGB;
                    case ImageMemoryType::Int16:        return MTL::PixelFormatRGBA16Sint;
                    case ImageMemoryType::UInt16:       return MTL::PixelFormatRGBA16Uint;
                    case ImageMemoryType::Norm16:       return MTL::PixelFormatRGBA16Snorm;
                    case ImageMemoryType::UNorm16:      return MTL::PixelFormatRGBA16Unorm;
                    case ImageMemoryType::Float16:      return MTL::PixelFormatRGBA16Float;
                    case ImageMemoryType::Int32:
                    case ImageMemoryType::Int64:        return MTL::PixelFormatRGBA32Sint;
                    case ImageMemoryType::UInt32:
                    case ImageMemoryType::UInt64:       return MTL::PixelFormatRGBA32Uint;
                    case ImageMemoryType::Float32:
                    case ImageMemoryType::Float64:      return MTL::PixelFormatRGBA32Float;
                }
            }
            case ImageChannels::BGRA:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::UNorm8:    return MTL::PixelFormatBGRA8Unorm;
                    case ImageMemoryType::SRGB8:     return MTL::PixelFormatBGRA8Unorm_sRGB;
                    default:                         break;
                }
            }
            case ImageChannels::D:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::UNorm16:   return MTL::PixelFormatDepth16Unorm;
                    case ImageMemoryType::Float32:   return MTL::PixelFormatDepth32Float;
                    default:                         break;
                }
            }
        }

        SR_ERROR("[Metal]: Cannot determine image format of invalid channel and memory configuration!");
        return MTL::PixelFormatInvalid;
    }

    MTL::TextureUsage MetalImage::ImageUsageToTextureUsage(const ImageUsage usage)
    {
        MTL::TextureUsage usageFlags = MTL::TextureUsageUnknown;
        if (usage & ImageUsage::SourceTransfer)             usageFlags |= MTL::TextureUsageUnknown;
        if (usage & ImageUsage::DestinationTransfer)        usageFlags |= MTL::TextureUsageUnknown;
        if (usage & ImageUsage::Storage)                    usageFlags |= MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite;
        if (usage & ImageUsage::Sampled)                    usageFlags |= MTL::TextureUsageShaderRead;
        if (usage & ImageUsage::ColorAttachment ||
            usage & ImageUsage::DepthAttachment ||
            usage & ImageUsage::InputAttachment ||
            usage & ImageUsage::TransientAttachment)        usageFlags |= MTL::TextureUsageRenderTarget;
        return usageFlags;
    }

    NS::UInteger MetalImage::ImageSamplingToUInteger(const ImageSampling sampling)
    {
        switch (sampling)
        {
            case ImageSampling::x1:         return 1;
            case ImageSampling::x2:         return 2;
            case ImageSampling::x4:         return 4;
            case ImageSampling::x8:         return 8;
            case ImageSampling::x16:        return 16;
            case ImageSampling::x32:        return 32;
            case ImageSampling::x64:        return 64;
        }

        return 1;
    }

    MTL::StorageMode MetalImage::ImageMemoryLocationToStorageMode(const ImageMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::Host:        return MTL::StorageModeManaged;
            case ImageMemoryLocation::Device:      return MTL::StorageModePrivate;
            case ImageMemoryLocation::Auto:        return MTL::StorageModeManaged;
        }

        return MTL::StorageModeManaged;
    }

    MTL::CPUCacheMode MetalImage::ImageMemoryLocationToCPUCacheMode(const ImageMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::Host:        return MTL::CPUCacheModeDefaultCache;
            case ImageMemoryLocation::Device:      return MTL::CPUCacheModeWriteCombined;
            case ImageMemoryLocation::Auto:        return MTL::CPUCacheModeDefaultCache;
        }

        return MTL::CPUCacheModeDefaultCache;
    }

}