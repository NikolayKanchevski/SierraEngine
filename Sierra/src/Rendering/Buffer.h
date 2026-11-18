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
        RAM,
        GPU
    };

    struct BufferCreateInfo
    {
        std::string_view name = "Buffer";
        uint64 memorySize = 0;
        BufferUsage usage = BufferUsage::Undefined;
        BufferMemoryLocation memoryLocation = BufferMemoryLocation::GPU;
    };

    class SIERRA_API Buffer : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual bool Write(uint64 destinationOffset, std::span<const uint8> buffer);

        template<typename T>
        bool WriteAs(const uint64 destinationOffset, const T& t)
        {
            return Write(destinationOffset, { reinterpret_cast<const uint8*>(&t), sizeof(T) });
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint64 GetMemorySize() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        /* --- MOVE SEMANTICS --- */
        Buffer(Buffer&&) noexcept = default;
        Buffer& operator=(Buffer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Buffer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Buffer(const BufferCreateInfo& createInfo);

    };

}
