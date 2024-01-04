//
// Created by Nikolay Kanchevski on 21.11.23.
//

#include "MetalDevice.h"

#include "MetalImage.h"
#include "MetalCommandBuffer.h"

namespace MTL { static MTL::GPUFamily GPUFamilyApple9 = (MTL::GPUFamily) 1009; } // metal-cpp does not contain it
namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalDevice::MetalDevice(const DeviceCreateInfo &createInfo)
        : Device(createInfo), MetalResource(createInfo.name)
    {
        // Create device
        device = MTL::CreateSystemDefaultDevice();
        SR_ERROR_IF(device == nullptr, "[Metal]: Could not create default system device for device [{0}]!", GetName());

        // Create command queue
        commandQueue = device->newCommandQueue();
        SR_ERROR_IF(commandQueue == nullptr, "[Metal]: Could not create command queue for device [{0}]!", GetName());

        // Create shared command buffer semaphore
        sharedCommandBufferSemaphore = dispatch_semaphore_create(1);
    }

    /* --- POLLING METHODS --- */

    void MetalDevice::SubmitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot, from device [{0}], submit command buffer [{1}] with a graphics API, that is not [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        // Submit command buffer
        metalCommandBuffer.GetMetalCommandBuffer()->commit();
    }

    void MetalDevice::SubmitAndWaitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot, from device [{0}], submit command buffer [{1}] with a graphics API, that is not [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        // Submit command buffer
        metalCommandBuffer.GetMetalCommandBuffer()->addCompletedHandler(^(MTL::CommandBuffer*) { dispatch_semaphore_signal(sharedCommandBufferSemaphore); });
        metalCommandBuffer.GetMetalCommandBuffer()->commit();

        // Wait for command buffer
        dispatch_semaphore_wait(sharedCommandBufferSemaphore, DISPATCH_TIME_FOREVER);
    }

    void MetalDevice::WaitUntilIdle() const
    {

    }

    /* --- GETTER METHODS --- */

    bool MetalDevice::IsImageConfigurationSupported(const ImageFormat format, const ImageUsage usage) const
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
        const MTL::PixelFormat pixelFormat = MetalImage::ImageFormatToPixelFormat(format);

        #pragma clang diagnostic push
        #pragma ide diagnostic ignored "bugprone-branch-clone"

        /* === Reference: https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf === */
        switch (pixelFormat)
        {
            case MTL::PixelFormatR8Sint:                return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR8Uint:                return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR8Unorm:               return true;
            case MTL::PixelFormatR8Unorm_sRGB:          return true;
            case MTL::PixelFormatR16Sint:               return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR16Uint:               return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR16Snorm:              return !(usage & ImageUsage::ResolveAttachment);
            case MTL::PixelFormatR16Unorm:              return !(usage & ImageUsage::ResolveAttachment);
            case MTL::PixelFormatR16Float:              return true;
            case MTL::PixelFormatR32Sint:               return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR32Uint:               return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR32Float:              return device->supportsFamily(MTL::GPUFamilyApple8) || !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG8Sint:               return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG8Uint:               return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG8Unorm:              return true;
            case MTL::PixelFormatRG8Unorm_sRGB:         return device->supportsFamily(MTL::GPUFamilyApple2);
            case MTL::PixelFormatRG16Sint:              return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG16Uint:              return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG16Snorm:             return !(usage & ImageUsage::ResolveAttachment);
            case MTL::PixelFormatRG16Unorm:             return !(usage & ImageUsage::ResolveAttachment);
            case MTL::PixelFormatRG16Float:             return true;
            case MTL::PixelFormatRG32Sint:              return device->supportsFamily(MTL::GPUFamilyApple7) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG32Uint:              return device->supportsFamily(MTL::GPUFamilyApple7) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG32Float:             return device->supportsFamily(MTL::GPUFamilyApple9);
            case MTL::PixelFormatRGBA8Sint:             return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRGBA8Uint:             return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRGBA8Unorm:            return true;
            case MTL::PixelFormatRGBA8Unorm_sRGB:       return true;
            case MTL::PixelFormatRGBA16Sint:            return !(usage & ImageUsage::ColorAttachment) && !(usage & ImageUsage::DepthAttachment) && !(usage & ImageUsage::ResolveAttachment);
            case MTL::PixelFormatRGBA16Uint:            return !(usage & ImageUsage::ColorAttachment) && !(usage & ImageUsage::DepthAttachment) && !(usage & ImageUsage::ResolveAttachment);
            case MTL::PixelFormatRGBA16Snorm:           return !(usage & ImageUsage::ResolveAttachment);
            case MTL::PixelFormatRGBA16Unorm:           return !(usage & ImageUsage::ResolveAttachment);
            case MTL::PixelFormatRGBA16Float:           return true;
            case MTL::PixelFormatRGBA32Sint:            return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRGBA32Uint:            return !(usage & ImageUsage::ResolveAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRGBA32Float:           return device->supportsFamily(MTL::GPUFamilyApple9) || (!(usage & ImageUsage::Filtered) && !(usage & ImageUsage::ResolveAttachment));
            default:                                    break;
        }

        return false;
    }

    #pragma clang diagnostic pop

    bool MetalDevice::IsImageSamplingSupported(const ImageSampling sampling) const
    {
        return device->supportsTextureSampleCount(MetalImage::ImageSamplingToUInteger(sampling));
    }

    ImageSampling MetalDevice::GetHighestImageSamplingSupported() const
    {
        return IsImageSamplingSupported(ImageSampling::x8) ? ImageSampling::x8 : ImageSampling::x4;
    }

    /* --- DESTRUCTOR --- */

    MetalDevice::~MetalDevice()
    {
        device->release();
    }

}