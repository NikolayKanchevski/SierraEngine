//
// Created by Nikolay Kanchevski on 24.12.24.
//

#pragma once

#include "../Utilities/Stream.h"

namespace Sierra
{

    class SIERRA_API WriteStream final : public Stream
    {
    public:
        /* --- CONSTRUCTORS --- */
        WriteStream(size capacity = 0);

        /* --- POLLING METHODS --- */
        [[nodiscard]] StreamResult Write(std::span<const uint8> buffer) override;
        StreamResult Read(std::span<uint8> outBuffer) override;

        void Resize(size memorySize) noexcept;
        std::vector<uint8> Release() noexcept;

        /* --- SETTER METHODS --- */
        [[nodiscard]] StreamResult SetOffset(size offset) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetOffset() const override { return offset; }
        [[nodiscard]] size GetSize() const override { return buffer.size(); }
        [[nodiscard]] StreamAccess GetAccess() const noexcept override { return StreamAccess::ReadWrite; }

        /* --- COPY SEMANTICS --- */
        WriteStream(const WriteStream&) = delete;
        WriteStream& operator=(const WriteStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        WriteStream(WriteStream&&) noexcept = default;
        WriteStream& operator=(WriteStream&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~WriteStream() noexcept override = default;

    private:
        size offset = 0;
        std::vector<uint8> buffer = { };

    };

}