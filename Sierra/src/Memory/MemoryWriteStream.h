//
// Created by Nikolay Kanchevski on 24.12.24.
//

#pragma once

#include "../Utilities/Stream.h"

namespace Sierra
{

    class SIERRA_API MemoryWriteStream final : public Stream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MemoryWriteStream(size capacity = 0);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::vector<uint8> Read(size memorySize) override;
        void Write(const void* memory, size memorySize) override;

        std::vector<uint8> Release() noexcept;

        /* --- SETTER METHODS --- */
        void SetOffset(size offset) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetOffset() const override { return offset; }
        [[nodiscard]] size GetSize() const override { return memory.size(); }
        [[nodiscard]] StreamAccess GetAccess() const noexcept override { return StreamAccess::ReadWrite; }

        /* --- COPY SEMANTICS --- */
        MemoryWriteStream(const MemoryWriteStream&) = delete;
        MemoryWriteStream& operator=(const MemoryWriteStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        MemoryWriteStream(MemoryWriteStream&&) noexcept = default;
        MemoryWriteStream& operator=(MemoryWriteStream&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MemoryWriteStream() noexcept override = default;

    private:
        size offset = 0;
        std::vector<uint8> memory = { };

    };

}