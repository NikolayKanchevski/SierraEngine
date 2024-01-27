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

        // Save device name
        deviceName = device->name()->utf8String();

        // Create command queue
        commandQueue = device->newCommandQueue();
        SR_ERROR_IF(commandQueue == nullptr, "[Metal]: Could not create command queue for device [{0}]!", GetName());
    }

    /* --- POLLING METHODS --- */

    void MetalDevice::SubmitCommandBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::initializer_list<std::reference_wrapper<std::unique_ptr<CommandBuffer>>> &commandBuffersToWait) const
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
        metalCommandBuffer.GetMetalCommandBuffer()->commit();
    }

    void MetalDevice::WaitForCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot, from device [{0}], wait for command buffer [{1}] with a graphics API, that differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<MetalCommandBuffer&>(*commandBuffer);

        // Wait for completion
        while (!metalCommandBuffer.HasFinishedExecution());
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
            case MTL::PixelFormatR8Sint:                return !(usage & ImageUsage::ResolvedAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR8Uint:                return !(usage & ImageUsage::ResolvedAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR8Unorm:               return true;
            case MTL::PixelFormatR8Unorm_sRGB:          return true;
            case MTL::PixelFormatR16Sint:               return !(usage & ImageUsage::ResolvedAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR16Uint:               return !(usage & ImageUsage::ResolvedAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR16Snorm:              return !(usage & ImageUsage::ResolvedAttachment);
            case MTL::PixelFormatR16Unorm:              return !(usage & ImageUsage::ResolvedAttachment);
            case MTL::PixelFormatR16Float:              return true;
            case MTL::PixelFormatR32Sint:               return !(usage & ImageUsage::ResolvedAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR32Uint:               return !(usage & ImageUsage::ResolvedAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatR32Float:              return device->supportsFamily(MTL::GPUFamilyApple8) || !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG8Sint:               return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG8Uint:               return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG8Unorm:              return true;
            case MTL::PixelFormatRG8Unorm_sRGB:         return device->supportsFamily(MTL::GPUFamilyApple2);
            case MTL::PixelFormatRG16Sint:              return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG16Uint:              return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG16Snorm:             return !(usage & ImageUsage::ResolvedAttachment);
            case MTL::PixelFormatRG16Unorm:             return !(usage & ImageUsage::ResolvedAttachment);
            case MTL::PixelFormatRG16Float:             return true;
            case MTL::PixelFormatRG32Sint:              return device->supportsFamily(MTL::GPUFamilyApple7) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG32Uint:              return device->supportsFamily(MTL::GPUFamilyApple7) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRG32Float:             return device->supportsFamily(MTL::GPUFamilyApple9);
            case MTL::PixelFormatRGBA8Sint:             return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRGBA8Uint:             return !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRGBA8Unorm:            return true;
            case MTL::PixelFormatRGBA8Unorm_sRGB:       return true;
            case MTL::PixelFormatRGBA16Sint:            return !(usage & ImageUsage::ColorAttachment) && !(usage & ImageUsage::DepthAttachment) && !(usage & ImageUsage::ResolvedAttachment);
            case MTL::PixelFormatRGBA16Uint:            return !(usage & ImageUsage::ColorAttachment) && !(usage & ImageUsage::DepthAttachment) && !(usage & ImageUsage::ResolvedAttachment);
            case MTL::PixelFormatRGBA16Snorm:           return !(usage & ImageUsage::ResolvedAttachment);
            case MTL::PixelFormatRGBA16Unorm:           return !(usage & ImageUsage::ResolvedAttachment);
            case MTL::PixelFormatRGBA16Float:           return true;
            case MTL::PixelFormatRGBA32Sint:            return !(usage & ImageUsage::ResolvedAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRGBA32Uint:            return !(usage & ImageUsage::ResolvedAttachment) && !(usage & ImageUsage::Filtered);
            case MTL::PixelFormatRGBA32Float:           return device->supportsFamily(MTL::GPUFamilyApple9) || (!(usage & ImageUsage::Filtered) && !(usage & ImageUsage::ResolvedAttachment));
            case MTL::PixelFormatBGRA8Unorm:            return true;
            case MTL::PixelFormatBGRA8Unorm_sRGB:       return true;
            case MTL::PixelFormatDepth16Unorm:          return device->supportsFamily(MTL::GPUFamilyApple3) || !(usage & ImageUsage::ResolvedAttachment);
            case MTL::PixelFormatDepth32Float:          return (device->supportsFamily(MTL::GPUFamilyApple3) || !(usage & ImageUsage::ResolvedAttachment)) && (device->supportsFamily(MTL::GPUFamilyMac2) || !(usage & ImageUsage::Filtered));
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