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
        Vertex                = 0x0020
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
        uint64 memorySize = 0;
        BufferUsage usage = BufferUsage::Undefined;
        BufferMemoryLocation memoryLocation = BufferMemoryLocation::CPU;
    };

    class SIERRA_API Buffer : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void CopyFromMemory(const void* memoryPointer, uint64 memoryRange = 0, uint64 sourceByteOffset = 0, uint64 destinationByteOffset = 0) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const void* GetData() const = 0;
        template<typename T>
        [[nodiscard]] inline const T& GetDataAs() const { return *reinterpret_cast<const T*>(GetData()); }
        [[nodiscard]] virtual uint64 GetMemorySize() const = 0;

        /* --- OPERATORS --- */
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Buffer() = default;

    protected:
        explicit Buffer(const BufferCreateInfo &createInfo);

    };

}
