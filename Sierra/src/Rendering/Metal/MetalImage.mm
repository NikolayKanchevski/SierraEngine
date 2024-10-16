//
// Created by Nikolay Kanchevski on 7.12.23.
//

#include "MetalImage.h"

#include "../DeviceErrors.h"

namespace Sierra
{

    namespace
    {
        ImageFormat SwapchainPixelFormatToImageFormat(const MTLPixelFormat format)
        {
            switch (format)
            {
                case MTLPixelFormatBGRA8Unorm:          return ImageFormat::B8G8R8A8_UNorm;
                case MTLPixelFormatBGRA8Unorm_sRGB:     return ImageFormat::B8G8R8A8_SRGB;
                case MTLPixelFormatRGBA16Float:         return ImageFormat::R16G16B16A16_Float;
                default:                                break;
            }

            return ImageFormat::Undefined;
        }
    }

    /* --- CONVERSIONS --- */

    MTLTextureType ImageSettingsToTextureType(const ImageType type, const uint32 layerCount, const ImageSampling sampling) noexcept
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

    MTLPixelFormat ImageFormatToPixelFormat(const ImageFormat format) noexcept
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

    MTLTextureUsage ImageUsageToTextureUsage(const ImageUsage usage) noexcept
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

    NSUInteger ImageSamplingToUInteger(const ImageSampling sampling) noexcept
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

    MTLTextureSwizzleChannels ImageComponentSwizzlesToTextureSwizzleChannels(const ImageComponentSwizzle redSwizzle, const ImageComponentSwizzle greenSwizzle, const ImageComponentSwizzle blueSwizzle, const ImageComponentSwizzle alphaSwizzle) noexcept
    {
        constexpr auto NonIdentityImageComponentSwizzleToTextureSwizzle = [](const ImageComponentSwizzle componentSwizzle) -> MTLTextureSwizzle
        {
            switch (componentSwizzle)
            {
                case ImageComponentSwizzle::Zero:       return MTLTextureSwizzleZero;
                case ImageComponentSwizzle::One:        return MTLTextureSwizzleOne;
                case ImageComponentSwizzle::Red:        return MTLTextureSwizzleRed;
                case ImageComponentSwizzle::Green:      return MTLTextureSwizzleGreen;
                case ImageComponentSwizzle::Blue:       return MTLTextureSwizzleBlue;
                case ImageComponentSwizzle::Alpha:      return MTLTextureSwizzleAlpha;
                default:                                break;
            }

            return MTLTextureSwizzleZero;
        };

        return MTLTextureSwizzleChannelsMake(
            redSwizzle   == ImageComponentSwizzle::Identity ? MTLTextureSwizzleRed   : NonIdentityImageComponentSwizzleToTextureSwizzle(redSwizzle),
            greenSwizzle == ImageComponentSwizzle::Identity ? MTLTextureSwizzleGreen : NonIdentityImageComponentSwizzleToTextureSwizzle(greenSwizzle),
            blueSwizzle  == ImageComponentSwizzle::Identity ? MTLTextureSwizzleBlue  : NonIdentityImageComponentSwizzleToTextureSwizzle(blueSwizzle),
            alphaSwizzle == ImageComponentSwizzle::Identity ? MTLTextureSwizzleAlpha : NonIdentityImageComponentSwizzleToTextureSwizzle(alphaSwizzle)
        );
    }

