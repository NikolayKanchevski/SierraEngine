//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "MetalBuffer.h"

#include "../DeviceErrors.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    MTLResourceOptions BufferMemoryLocationToResourceOptions(const BufferMemoryLocation memoryLocation) noexcept
    {
        switch (memoryLocation)
        {
            case BufferMemoryLocation::RAM:      return MTLResourceStorageModeShared | MTLResourceHazardTrackingModeUntracked | MTLResourceCPUCacheModeDefaultCache;
            case BufferMemoryLocation::GPU:      return MTLResourceStorageModePrivate | MTLResourceHazardTrackingModeUntracked | MTLResourceCPUCacheModeWriteCombined;
        }

        return MTLResourceStorageModeShared | MTLResourceHazardTrackingModeUntracked | MTLResourceCPUCacheModeDefaultCache;
    }

    /* --- CONSTRUCTORS --- */

    MetalBuffer::MetalBuffer(const MetalDevice& device, const BufferCreateInfo& createInfo)
        : MetalResource(createInfo.name), Buffer(createInfo)
    {
        SR_THROW_IF(createInfo.usage & BufferUsage::Uniform && createInfo.memorySize > device.GetLimits().maxUniformBufferSize, ValueOutOfRangeError(SR_FORMAT("Cannot create buffer [{0}], as specified memory size is greater than device [{1}]'s max uniform buffer size - use Device::GetLimits() to query limits", createInfo.name, device.GetName()), createInfo.memorySize, static_cast<uint64>(0), device.GetLimits().maxUniformBufferSize));
        SR_THROW_IF(createInfo.usage & BufferUsage::Storage && createInfo.memorySize > device.GetLimits().maxStorageBufferSize, ValueOutOfRangeError(SR_FORMAT("Cannot create buffer [{0}], as specified memory size is greater than device [{1}]'s max storage buffer size - use Device::GetLimits() to query limits", createInfo.name, device.GetName()), createInfo.memorySize, static_cast<uint64>(0), device.GetLimits().maxUniformBufferSize));

        // Create buffer
        buffer = [device.GetMetalDevice() newBufferWithLength: createInfo.memorySize options: BufferMemoryLocationToResourceOptions(createInfo.memoryLocation)];
        SR_THROW_IF(buffer == nil, UnknownDeviceError(SR_FORMAT("Could not create buffer [{0}]", createInfo.name)));
        device.SetResourceName(buffer, createInfo.name);

        // Map and reset memory if CPU-visible
        if (createInfo.memoryLocation == BufferMemoryLocation::RAM) std::memset([buffer contents], 0, createInfo.memorySize);
    }

    /* --- POLLING METHODS --- */

    bool MetalBuffer::Write(const uint64 destinationOffset, const std::span<const uint8> givenBuffer)
    {
        if (!Buffer::Write(destinationOffset, givenBuffer))
            return false;

        if (buffer.contents == nil)
            return false;

        std::memcpy(reinterpret_cast<uint8*>(buffer.contents) + destinationOffset, givenBuffer.data(), givenBuffer.size());
        return true;
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalBuffer::GetName() const noexcept
    {
        return { buffer.label.UTF8String, buffer.label.length };
    }

    uint64 MetalBuffer::GetMemorySize() const noexcept
    {
        return buffer.length;
    }

    /* --- DESTRUCTOR --- */

    MetalBuffer::~MetalBuffer() noexcept
    {
        [buffer release];
    }

}
