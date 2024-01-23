//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "MetalBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalBuffer::MetalBuffer(const MetalDevice &device, const BufferCreateInfo &createInfo)
        : Buffer(createInfo), MetalResource(createInfo.name)
    {
        // Create buffer
        buffer = device.GetMetalDevice()->newBuffer(createInfo.memorySize, BufferMemoryLocationToResourceOptions(createInfo.memoryLocation));
        SR_ERROR_IF(buffer == nullptr, "[Metal]: Failed to create buffer [{0}]!", GetName());
        device.SetResourceName(buffer, GetName());

        // Map and reset memory
        data = buffer->contents();
        memset(data, 0, createInfo.memorySize);
    }

    /* --- POLLING METHODS --- */

    void MetalBuffer::CopyFromMemory(const void* memoryPointer, uint64 memoryRange, const uint64 sourceOffset, const uint64 destinationOffset)
    {
        memoryRange = memoryRange != 0 ? memoryRange : GetMemorySize();
        SR_ERROR_IF(destinationOffset + memoryRange > GetMemorySize(), "[Metal]: Cannot copy [{0}] bytes of memory, which is offset by another [{1}] bytes, to buffer [{2}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, destinationOffset, GetName(), destinationOffset + memoryRange, GetMemorySize());

        std::memcpy(reinterpret_cast<char*>(data) + destinationOffset, reinterpret_cast<const char*>(memoryPointer) + sourceOffset, memoryRange);
        buffer->didModifyRange(NS::Range::Make(destinationOffset, memoryRange));
    }

    /* --- DESTRUCTOR --- */

    MetalBuffer::~MetalBuffer()
    {
        buffer->release();
    }

    /* --- CONVERSIONS --- */

    MTL::ResourceOptions MetalBuffer::BufferMemoryLocationToResourceOptions(const BufferMemoryLocation memoryLocation)
    {
        #if SR_PLATFORM_macOS
            constexpr MTL::ResourceOptions SHARED_STORAGE_RESOURCE_OPTION = MTL::ResourceStorageModeManaged;
        #else
            constexpr MTL::ResourceOptions SHARED_STORAGE_RESOURCE_OPTION = MTL::ResourceStorageModeShared;
        #endif

        switch (memoryLocation)
        {
            case BufferMemoryLocation::Host:        return SHARED_STORAGE_RESOURCE_OPTION | MTL::ResourceHazardTrackingModeUntracked | MTL::ResourceCPUCacheModeDefaultCache;
            case BufferMemoryLocation::Device:      return MTL::ResourceStorageModePrivate | MTL::ResourceHazardTrackingModeUntracked | MTL::ResourceCPUCacheModeWriteCombined;
            case BufferMemoryLocation::Auto:        return SHARED_STORAGE_RESOURCE_OPTION | MTL::ResourceHazardTrackingModeUntracked | MTL::ResourceCPUCacheModeDefaultCache;
        }

        return SHARED_STORAGE_RESOURCE_OPTION | MTL::ResourceHazardTrackingModeUntracked | MTL::ResourceCPUCacheModeDefaultCache;
    }

}