    MTLStorageMode ImageMemoryLocationToStorageMode(const ImageMemoryLocation memoryLocation) noexcept
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::CPU:      return MTLStorageModeShared;
            case ImageMemoryLocation::GPU:      return MTLStorageModePrivate;
        }

        return MTLStorageModeShared;
    }

    MTLCPUCacheMode ImageMemoryLocationToCPUCacheMode(const ImageMemoryLocation memoryLocation) noexcept
    {
        switch (memoryLocation)
        {
            case ImageMemoryLocation::CPU:      return MTLCPUCacheModeDefaultCache;
            case ImageMemoryLocation::GPU:      return MTLCPUCacheModeWriteCombined;
        }

        return MTLCPUCacheModeDefaultCache;
    }

    /* --- CONSTRUCTORS --- */

    MetalImage::MetalImage(const MetalDevice& device, const ImageCreateInfo& createInfo)
        : Image(createInfo), width(createInfo.width), height(createInfo.height), depth(createInfo.depth), format(createInfo.format), levelCount(createInfo.levelCount), layerCount(createInfo.layerCount), sampling(createInfo.sampling)
    {
        SR_THROW_IF(createInfo.type == ImageType::Line && createInfo.width > device.GetLimits().maxLineImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified width is greater than device [{1}]'s max line image dimensions", createInfo.name, device.GetName()), createInfo.width, 0U, device.GetLimits().maxLineImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Line && createInfo.height > device.GetLimits().maxLineImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified height is greater than device [{1}]'s max line image dimensions", createInfo.name, device.GetName()), createInfo.height, 0U, device.GetLimits().maxLineImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Line && createInfo.depth > device.GetLimits().maxLineImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified depth is greater than device [{1}]'s max line image dimensions", createInfo.name, device.GetName()), createInfo.depth, 0U, device.GetLimits().maxLineImageDimensions));

        SR_THROW_IF(createInfo.type == ImageType::Plane && createInfo.width > device.GetLimits().maxPlaneImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified width is greater than device [{1}]'s max plane image dimensions", createInfo.name, device.GetName()), createInfo.width, 0U, device.GetLimits().maxPlaneImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Plane && createInfo.height > device.GetLimits().maxPlaneImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified height is greater than device [{1}]'s max plane image dimensions", createInfo.name, device.GetName()), createInfo.height, 0U, device.GetLimits().maxPlaneImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Plane && createInfo.depth > device.GetLimits().maxPlaneImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified depth is greater than device [{1}]'s max plane image dimensions", createInfo.name, device.GetName()), createInfo.depth, 0U, device.GetLimits().maxPlaneImageDimensions));

        SR_THROW_IF(createInfo.type == ImageType::Volume && createInfo.width > device.GetLimits().maxVolumeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified width is greater than device [{1}]'s max volume image dimensions", createInfo.name, device.GetName()), createInfo.width, 0U, device.GetLimits().maxVolumeImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Volume && createInfo.height > device.GetLimits().maxVolumeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified height is greater than device [{1}]'s max volume image dimensions", createInfo.name, device.GetName()), createInfo.height, 0U, device.GetLimits().maxVolumeImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Volume && createInfo.depth > device.GetLimits().maxVolumeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified depth is greater than device [{1}]'s max volume image dimensions", createInfo.name, device.GetName()), createInfo.depth, 0U, device.GetLimits().maxVolumeImageDimensions));

        SR_THROW_IF(createInfo.type == ImageType::Cube && createInfo.width > device.GetLimits().maxCubeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified width is greater than device [{1}]'s max cube image dimensions", createInfo.name, device.GetName()), createInfo.width, 0U, device.GetLimits().maxCubeImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Cube && createInfo.height > device.GetLimits().maxCubeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified height is greater than device [{1}]'s max cube image dimensions", createInfo.name, device.GetName()), createInfo.height, 0U, device.GetLimits().maxCubeImageDimensions));
        SR_THROW_IF(createInfo.type == ImageType::Cube && createInfo.depth > device.GetLimits().maxCubeImageDimensions, ValueOutOfRangeError(SR_FORMAT("Cannot create image [{0}], as specified depth is greater than device [{1}]'s max cube image dimensions", createInfo.name, device.GetName()), createInfo.depth, 0U, device.GetLimits().maxCubeImageDimensions));

        SR_THROW_IF(!device.IsImageFormatSupported(createInfo.format, createInfo.usage), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create image [{1}] with unsupported format - use Device::IsImageFormatSupported() to query support", device.GetName(), createInfo.name)));
        SR_THROW_IF(!device.IsImageSamplingSupported(createInfo.sampling), UnsupportedFeatureError(SR_FORMAT("Device [{0}] cannot create image [{1}] with unsupported sampling - use Device::IsImageSamplingSupported() to query support", device.GetName(), createInfo.name)));

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
        [textureDescriptor setSwizzle: ImageComponentSwizzlesToTextureSwizzleChannels(createInfo.redSwizzle, createInfo.greenSwizzle, createInfo.blueSwizzle, createInfo.alphaSwizzle)];
        [textureDescriptor setStorageMode: createInfo.usage & ImageUsage::TransientAttachment ? MTLStorageModeMemoryless : ImageMemoryLocationToStorageMode(createInfo.memoryLocation)];
        [textureDescriptor setCpuCacheMode: ImageMemoryLocationToCPUCacheMode(createInfo.memoryLocation)];
        [textureDescriptor setHazardTrackingMode: MTLHazardTrackingModeUntracked];

        // Allocate texture
        texture = [device.GetMetalDevice() newTextureWithDescriptor: textureDescriptor];
        SR_THROW_IF(texture == nil, UnknownDeviceError(SR_FORMAT("Could not create image [{0}]", createInfo.name)));
        device.SetResourceName(texture, createInfo.name);

        [textureDescriptor release];
    }

    MetalImage::MetalImage(const MetalDevice& device, const SwapchainImageCreateInfo& createInfo)
        : Image({ .name = createInfo.name, .width = createInfo.width, .height = createInfo.height, .format = SwapchainPixelFormatToImageFormat(createInfo.format), .usage = ImageUsage::ColorAttachment, .memoryLocation = ImageMemoryLocation::GPU }),
          width(createInfo.width), height(createInfo.height), format(SwapchainPixelFormatToImageFormat(createInfo.format)), texture(createInfo.texture), swapchainImage(true)
    {
        SR_THROW_IF(createInfo.texture == nil, InvalidValueError(SR_FORMAT("Cannot create swapchain image [{0}], as specified texture must not be nil", createInfo.name)));
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalImage::GetName() const noexcept
    {
        return { texture.label.UTF8String, texture.label.length };
    }

    /* --- DESTRUCTOR --- */

    MetalImage::~MetalImage() noexcept
    {
        if (!swapchainImage) [texture release];
    }

}
