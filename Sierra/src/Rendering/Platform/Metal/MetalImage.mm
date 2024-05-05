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
        SR_ERROR_IF(!device.IsImageSamplingSupported(createInfo.sampling), "[Metal]: Cannot create image [{0}] with unsupported sampling! Use Device::IsImageSamplingSupported() to query image sampling support.", GetName());
        SR_ERROR_IF(!device.IsImageFormatSupported(createInfo.format, createInfo.usage), "[Metal]: Cannot create [{0}] image with unsupported format! Use Device::IsImageFormatSupported() to query format support.", GetName());

        // Set up texture descriptor
        MTLTextureDescriptor* const textureDescriptor = [[MTLTextureDescriptor alloc] init];
        [textureDescriptor setTextureType: ImageSettingsToTextureType(createInfo.type, createInfo.layerCount, createInfo.sampling)];
        [textureDescriptor setWidth: createInfo.width];
        [textureDescriptor setHeight: createInfo.height];
        [textureDescriptor setDepth: createInfo.depth];
        [textureDescriptor setMipmapLevelCount: createInfo.levelCount];
        [textureDescriptor setArrayLength: createInfo.type != ImageType::Cube ? createInfo.layerCount : (createInfo.layerCount / 6)];
        [textureDescriptor setPixelFormat: ImageFormatToPixelFormat(createInfo.format)];
        [textureDescriptor setUsage: ImageUsageToTextureUsage(createInfo.usage)];
        [textureDescriptor setSampleCount: ImageSamplingToUInteger(createInfo.sampling)];
        [textureDescriptor setStorageMode: createInfo.usage & ImageUsage::TransientAttachment ? MTLStorageModeMemoryless : ImageMemoryLocationToStorageMode(createInfo.memoryLocation)];
        [textureDescriptor setCpuCacheMode: ImageMemoryLocationToCPUCacheMode(createInfo.memoryLocation)];
        [textureDescriptor setHazardTrackingMode: MTLHazardTrackingModeUntracked];

        // Allocate texture
        texture = [device.GetMetalDevice() newTextureWithDescriptor: textureDescriptor];
        SR_ERROR_IF(texture == nil, "[Metal]: Could not create image!");
        device.SetResourceName(texture, GetName());

        [textureDescriptor release];
    }

    MetalImage::MetalImage(const MetalDevice &device, const SwapchainImageCreateInfo &createInfo)
        : Image({ .name = createInfo.name, .width = createInfo.width, .height = createInfo.height, .format = SwapchainPixelFormatToImageFormat(createInfo.format), .usage = ImageUsage::ColorAttachment, .memoryLocation = ImageMemoryLocation::GPU }), MetalResource(createInfo.name),
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
            case MTLPixelFormatBGRA8Unorm:          return ImageFormat::B8G8R8A8_UNorm;
            case MTLPixelFormatBGRA8Unorm_sRGB:     return ImageFormat::B8G8R8A8_SRGB;
            case MTLPixelFormatRGBA16Float:         return ImageFormat::R16G16B16A16_Float;
            default:                                break;
        }

        SR_ERROR("[Metal]: Cannot determine image format of invalid swapchain MTLPixelFormat!");
        return { };
    }

    /* --- CONVERSIONS --- */

    MTLTextureType MetalImage::ImageSettingsToTextureType(const ImageType type, const uint32 layerCount, const ImageSampling sampling)
    {
        switch (type)
        {
            case ImageType::Line:           return layerCount == 1 ? MTLTextureType1D : MTLTextureType1DArray;
            case ImageType::Plane:          return layerCount == 1 ? (sampling == ImageSampling::x1 ? MTLTextureType2D : MTLTextureType2DMultisample) : (sampling == ImageSampling::x1 ? MTLTextureType2DArray : MTLTextureType2DMultisampleArray);
            case ImageType::Volume:         return MTLTextureType3D;
            case ImageType::Cube:           return (layerCount / 6) == 1 ? MTLTextureTypeCube : MTLTextureTypeCubeArray;
        }

        return MTLTextureType2D;
    }

    MTLPixelFormat MetalImage::ImageFormatToPixelFormat(const ImageFormat format)
    {
        switch (format)
        {
            case ImageFormat::Undefined:                return MTLPixelFormatInvalid;

            case ImageFormat::R8_Int:                   return MTLPixelFormatR8Sint;
            case ImageFormat::R8_UInt:                  return MTLPixelFormatR8Uint;
            case ImageFormat::R8_Norm:                  return MTLPixelFormatR8Snorm;
            case ImageFormat::R8_UNorm:                 return MTLPixelFormatR8Unorm;
            case ImageFormat::R8_SRGB:                  return MTLPixelFormatR8Unorm_sRGB;
            case ImageFormat::R8G8_Int:                 return MTLPixelFormatRG8Sint;
            case ImageFormat::R8G8_UInt:                return MTLPixelFormatRG8Uint;
            case ImageFormat::R8G8_Norm:                return MTLPixelFormatRG8Snorm;
            case ImageFormat::R8G8_UNorm:               return MTLPixelFormatRG8Unorm;
            case ImageFormat::R8G8_SRGB:                return MTLPixelFormatRG8Unorm_sRGB;
            case ImageFormat::R8G8B8A8_Int:             return MTLPixelFormatRGBA8Sint;
            case ImageFormat::R8G8B8A8_UInt:            return MTLPixelFormatRGBA8Uint;
            case ImageFormat::R8G8B8A8_Norm:            return MTLPixelFormatRGBA8Snorm;
            case ImageFormat::R8G8B8A8_UNorm:           return MTLPixelFormatRGBA8Unorm;
            case ImageFormat::R8G8B8A8_SRGB:            return MTLPixelFormatRGBA8Unorm_sRGB;

            case ImageFormat::R16_Int:                  return MTLPixelFormatR16Sint;
            case ImageFormat::R16_UInt:                 return MTLPixelFormatR16Uint;
            case ImageFormat::R16_Norm:                 return MTLPixelFormatR16Snorm;
            case ImageFormat::R16_UNorm:                return MTLPixelFormatR16Unorm;
            case ImageFormat::R16_Float:                return MTLPixelFormatR16Float;
            case ImageFormat::R16G16_Int:               return MTLPixelFormatRG16Sint;
            case ImageFormat::R16G16_UInt:              return MTLPixelFormatRG16Uint;
            case ImageFormat::R16G16_Norm:              return MTLPixelFormatRG16Snorm;
            case ImageFormat::R16G16_UNorm:             return MTLPixelFormatRG16Unorm;
            case ImageFormat::R16G16_Float:             return MTLPixelFormatRG16Float;
            case ImageFormat::R16G16B16A16_Int:         return MTLPixelFormatRGBA16Sint;
            case ImageFormat::R16G16B16A16_UInt:        return MTLPixelFormatRGBA16Uint;
            case ImageFormat::R16G16B16A16_Norm:        return MTLPixelFormatRGBA16Snorm;
            case ImageFormat::R16G16B16A16_UNorm:       return MTLPixelFormatRGBA16Unorm;
            case ImageFormat::R16G16B16A16_Float:       return MTLPixelFormatRGBA16Float;

            case ImageFormat::R32_Int:                  return MTLPixelFormatR32Sint;
            case ImageFormat::R32_UInt:                 return MTLPixelFormatR32Uint;
            case ImageFormat::R32_Float:                return MTLPixelFormatR32Float;
            case ImageFormat::R32G32_Int:               return MTLPixelFormatRG32Sint;
            case ImageFormat::R32G32_UInt:              return MTLPixelFormatRG32Uint;
            case ImageFormat::R32G32_Float:             return MTLPixelFormatRG32Float;
            case ImageFormat::R32G32B32A32_Int:         return MTLPixelFormatRGBA32Sint;
            case ImageFormat::R32G32B32A32_UInt:        return MTLPixelFormatRGBA32Uint;
            case ImageFormat::R32G32B32A32_Float:       return MTLPixelFormatRGBA32Float;

            case ImageFormat::D16_UNorm:                return MTLPixelFormatDepth16Unorm;
            case ImageFormat::D32_Float:                return MTLPixelFormatDepth32Float;

            case ImageFormat::B8G8R8A8_UNorm:           return MTLPixelFormatBGRA8Unorm;
            case ImageFormat::B8G8R8A8_SRGB:            return MTLPixelFormatBGRA8Unorm_sRGB;

            case ImageFormat::BC1_RGB_UNorm:
            case ImageFormat::BC1_RGBA_UNorm:           return MTLPixelFormatBC1_RGBA;
            case ImageFormat::BC1_RGB_SRGB:
            case ImageFormat::BC1_RGBA_SRGB:            return MTLPixelFormatBC1_RGBA_sRGB;
            case ImageFormat::BC3_RGBA_UNorm:           return MTLPixelFormatBC3_RGBA;
            case ImageFormat::BC3_RGBA_SRGB:            return MTLPixelFormatBC3_RGBA_sRGB;
            case ImageFormat::BC4_R_Norm:               return MTLPixelFormatBC4_RSnorm;
            case ImageFormat::BC4_R_UNorm:              return MTLPixelFormatBC4_RUnorm;
            case ImageFormat::BC5_RG_Norm:              return MTLPixelFormatBC5_RGSnorm;
            case ImageFormat::BC5_RG_UNorm:             return MTLPixelFormatBC5_RGUnorm;
            case ImageFormat::BC6_HDR_RGB_Float:        return MTLPixelFormatBC6H_RGBFloat;
            case ImageFormat::BC6_HDR_RGB_UFloat:       return MTLPixelFormatBC6H_RGBUfloat;
            case ImageFormat::BC7_RGB_UNorm:
            case ImageFormat::BC7_RGBA_UNorm:           return MTLPixelFormatBC7_RGBAUnorm;
            case ImageFormat::BC7_RGB_SRGB:
            case ImageFormat::BC7_RGBA_SRGB:            return MTLPixelFormatBC7_RGBAUnorm_sRGB;

            case ImageFormat::ASTC_4x4_UNorm:           return MTLPixelFormatASTC_4x4_LDR;
            case ImageFormat::ASTC_4x4_SRGB:            return MTLPixelFormatASTC_4x4_sRGB;
            case ImageFormat::ASTC_8x8_UNorm:           return MTLPixelFormatASTC_8x8_LDR;
            case ImageFormat::ASTC_8x8_SRGB:            return MTLPixelFormatASTC_8x8_sRGB;

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
            case ImageFormat::R64G64B64A64_Float:       return MTLPixelFormatInvalid;
        }

        return MTLPixelFormatInvalid;
    }

    MTLTextureUsage MetalImage::ImageUsageToTextureUsage(const ImageUsage usage)
    {
        MTLTextureUsage usageFlags = MTLTextureUsageUnknown;
        if (usage & ImageUsage::SourceMemory)           usageFlags |= MTLTextureUsageUnknown;
        if (usage & ImageUsage::DestinationMemory)      usageFlags |= MTLTextureUsageUnknown;
        if (usage & ImageUsage::Storage)                usageFlags |= MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
        if (usage & ImageUsage::Sample)                 usageFlags |= MTLTextureUsageShaderRead;
        if (usage & ImageUsage::ColorAttachment ||
            usage & ImageUsage::DepthAttachment ||
            usage & ImageUsage::InputAttachment ||
            usage & ImageUsage::TransientAttachment)    usageFlags |= MTLTextureUsageRenderTarget;
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
            case ImageMemoryLocation::CPU:      return MTLStorageModeShared;
            case ImageMemoryLocation::GPU:      return MTLStorageModePrivate;
        }

        return MTLStorageModeShared;
    }

    MTLCPUCacheMode MetalImage::ImageMemoryLocationToCPUCacheMode(const ImageMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::CPU:      return MTLCPUCacheModeDefaultCache;
            case ImageMemoryLocation::GPU:      return MTLCPUCacheModeWriteCombined;
        }

        return MTLCPUCacheModeDefaultCache;
    }

}
