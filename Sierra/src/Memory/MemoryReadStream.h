//
// Created by Nikolay Kanchevski on 24.12.24.
//

#pragma once

#import "../Utilities/Stream.h"

namespace Sierra
{

    class SIERRA_API MemoryReadStream final : public Stream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MemoryReadStream(std::span<const uint8> memory);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::vector<uint8> Read(size memorySize) override;
        void Write(const void* memory, size memorySize) override;

        /* --- SETTER METHODS --- */
        void SetOffset(size offset) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetOffset() const override { return offset; }
        [[nodiscard]] size GetSize() const override { return memory.size(); }
        [[nodiscard]] StreamAccess GetAccess() const noexcept override { return StreamAccess::ReadOnly; }

        /* --- COPY SEMANTICS --- */
        MemoryReadStream(const MemoryReadStream&) = delete;
        MemoryReadStream& operator=(const MemoryReadStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        MemoryReadStream(MemoryReadStream&&) noexcept = default;
        MemoryReadStream& operator=(MemoryReadStream&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MemoryReadStream() noexcept override = default;

    private:
        size offset = 0;
        std::span<const uint8> memory = { };
        
    };

}