//
// Created by Nikolay Kanchevski on 5.12.23.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLBuffer = void;
        using MTLResourceOptions = ulong;
    }
#endif

#include "../Buffer.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API MTLResourceOptions BufferMemoryLocationToResourceOptions(BufferMemoryLocation memoryLocation) noexcept;

    class SIERRA_API MetalBuffer final : public Buffer, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalBuffer(const MetalDevice& device, const BufferCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Write(const void* memory, size sourceOffset, size destinationOffset, size memorySize) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override;

        [[nodiscard]] void* GetMemory() const noexcept override;
        [[nodiscard]] size GetMemorySize() const noexcept override;

        [[nodiscard]] id<MTLBuffer> GetMetalBuffer() const noexcept { return buffer; }

        /* --- COPY SEMANTICS --- */
        MetalBuffer(const MetalBuffer&) = delete;
        MetalBuffer& operator=(const MetalBuffer&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalBuffer(MetalBuffer&&) = delete;
        MetalBuffer& operator=(MetalBuffer&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalBuffer() noexcept override;

    private:
        id<MTLBuffer> buffer = nil;

    };

}
