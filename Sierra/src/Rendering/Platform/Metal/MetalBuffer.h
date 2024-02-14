//
// Created by Nikolay Kanchevski on 5.12.23.
//

#pragma once

#include "../../Buffer.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalBuffer final : public Buffer, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalBuffer(const MetalDevice &device, const BufferCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void CopyFromMemory(const void* memoryPointer, uint64 memoryRange = 0, uint64 sourceByteOffset = 0, uint64 destinationByteOffset = 0) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const void* GetData() const override { SR_ERROR_IF(memoryLocation != BufferMemoryLocation::CPU, "[Metal]: Cannot get data pointer of buffer [{0}], as it is not CPU-visible!", GetName()); return buffer->contents(); }
        [[nodiscard]] inline uint64 GetMemorySize() const override { return buffer->length(); }
        [[nodiscard]] inline BufferMemoryLocation GetMemoryLocation() const override { return memoryLocation; }

        [[nodiscard]] inline MTL::Buffer* GetMetalBuffer() const { return buffer; }

        /* --- DESTRUCTOR --- */
        ~MetalBuffer() override;

        /* --- CONVERSIONS --- */
        static MTL::ResourceOptions BufferMemoryLocationToResourceOptions(BufferMemoryLocation memoryLocation);

    private:
        MTL::Buffer* buffer = nullptr;
        BufferMemoryLocation memoryLocation = BufferMemoryLocation::CPU;

    };

}
