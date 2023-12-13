//
// Created by Nikolay Kanchevski on 5.12.23.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    enum class BufferUsage : uint8
    {
        Undefined                = 0x0000,
        SourceTransfer           = 0x0001,
        DestinationTransfer      = 0x0002,
        Uniform                  = 0x0004,
        Storage                  = 0x0008,
        Index                    = 0x0010,
        Vertex                   = 0x0020
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(BufferUsage);

    enum class BufferMemoryLocation : uint8
    {
        Host,
        Device,
        Auto
    };

    struct BufferCreateInfo
    {
        const std::string &name = "Buffer";
        uint64 memorySize = 0;
        BufferUsage usage = BufferUsage::Undefined;
        BufferMemoryLocation memoryLocation = BufferMemoryLocation::Auto;
    };

    class SIERRA_API Buffer : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void CopyFromMemory(const void* memoryPointer, uint64 memorySize = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint64 GetMemorySize() const { return memorySize; };

        /* --- OPERATORS --- */
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

    protected:
        explicit Buffer(const BufferCreateInfo &createInfo);

    private:
        uint64 memorySize = 0;

    };

}
