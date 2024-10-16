//
// Created by Nikolay Kanchevski on 21.11.23.
//

#include "MetalDevice.h"

#include "MetalBuffer.h"
#include "MetalImage.h"
#include "MetalSampler.h"
#include "MetalRenderPass.h"
#include "MetalSwapchain.h"
#include "MetalShader.h"
#include "MetalGraphicsPipeline.h"
#include "MetalComputePipeline.h"
#include "MetalResourceTable.h"
#include "MetalQueue.h"
#include "../DeviceErrors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalDevice::MetalDevice(const MetalContext& context, const id<MTLDevice> device, const DeviceCreateInfo& createInfo)
        : Device(createInfo), context(context), device(device), name(createInfo.name)
    {
        SR_THROW_IF(device == nil, InvalidValueError(SR_FORMAT("Cannot create Metal device [{0}], as specified device must not be null", createInfo.name)));

        #if SR_ENABLE_LOGGING
            setenv("MTL_DEBUG_LAYER", "1", true);
            setenv("MTL_DEBUG_LAYER_ERROR_MODE", "assert", true);
            setenv("MTL_DEBUG_LAYER_VALIDATE_LOAD_ACTIONS", "1", true);
            setenv("MTL_DEBUG_LAYER_VALIDATE_STORE_ACTIONS", "1", true);
            setenv("MTL_DEBUG_LAYER_VALIDATE_UNRETAINED_RESOURCES", "0x4", true);
            setenv("MTL_SHADER_VALIDATION", "1", true);
            setenv("MTL_HUD_ENABLED", "1", true);
        #endif

        driverVersion = Version({
            static_cast<uint32>(NSProcessInfo.processInfo.operatingSystemVersion.majorVersion),
            static_cast<uint32>(NSProcessInfo.processInfo.operatingSystemVersion.minorVersion),
            static_cast<uint32>(NSProcessInfo.processInfo.operatingSystemVersion.patchVersion)
        });

        // Create synchronization
        semaphore = [device newSharedEvent];
        SR_THROW_IF(semaphore == nil, UnknownDeviceError(SR_FORMAT("Could not create buffer [{0}], as creation of shared signal semaphore failed", GetName())));
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Buffer> MetalDevice::CreateBuffer(const BufferCreateInfo& createInfo) const
    {
        return std::make_unique<MetalBuffer>(*this, createInfo);
    }

    std::unique_ptr<Image> MetalDevice::CreateImage(const ImageCreateInfo& createInfo) const
    {
        return std::make_unique<MetalImage>(*this, createInfo);
    }

    std::unique_ptr<Sampler> MetalDevice::CreateSampler(const SamplerCreateInfo& createInfo) const
    {
        return std::make_unique<MetalSampler>(*this, createInfo);
    }

    std::unique_ptr<RenderPass> MetalDevice::CreateRenderPass(const RenderPassCreateInfo& createInfo) const
    {
        return std::make_unique<MetalRenderPass>(*this, createInfo);
    }

    std::unique_ptr<Swapchain> MetalDevice::CreateSwapchain(const SwapchainCreateInfo& createInfo) const
    {
        return std::make_unique<MetalSwapchain>(*this, createInfo);
    }

    std::unique_ptr<Shader> MetalDevice::CreateShader(const ShaderCreateInfo& createInfo) const
    {
        return std::make_unique<MetalShader>(*this, createInfo);
    }

    std::unique_ptr<GraphicsPipeline> MetalDevice::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) const
    {
        return std::make_unique<MetalGraphicsPipeline>(*this, createInfo);
    }

    std::unique_ptr<ComputePipeline> MetalDevice::CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) const
    {
        return std::make_unique<MetalComputePipeline>(*this, createInfo);
    }

    std::unique_ptr<ResourceTable> MetalDevice::CreateResourceTable(const ResourceTableCreateInfo& createInfo) const
    {
        return std::make_unique<MetalResourceTable>(*this, createInfo);
    }

    std::unique_ptr<Queue> MetalDevice::CreateQueue(const QueueCreateInfo& createInfo) const
    {
        return std::make_unique<MetalQueue>(*this, createInfo);
    }

    /* --- SETTER METHODS --- */

    void MetalDevice::SetResourceName(const MTLResource* resource, const std::string_view resourceName) const noexcept
    {
        const NSString* const label = [NSString stringWithCString: resourceName.data() length: resourceName.size()];
        [((__bridge id) resource) performSelector: @selector(setLabel:) withObject: label];
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalDevice::GetHardwareName() const noexcept
    {
        return { device.name.UTF8String, device.name.length };
    }

    DeviceLimits MetalDevice::GetLimits() const noexcept
    {
        /* === Reference: https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf === */
        return
        {
            .maxUniformBufferSize = std::numeric_limits<size>::max(),
            .maxStorageBufferSize = std::numeric_limits<size>::max(),
            .maxLineImageDimensions = [device supportsFamily: MTLGPUFamilyApple3]   ? 16384U : 8192U,
            .maxPlaneImageDimensions = [device supportsFamily: MTLGPUFamilyApple3]  ? 16384U : 8192U,
            .maxVolumeImageDimensions = [device supportsFamily: MTLGPUFamilyApple3] ? 16384U : 8192U,
            .maxCubeImageDimensions = [device supportsFamily: MTLGPUFamilyApple3]   ? 16384U : 8192U,
            .resourceTableUniformBufferCapacity = RESOURCE_TABLE_UNIFORM_BUFFER_CAPACITY,
            .resourceTableStorageBufferCapacity = RESOURCE_TABLE_STORAGE_BUFFER_CAPACITY,
            .resourceTableSampledImageCapacity = RESOURCE_TABLE_SAMPLED_IMAGE_CAPACITY,
            .resourceTableStorageImageCapacity = RESOURCE_TABLE_STORAGE_IMAGE_CAPACITY,
            .resourceTableSamplerCapacity = RESOURCE_TABLE_SAMPLER_CAPACITY,
            .maxRenderPassWidth = [device supportsFamily: MTLGPUFamilyApple3]  ? 16384U : 8192U,
            .maxRenderPassHeight = [device supportsFamily: MTLGPUFamilyApple3] ? 16384U : 8192U,
            .maxWorkGroupSize = { [device supportsFamily: MTLGPUFamilyApple4] ? 1024U : 512U, [device supportsFamily: MTLGPUFamilyApple4] ? 1024U : 512U, [device supportsFamily: MTLGPUFamilyApple4] ? 1024U : 512U },
            .highestImageSampling = GetHighestImageSamplingSupported(),
            .highestSamplerAnisotropy = GetHighestSamplerAnisotropySupported()
        };
    }

    bool MetalDevice::IsImageFormatSupported(const ImageFormat format, const ImageUsage usage) const noexcept
    {
        // No 64-bit format is supported by Metal
        if (ImageFormatToChannelMemorySize(format) == 8) return false;

        // Get pixel format
        const MTLPixelFormat pixelFormat = ImageFormatToPixelFormat(format);

        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "bugprone-branch-clone"

        /* === Reference: https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf === */
        switch (pixelFormat)
        {
            case MTLPixelFormatInvalid:               return false;
            case MTLPixelFormatR8Sint:                return !(usage & ImageUsage::ResolverAttachment) && !(usage & ImageUsage::Filter);
            case MTLPixelFormatR8Uint:                return !(usage & ImageUsage::ResolverAttachment) && !(usage & ImageUsage::Filter);
            case MTLPixelFormatR8Unorm:               return true;
            case MTLPixelFormatR8Unorm_sRGB:          return true;
            case MTLPixelFormatR16Sint:               return !(usage & ImageUsage::ResolverAttachment) && !(usage & ImageUsage::Filter);
            case MTLPixelFormatR16Uint:               return !(usage & ImageUsage::ResolverAttachment) && !(usage & ImageUsage::Filter);
            case MTLPixelFormatR16Snorm:              return !(usage & ImageUsage::ResolverAttachment);
            case MTLPixelFormatR16Unorm:              return !(usage & ImageUsage::ResolverAttachment);
            case MTLPixelFormatR16Float:              return true;
            case MTLPixelFormatR32Sint:               return !(usage & ImageUsage::ResolverAttachment) && !(usage & ImageUsage::Filter);
            case MTLPixelFormatR32Uint:               return !(usage & ImageUsage::ResolverAttachment) && !(usage & ImageUsage::Filter);
            case MTLPixelFormatR32Float:              return [device supportsFamily: MTLGPUFamilyApple8] || !(usage & ImageUsage::Filter);
            case MTLPixelFormatRG8Sint:               return !(usage & ImageUsage::Filter);
            case MTLPixelFormatRG8Uint:               return !(usage & ImageUsage::Filter);
            case MTLPixelFormatRG8Unorm:              return true;
            case MTLPixelFormatRG8Unorm_sRGB:         return [device supportsFamily: MTLGPUFamilyApple2];
            case MTLPixelFormatRG16Sint:              return !(usage & ImageUsage::Filter);
            case MTLPixelFormatRG16Uint:              return !(usage & ImageUsage::Filter);
            case MTLPixelFormatRG16Snorm:             return !(usage & ImageUsage::ResolverAttachment);
            case MTLPixelFormatRG16Unorm:             return !(usage & ImageUsage::ResolverAttachment);
            case MTLPixelFormatRG16Float:             return true;
            case MTLPixelFormatRG32Sint:              return [device supportsFamily: MTLGPUFamilyApple7] && !(usage & ImageUsage::Filter);
            case MTLPixelFormatRG32Uint:              return [device supportsFamily: MTLGPUFamilyApple7] && !(usage & ImageUsage::Filter);
            case MTLPixelFormatRG32Float:             return [device supportsFamily: MTLGPUFamilyApple9];
            case MTLPixelFormatRGBA8Sint:             return !(usage & ImageUsage::Filter);
            case MTLPixelFormatRGBA8Uint:             return !(usage & ImageUsage::Filter);
            case MTLPixelFormatRGBA8Unorm:            return true;
            case MTLPixelFormatRGBA8Unorm_sRGB:       return true;
            case MTLPixelFormatRGBA16Sint:            return !(usage & ImageUsage::ColorAttachment) && !(usage & ImageUsage::DepthAttachment) && !(usage & ImageUsage::ResolverAttachment);
            case MTLPixelFormatRGBA16Uint:            return !(usage & ImageUsage::ColorAttachment) && !(usage & ImageUsage::DepthAttachment) && !(usage & ImageUsage::ResolverAttachment);
            case MTLPixelFormatRGBA16Snorm:           return !(usage & ImageUsage::ResolverAttachment);
            case MTLPixelFormatRGBA16Unorm:           return !(usage & ImageUsage::ResolverAttachment);
            case MTLPixelFormatRGBA16Float:           return true;
            case MTLPixelFormatRGBA32Sint:            return !(usage & ImageUsage::ResolverAttachment) && !(usage & ImageUsage::Filter);
            case MTLPixelFormatRGBA32Uint:            return !(usage & ImageUsage::ResolverAttachment) && !(usage & ImageUsage::Filter);
            case MTLPixelFormatRGBA32Float:           return [device supportsFamily: MTLGPUFamilyApple9] || (!(usage & ImageUsage::Filter) && !(usage & ImageUsage::ResolverAttachment));
            case MTLPixelFormatBGRA8Unorm:            return true;
            case MTLPixelFormatBGRA8Unorm_sRGB:       return true;
            case MTLPixelFormatDepth16Unorm:          return [device supportsFamily: MTLGPUFamilyApple3] || !(usage & ImageUsage::ResolverAttachment);
            case MTLPixelFormatDepth32Float:          return ([device supportsFamily: MTLGPUFamilyApple3] || !(usage & ImageUsage::ResolverAttachment)) && ([device supportsFamily: MTLGPUFamilyMac2] || !(usage & ImageUsage::Filter));
            case MTLPixelFormatBC1_RGBA:
            case MTLPixelFormatBC1_RGBA_sRGB:
            case MTLPixelFormatBC3_RGBA:
            case MTLPixelFormatBC3_RGBA_sRGB:
            case MTLPixelFormatBC4_RUnorm:
            case MTLPixelFormatBC4_RSnorm:
            case MTLPixelFormatBC6H_RGBFloat:
            case MTLPixelFormatBC6H_RGBUfloat:
            case MTLPixelFormatBC7_RGBAUnorm:
            case MTLPixelFormatBC7_RGBAUnorm_sRGB:    return device.supportsBCTextureCompression;
            default:                                  break;
        }

        return false;
    }

    #pragma clang diagnostic pop

    bool MetalDevice::IsImageSamplingSupported(const ImageSampling sampling) const noexcept
    {
        return [device supportsTextureSampleCount: ImageSamplingToUInteger(sampling)];
    }

    bool MetalDevice::IsSamplerAnisotropySupported(const SamplerAnisotropy anisotropy) const noexcept
    {
        switch (anisotropy)
        {
            case SamplerAnisotropy::x1:
            case SamplerAnisotropy::x2:
            case SamplerAnisotropy::x4:
            case SamplerAnisotropy::x8:
            case SamplerAnisotropy::x16:        return true;
            default:                            break;
        }

        return false;
    }

    /* --- OPERATORS --- */

    bool MetalDevice::operator==(const MetalDevice& other) noexcept
    {
        return device.registryID == other.device.registryID;
    }

    bool MetalDevice::operator!=(const MetalDevice& other) noexcept
    {
        return !(*this == other);
    }

    /* --- DESTRUCTOR --- */

    MetalDevice::~MetalDevice() noexcept
    {
        [semaphore release];
        [device release];
    }

}