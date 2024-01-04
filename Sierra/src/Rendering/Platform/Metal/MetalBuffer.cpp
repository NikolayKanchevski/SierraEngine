//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "MetalBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalBuffer::MetalBuffer(const MetalDevice &device, const BufferCreateInfo &createInfo)
        : Buffer(createInfo), MetalResource(createInfo.name), data(createInfo.memorySize, 0)
    {
        // Create buffer
        buffer = device.GetMetalDevice()->newBuffer(createInfo.memorySize, BufferMemoryLocationToResourceOptions(createInfo.memoryLocation));
        SR_ERROR_IF(buffer == nullptr, "[Metal]: Could not create buffer!");
        MTL_SET_RESOURCE_NAME(buffer, GetName().c_str());

        // Map and reset memory
        data.GetData() = buffer->contents();
        data.SetMemory(0);
    }

    /* --- POLLING METHODS --- */

    void MetalBuffer::CopyFromMemory(const void* memoryPointer, const uint64 memorySize, const uint64 sourceOffset, const uint64 destinationOffset)
    {
        data.CopyFromMemory(memoryPointer, memorySize, sourceOffset, destinationOffset);
        buffer->didModifyRange(NS::Range::Make(destinationOffset, memorySize));
    }

    /* --- DESTRUCTOR --- */

    MetalBuffer::~MetalBuffer()
    {
        buffer->release();
    }

    /* --- CONVERSIONS --- */

    MTL::ResourceOptions MetalBuffer::BufferMemoryLocationToResourceOptions(const BufferMemoryLocation memoryLocation)
    {
        switch (memoryLocation)
        {
            case BufferMemoryLocation::Host:        return MTL::ResourceStorageModeManaged;
            case BufferMemoryLocation::Device:      return MTL::ResourceStorageModePrivate;
            case BufferMemoryLocation::Auto:        return MTL::ResourceStorageModeManaged;
        }

        return MTL::ResourceStorageModeManaged;
    }

}