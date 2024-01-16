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
        template<typename T>
        inline void CopyFromMemory(const T &memory, const uint64 sourceOffset = 0, const uint64 destinationOffset = 0) { CopyFromMemory(&memory, sizeof(T), sourceOffset, destinationOffset); }
        virtual void CopyFromMemory(const void* memoryPointer, uint64 memorySize = 0, uint64 sourceOffset = 0, uint64 destinationOffset = 0) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint64 GetMemorySize() const { return memorySize; };
        [[nodiscard]] inline const void* GetData() const { return data; }
        template<typename T>
        [[nodiscard]] inline const T& GetDataAs() const { return *reinterpret_cast<T*>(data); }

        /* --- OPERATORS --- */
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Buffer() = default;

    protected:
        explicit Buffer(const BufferCreateInfo &createInfo);
        void* data = nullptr;

    private:
        uint64 memorySize = 0;

    };

}
