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
        SR_ERROR_IF(!device.IsImageSamplingSupported(createInfo.sampling), "[Metal]: Cannot create image [{0}] with unsupported sampling! Make sure to use Device::IsImageSamplingSupported() to query image sampling support.", GetName());
        SR_ERROR_IF(!device.IsImageFormatSupported(createInfo.format, createInfo.usage), "[Metal]: Cannot create [{0}] image with unsupported format! Use Device::IsImageFormatSupported() to query format support.", GetName());

        // Set up texture descriptor
        MTLTextureDescriptor* const textureDescriptor = [[MTLTextureDescriptor alloc] init];
        [textureDescriptor setTextureType: ImageSettingsToTextureType(createInfo.sampling, createInfo.layerCount)];
        [textureDescriptor setWidth: createInfo.width];
        [textureDescriptor setHeight: createInfo.height];
        [textureDescriptor setDepth: 1];
        [textureDescriptor setMipmapLevelCount: createInfo.mipLevelCount];
        [textureDescriptor setArrayLength: createInfo.layerCount];
        [textureDescriptor setPixelFormat: ImageFormatToPixelFormat(createInfo.format)];
        [textureDescriptor setUsage: ImageUsageToTextureUsage(createInfo.usage)];
        [textureDescriptor setSampleCount: ImageSamplingToUInteger(createInfo.sampling)];
        [textureDescriptor setStorageMode: createInfo.usage & ImageUsage::TransientAttachment ? MTLStorageModeMemoryless : ImageMemoryLocationToStorageMode(createInfo.memoryLocation)];
        [textureDescriptor setCpuCacheMode: ImageMemoryLocationToCPUCacheMode(createInfo.memoryLocation)];
        [textureDescriptor setHazardTrackingMode: MTLHazardTrackingModeUntracked];
        auto a = ImageMemoryLocationToStorageMode(createInfo.memoryLocation);

        // Allocate texture
        texture = [device.GetMetalDevice() newTextureWithDescriptor: textureDescriptor];
        SR_ERROR_IF(texture == nil, "[Metal]: Could not create image!");
        device.SetResourceName(texture, GetName());

        [textureDescriptor release];
    }

    MetalImage::MetalImage(const MetalDevice &device, const SwapchainImageCreateInfo &createInfo)
        : Image({ .name = createInfo.name, .width = createInfo.width, .height = createInfo.height, .format = SwapchainPixelFormatToImageFormat(createInfo.format), .usage = ImageUsage::ColorAttachment, .memoryLocation = ImageMemoryLocation::Device }), MetalResource(createInfo.name),
          texture(createInfo.texture), swapchainImage(true)
    {

    }

    /* --- DESTRUCTOR --- */

    MetalImage::~MetalImage()
    {
        if (!swapchainImage) [texture release];
    }

    /* --- PRIVATE METHODS --- */

    ImageFormat MetalImage::SwapchainPixelFormatToImageFormat(const MTLPixelFormat format)
    {
        switch (format)
        {
            case MTLPixelFormatBGRA8Unorm:        return { .channels = ImageChannels::BGRA, .memoryType = ImageMemoryType::UNorm8 };
            case MTLPixelFormatBGRA8Unorm_sRGB:   return { .channels = ImageChannels::BGRA, .memoryType = ImageMemoryType::SRGB8 };
            case MTLPixelFormatRGBA16Float:       return { .channels = ImageChannels::BGRA, .memoryType = ImageMemoryType::Float16 };
            default:                                break;
        }

        SR_ERROR("[Metal]: Cannot determine image format of invalid swapchain MTLPixelFormat!");
        return { };
    }

    /* --- CONVERSIONS --- */

    MTLTextureType MetalImage::ImageSettingsToTextureType(const ImageSampling sampling, const uint32 layerCount)
    {
        if (sampling == ImageSampling::x1 && layerCount == 1) return MTLTextureType2D;
        if (sampling != ImageSampling::x1 && layerCount == 1) return MTLTextureType2DMultisample;
        if (sampling == ImageSampling::x1 && layerCount > 1)  return MTLTextureType2DArray;
        if (sampling != ImageSampling::x1 && layerCount > 1)  return MTLTextureType2DMultisampleArray;

        return MTLTextureType2D;
    }

    MTLPixelFormat MetalImage::ImageFormatToPixelFormat(const ImageFormat format)
    {
        switch (format.channels)
        {
            case ImageChannels::R:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return MTLPixelFormatR8Sint;
                    case ImageMemoryType::UInt8:        return MTLPixelFormatR8Uint;
                    case ImageMemoryType::Norm8:        return MTLPixelFormatR8Snorm;
                    case ImageMemoryType::UNorm8:       return MTLPixelFormatR8Unorm;
                    case ImageMemoryType::SRGB8:        return MTLPixelFormatR8Unorm_sRGB;
                    case ImageMemoryType::Int16:        return MTLPixelFormatR16Sint;
                    case ImageMemoryType::UInt16:       return MTLPixelFormatR16Uint;
                    case ImageMemoryType::Norm16:       return MTLPixelFormatR16Snorm;
                    case ImageMemoryType::UNorm16:      return MTLPixelFormatR16Unorm;
                    case ImageMemoryType::Float16:      return MTLPixelFormatR16Float;
                    case ImageMemoryType::Int32:
                    case ImageMemoryType::Int64:        return MTLPixelFormatR32Sint;
                    case ImageMemoryType::UInt32:
                    case ImageMemoryType::UInt64:       return MTLPixelFormatR32Uint;
                    case ImageMemoryType::Float32:
                    case ImageMemoryType::Float64:      return MTLPixelFormatR32Float;
                }
            }
            case ImageChannels::RG:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return MTLPixelFormatRG8Sint;
                    case ImageMemoryType::UInt8:        return MTLPixelFormatRG8Uint;
                    case ImageMemoryType::Norm8:        return MTLPixelFormatRG8Snorm;
                    case ImageMemoryType::UNorm8:       return MTLPixelFormatRG8Unorm;
                    case ImageMemoryType::SRGB8:        return MTLPixelFormatRG8Unorm_sRGB;
                    case ImageMemoryType::Int16:        return MTLPixelFormatRG16Sint;
                    case ImageMemoryType::UInt16:       return MTLPixelFormatRG16Uint;
                    case ImageMemoryType::Norm16:       return MTLPixelFormatRG16Snorm;
                    case ImageMemoryType::UNorm16:      return MTLPixelFormatRG16Unorm;
                    case ImageMemoryType::Float16:      return MTLPixelFormatRG16Float;
                    case ImageMemoryType::Int32:
                    case ImageMemoryType::Int64:        return MTLPixelFormatRG32Sint;
                    case ImageMemoryType::UInt32:
                    case ImageMemoryType::UInt64:       return MTLPixelFormatRG32Uint;
                    case ImageMemoryType::Float32:
                    case ImageMemoryType::Float64:      return MTLPixelFormatRG32Float;
                }
            }
            case ImageChannels::RGB:
            case ImageChannels::RGBA:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::Int8:         return MTLPixelFormatRGBA8Sint;
                    case ImageMemoryType::UInt8:        return MTLPixelFormatRGBA8Uint;
                    case ImageMemoryType::Norm8:        return MTLPixelFormatRGBA8Snorm;
                    case ImageMemoryType::UNorm8:       return MTLPixelFormatRGBA8Unorm;
                    case ImageMemoryType::SRGB8:        return MTLPixelFormatRGBA8Unorm_sRGB;
                    case ImageMemoryType::Int16:        return MTLPixelFormatRGBA16Sint;
                    case ImageMemoryType::UInt16:       return MTLPixelFormatRGBA16Uint;
                    case ImageMemoryType::Norm16:       return MTLPixelFormatRGBA16Snorm;
                    case ImageMemoryType::UNorm16:      return MTLPixelFormatRGBA16Unorm;
                    case ImageMemoryType::Float16:      return MTLPixelFormatRGBA16Float;
                    case ImageMemoryType::Int32:
                    case ImageMemoryType::Int64:        return MTLPixelFormatRGBA32Sint;
                    case ImageMemoryType::UInt32:
                    case ImageMemoryType::UInt64:       return MTLPixelFormatRGBA32Uint;
                    case ImageMemoryType::Float32:
                    case ImageMemoryType::Float64:      return MTLPixelFormatRGBA32Float;
                }
            }
            case ImageChannels::BGRA:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::UNorm8:    return MTLPixelFormatBGRA8Unorm;
                    case ImageMemoryType::SRGB8:     return MTLPixelFormatBGRA8Unorm_sRGB;
                    default:                         break;
                }
            }
            case ImageChannels::D:
            {
                switch (format.memoryType)
                {
                    case ImageMemoryType::UNorm16:   return MTLPixelFormatDepth16Unorm;
                    case ImageMemoryType::Float32:   return MTLPixelFormatDepth32Float;
                    default:                         break;
                }
            }
        }

        SR_ERROR("[Metal]: Cannot determine image format of invalid channel and memory configuration!");
        return MTLPixelFormatInvalid;
    }

    MTLTextureUsage MetalImage::ImageUsageToTextureUsage(const ImageUsage usage)
    {
        MTLTextureUsage usageFlags = MTLTextureUsageUnknown;
        if (usage & ImageUsage::SourceMemory) usageFlags |= MTLTextureUsageUnknown;
        if (usage & ImageUsage::DestinationMemory) usageFlags |= MTLTextureUsageUnknown;
        if (usage & ImageUsage::Storage)                    usageFlags |= MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
        if (usage & ImageUsage::Sample) usageFlags |= MTLTextureUsageShaderRead;
        if (usage & ImageUsage::ColorAttachment ||
            usage & ImageUsage::DepthAttachment ||
            usage & ImageUsage::InputAttachment ||
            usage & ImageUsage::TransientAttachment)        usageFlags |= MTLTextureUsageRenderTarget;
        return usageFlags;
    }

    NSUInteger MetalImage::ImageSamplingToUInteger(const ImageSampling sampling)
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

    MTLStorageMode MetalImage::ImageMemoryLocationToStorageMode(const ImageMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::Host:        return MTLStorageModeShared;
            case ImageMemoryLocation::Device:      return MTLStorageModePrivate;
            case ImageMemoryLocation::Auto:        return MTLStorageModeShared;
        }

        return MTLStorageModeShared;
    }

    MTLCPUCacheMode MetalImage::ImageMemoryLocationToCPUCacheMode(const ImageMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::Host:        return MTLCPUCacheModeDefaultCache;
            case ImageMemoryLocation::Device:      return MTLCPUCacheModeWriteCombined;
            case ImageMemoryLocation::Auto:        return MTLCPUCacheModeDefaultCache;
        }

        return MTLCPUCacheModeDefaultCache;
    }

}
