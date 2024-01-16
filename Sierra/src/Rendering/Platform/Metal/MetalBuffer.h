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
        void CopyFromMemory(const void* memoryPointer, uint64 memoryRange = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MTL::Buffer* GetMetalBuffer() const { return buffer; }

        /* --- DESTRUCTOR --- */
        ~MetalBuffer() override;

        /* --- CONVERSIONS --- */
        static MTL::ResourceOptions BufferMemoryLocationToResourceOptions(BufferMemoryLocation memoryLocation);

    private:
        MTL::Buffer* buffer = nullptr;

    };

}
