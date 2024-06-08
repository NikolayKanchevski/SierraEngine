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
        #if SR_ENABLE_LOGGING
            setenv("MTL_DEBUG_LAYER", "1", true);
            setenv("MTL_DEBUG_LAYER_ERROR_MODE", "assert", true);
            setenv("MTL_DEBUG_LAYER_VALIDATE_LOAD_ACTIONS", "1", true);
            setenv("MTL_DEBUG_LAYER_VALIDATE_STORE_ACTIONS", "1", true);
            setenv("MTL_DEBUG_LAYER_VALIDATE_UNRETAINED_RESOURCES", "0x4", true);
            setenv("MTL_SHADER_VALIDATION", "1", true);
            setenv("MTL_HUD_ENABLED", "1", true);
        #endif

        // Create device
        device = MTLCreateSystemDefaultDevice();
        SR_ERROR_IF(device == nil, "[Metal]: Could not create default system device for device [{0}]!", GetName());

        // Save device name
        deviceName = std::string(device.name.UTF8String, device.name.length);

        // Create command queue
        commandQueue = [device newCommandQueue];
        SR_ERROR_IF(commandQueue == nil, "[Metal]: Could not create command queue for device [{0}]!", GetName());

        // Create synchronization
        sharedSignalSemaphore = [device newSharedEvent];
        SR_ERROR_IF(commandQueue == nil, "[Metal]: Could not create shared signal semaphore for device [{0}]!", GetName());
    }

    /* --- POLLING METHODS --- */

    void MetalDevice::SubmitCommandBuffer(CommandBuffer &commandBuffer, const std::span<const CommandBuffer*> commandBuffersToWait) const
    {
        SR_ERROR_IF(commandBuffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot, from device [{0}], submit command buffer [{1}] with a graphics API, that differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer.GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(commandBuffer);

        // If we do not need any manual synchronization, directly submit command buffer
        if (commandBuffersToWait.empty())
        {
            [metalCommandBuffer.GetMetalCommandBuffer() encodeSignalEvent: sharedSignalSemaphore value: metalCommandBuffer.GetCompletionSignalValue()];
            [metalCommandBuffer.GetMetalCommandBuffer() commit];
            return;
        }

        // Try to find the semaphore counter of the command buffer
        auto iterator = std::find_if(commandBufferQueue.begin(), commandBufferQueue.end(), [&metalCommandBuffer](const CommandBufferQueueEntry &item) { return item.commandBuffer == metalCommandBuffer.GetMetalCommandBuffer(); });
        if (iterator == commandBufferQueue.end())
        {
            commandBufferQueue.push_back({ .commandBuffer = metalCommandBuffer.GetMetalCommandBuffer(), .counter = static_cast<uint32>(commandBuffersToWait.size()) });
            iterator = commandBufferQueue.end() - 1;
        }

        // Decrement semaphore counter after every command buffer to wait on until it reaches 0, then submit
        for (uint32 i = 0; i < commandBuffersToWait.size(); i++)
        {
            const CommandBuffer* commandBufferToWait = commandBuffersToWait[i];
            if (commandBufferToWait == nullptr) continue;

            SR_ERROR_IF(commandBuffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot, from device [{0}], submit command buffer [{1}], whilst waiting on command buffer [{2}], which has an index of [{3}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer.GetName(), commandBufferToWait->GetName(), i);
            const MetalCommandBuffer &metalCommandBufferToWait = static_cast<const MetalCommandBuffer&>(*commandBufferToWait);

            [metalCommandBufferToWait.GetMetalCommandBuffer() addCompletedHandler: ^(id<MTLCommandBuffer>)
            {
                auto semaphoreIterator = std::find_if(commandBufferQueue.begin(), commandBufferQueue.end(), [&metalCommandBuffer](const CommandBufferQueueEntry &item) { return item.commandBuffer == metalCommandBuffer.GetMetalCommandBuffer(); });
                if (semaphoreIterator->counter--; semaphoreIterator->counter == 0)
                {
                    [semaphoreIterator->commandBuffer encodeSignalEvent: sharedSignalSemaphore value: metalCommandBuffer.GetCompletionSignalValue()];
                    [semaphoreIterator->commandBuffer commit];
                    commandBufferQueue.erase(semaphoreIterator);
                }
            }];
        }
    }

    void MetalDevice::WaitForCommandBuffer(const CommandBuffer &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot, from device [{0}], wait for command buffer [{1}] with a graphics API, that differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer.GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(commandBuffer);

        // Wait for completion
        while ([sharedSignalSemaphore signaledValue] < metalCommandBuffer.GetCompletionSignalValue());
    }

    /* --- SETTER METHODS --- */

    void MetalDevice::SetResourceName( MTLHandle resource, const std::string_view name) const
    {
        #if SR_ENABLE_LOGGING
            NSString* const label = [[NSString alloc] initWithCString: name.data() encoding: NSASCIIStringEncoding];
            [((__bridge id) resource) performSelector: @selector(setLabel:) withObject: label];
            [label release];
        #endif
    }

    /* --- GETTER METHODS --- */

    bool MetalDevice::IsImageFormatSupported(const ImageFormat format, const ImageUsage usage) const
    {
        // No 64-bit format is supported by Metal
        if (ImageFormatToPixelMemorySize(format) == 8) return false;

        // Get pixel format
        const MTLPixelFormat pixelFormat = MetalImage::ImageFormatToPixelFormat(format);

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
            case MTLPixelFormatBC7_RGBAUnorm_sRGB:    return [device supportsBCTextureCompression];
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
            default:                            break;
        }

        return false;
    }

    /* --- DESTRUCTOR --- */

    MetalDevice::~MetalDevice()
    {
        [sharedSignalSemaphore release];
        [commandQueue release];

        [device release];
    }

}