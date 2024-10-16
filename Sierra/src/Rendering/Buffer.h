//
// Created by Nikolay Kanchevski on 5.12.23.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    enum class BufferUsage : uint8
    {
        Undefined             = 0x0000,
        SourceMemory          = 0x0001,
        DestinationMemory     = 0x0002,
        Uniform               = 0x0004,
        Storage               = 0x0008,
        Index                 = 0x0010,
        Vertex                = 0x0020,
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(BufferUsage);

    enum class BufferMemoryLocation : bool
    {
        CPU,
        GPU
    };

    struct BufferCreateInfo
    {
        std::string_view name = "Buffer";
        size memorySize = 0;
        BufferUsage usage = BufferUsage::Undefined;
        BufferMemoryLocation memoryLocation = BufferMemoryLocation::CPU;
    };

    class SIERRA_API Buffer : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Write(const void* memory, size sourceOffset, size destinationOffset, size memorySize);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual void* GetMemory() const noexcept = 0;
        [[nodiscard]] virtual size GetMemorySize() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        /* --- MOVE SEMANTICS --- */
        Buffer(Buffer&&) = delete;
        Buffer& operator=(Buffer&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Buffer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Buffer(const BufferCreateInfo& createInfo);

    };

}
