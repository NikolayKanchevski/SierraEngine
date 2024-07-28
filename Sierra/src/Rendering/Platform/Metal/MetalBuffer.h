//
// Created by Nikolay Kanchevski on 5.12.23.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLBuffer = void;
        using MTLResourceOptions = std::uintptr_t;
    }
#endif

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
        void CopyFromMemory(const void* memory, uint64 memoryByteSize = 0, uint64 sourceByteOffset = 0, uint64 destinationByteOffset = 0) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override;

        [[nodiscard]] std::span<const uint8> GetMemory() const override;
        [[nodiscard]] uint64 GetMemorySize() const override;

        [[nodiscard]] id<MTLBuffer> GetMetalBuffer() const { return buffer; }

        /* --- DESTRUCTOR --- */
        ~MetalBuffer() override;

        /* --- CONVERSIONS --- */
        static MTLResourceOptions BufferMemoryLocationToResourceOptions(BufferMemoryLocation memoryLocation);

    private:
        id<MTLBuffer> buffer = nil;

    };

}
