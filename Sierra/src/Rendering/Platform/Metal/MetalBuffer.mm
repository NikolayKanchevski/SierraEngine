//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "MetalBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalBuffer::MetalBuffer(const MetalDevice& device, const BufferCreateInfo& createInfo)
        : Buffer(createInfo)
    {
        // Create buffer
        buffer = [device.GetMetalDevice() newBufferWithLength: createInfo.memorySize options: BufferMemoryLocationToResourceOptions(createInfo.memoryLocation)];
        SR_ERROR_IF(buffer == nil, "[Metal]: Failed to create buffer [{0}]!", createInfo.name);
        device.SetResourceName(buffer, createInfo.name);

        // Map and reset memory if CPU-visible
        if (createInfo.memoryLocation == BufferMemoryLocation::CPU) std::memset([buffer contents], 0, createInfo.memorySize);
    }

    /* --- POLLING METHODS --- */

    void MetalBuffer::CopyFromMemory(const void* memory, uint64 memoryByteSize, const uint64 sourceByteOffset, const uint64 destinationByteOffset)
    {
        memoryByteSize = memoryByteSize != 0 ? memoryByteSize : GetMemorySize();
        SR_ERROR_IF(destinationByteOffset + memoryByteSize > GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}], as the resulting memory space of a total of [{3}] bytes is bigger than the size of the buffer - [{4}]!", memoryByteSize, destinationByteOffset, GetName(), destinationByteOffset + memoryByteSize, GetMemorySize());
        std::memcpy(reinterpret_cast<uint8*>([buffer contents]) + destinationByteOffset, reinterpret_cast<const uint8*>(memory) + sourceByteOffset, memoryByteSize);
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalBuffer::GetName() const
    {
        return { [buffer.label UTF8String], [buffer.label length] };
    }

    std::span<const uint8> MetalBuffer::GetMemory() const
    {
        return { reinterpret_cast<const uint8*>([buffer contents]), [buffer length] };
    }

    uint64 MetalBuffer::GetMemorySize() const
    {
        return [buffer length];
    }

    /* --- DESTRUCTOR --- */

    MetalBuffer::~MetalBuffer()
    {
        [buffer release];
    }

    /* --- CONVERSIONS --- */

    MTLResourceOptions MetalBuffer::BufferMemoryLocationToResourceOptions(const BufferMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case BufferMemoryLocation::CPU:      return MTLResourceStorageModeShared | MTLResourceHazardTrackingModeUntracked | MTLResourceCPUCacheModeDefaultCache;
            case BufferMemoryLocation::GPU:      return MTLResourceStorageModePrivate | MTLResourceHazardTrackingModeUntracked | MTLResourceCPUCacheModeWriteCombined;
        }

        return MTLResourceStorageModeShared | MTLResourceHazardTrackingModeUntracked | MTLResourceCPUCacheModeDefaultCache;
    }

}
