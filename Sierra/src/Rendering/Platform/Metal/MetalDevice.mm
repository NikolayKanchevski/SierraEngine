//
// Created by Nikolay Kanchevski on 21.11.23.
//

#include "MetalDevice.h"

#include "MetalImage.h"
#include "MetalCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalDevice::MetalDevice(const DeviceCreateInfo &createInfo)
        : Device(createInfo), MetalResource(createInfo.name)
    {
        // Create device
        device = MTLCreateSystemDefaultDevice();
        SR_ERROR_IF(device == nil, "[Metal]: Could not create default system device for device [{0}]!", GetName());

        // Save device name
        deviceName = std::string(device.name.UTF8String, device.name.length);

        // Create command queue
        commandQueue = [device newCommandQueue];
        SR_ERROR_IF(commandQueue == nil, "[Metal]: Could not create command queue for device [{0}]!", GetName());
    }

    /* --- POLLING METHODS --- */

    void MetalDevice::SubmitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer,  const std::initializer_list<std::reference_wrapper<std::unique_ptr<CommandBuffer>>> &commandBuffersToWait) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot, from device [{0}], submit command buffer [{1}] with a graphics API, that differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<MetalCommandBuffer&>(*commandBuffer);

        for (uint32 i = 0; i < commandBuffersToWait.size(); i++)
        {
            const auto &commandBufferToWait = (commandBuffersToWait.begin() + i)->get();
            SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot, from device [{0}], submit command buffer [{1}], whilst waiting on command buffer [{2}], which has an index of [{3}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName(), commandBufferToWait->GetName(), i);

            const MetalCommandBuffer &metalCommandBufferToWait = static_cast<MetalCommandBuffer&>(*commandBufferToWait);
            while (!metalCommandBufferToWait.HasFinishedExecution());
        }

        // Submit command buffer
        [metalCommandBuffer.GetMetalCommandBuffer() commit];
    }

    void MetalDevice::WaitForCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot, from device [{0}], wait for command buffer [{1}] with a graphics API, that differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<MetalCommandBuffer&>(*commandBuffer);

        // Wait for completion
        while (!metalCommandBuffer.HasFinishedExecution());
    }

    /* --- GETTER METHODS --- */

    bool MetalDevice::IsImageFormatSupported(const ImageFormat format, const ImageUsage usage) const
    {
        // Not RGB format is supported by Metal
        if (format.channels == ImageChannels::RGB) return false;

        // No 64-bit format is supported by Metal
        switch (format.memoryType)
        {
            case ImageMemoryType::Int64:
            case ImageMemoryType::UInt64:
            case ImageMemoryType::Float64:      return false;
            default:                            break;
        }

        // Get pixel format
        const MTLPixelFormat pixelFormat = MetalImage::ImageFormatToPixelFormat(format);

        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "bugprone-branch-clone"

        /* === Reference: https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf === */
        switch (pixelFormat)
        {
            case MTLPixelFormatR8Sint:                return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatR8Uint:                return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatR8Unorm:               return true;
            case MTLPixelFormatR8Unorm_sRGB:          return true;
            case MTLPixelFormatR16Sint:               return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatR16Uint:               return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatR16Snorm:              return !(usage & ImageUsage::ResolveAttachment);
            case MTLPixelFormatR16Unorm:              return !(usage & ImageUsage::ResolveAttachment);
            case MTLPixelFormatR16Float:              return true;
            case MTLPixelFormatR32Sint:               return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatR32Uint:               return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatR32Float:              return [device supportsFamily: MTLGPUFamilyApple8] || !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRG8Sint:               return !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRG8Uint:               return !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRG8Unorm:              return true;
            case MTLPixelFormatRG8Unorm_sRGB:         return [device supportsFamily: MTLGPUFamilyApple2];
            case MTLPixelFormatRG16Sint:              return !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRG16Uint:              return !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRG16Snorm:             return !(usage & ImageUsage::ResolveAttachment);
            case MTLPixelFormatRG16Unorm:             return !(usage & ImageUsage::ResolveAttachment);
            case MTLPixelFormatRG16Float:             return true;
            case MTLPixelFormatRG32Sint:              return [device supportsFamily: MTLGPUFamilyApple7] && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRG32Uint:              return [device supportsFamily: MTLGPUFamilyApple7] && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRG32Float:             return [device supportsFamily: MTLGPUFamilyApple9];
            case MTLPixelFormatRGBA8Sint:             return !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRGBA8Uint:             return !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRGBA8Unorm:            return true;
            case MTLPixelFormatRGBA8Unorm_sRGB:       return true;
            case MTLPixelFormatRGBA16Sint:            return !(usage & ImageUsage::ColorAttachment) && !(usage & ImageUsage::DepthAttachment) && !(usage & ImageUsage::ResolveAttachment);
            case MTLPixelFormatRGBA16Uint:            return !(usage & ImageUsage::ColorAttachment) && !(usage & ImageUsage::DepthAttachment) && !(usage & ImageUsage::ResolveAttachment);
            case MTLPixelFormatRGBA16Snorm:           return !(usage & ImageUsage::ResolveAttachment);
            case MTLPixelFormatRGBA16Unorm:           return !(usage & ImageUsage::ResolveAttachment);
            case MTLPixelFormatRGBA16Float:           return true;
            case MTLPixelFormatRGBA32Sint:            return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRGBA32Uint:            return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::SmoothSample);
            case MTLPixelFormatRGBA32Float:           return [device supportsFamily: MTLGPUFamilyApple9] || (!(usage & ImageUsage::SmoothSample) && !(usage & ImageUsage::ResolveAttachment));
            case MTLPixelFormatBGRA8Unorm:            return true;
            case MTLPixelFormatBGRA8Unorm_sRGB:       return true;
            case MTLPixelFormatDepth16Unorm:          return [device supportsFamily: MTLGPUFamilyApple3] || !(usage & ImageUsage::ResolveAttachment);
            case MTLPixelFormatDepth32Float:          return ([device supportsFamily: MTLGPUFamilyApple3] || !(usage & ImageUsage::ResolveAttachment)) && ([device supportsFamily: MTLGPUFamilyMac2] || !(usage & ImageUsage::SmoothSample));
            default:                                  break;
        }

        return false;
    }

    #pragma clang diagnostic pop

    bool MetalDevice::IsImageSamplingSupported(const ImageSampling sampling) const
    {
        return [device supportsTextureSampleCount: MetalImage::ImageSamplingToUInteger(sampling)];
    }

    bool MetalDevice::IsSamplerAnisotropySupported(const SamplerAnisotropy anisotropy) const
    {
        switch (anisotropy)
        {
            case SamplerAnisotropy::x1:
            case SamplerAnisotropy::x2:
            case SamplerAnisotropy::x4:
            case SamplerAnisotropy::x8:
            case SamplerAnisotropy::x16:        return true;
            default:                            return false;
        }

        return anisotropy == SamplerAnisotropy::x1;
    }

    /* --- DESTRUCTOR --- */

    MetalDevice::~MetalDevice()
    {
        [commandQueue release];
        [device release];
    }

}