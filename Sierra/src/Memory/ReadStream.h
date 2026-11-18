//
// Created by Nikolay Kanchevski on 24.12.24.
//

#pragma once

#include "../Utilities/Stream.h"

namespace Sierra
{

    class SIERRA_API ReadStream final : public Stream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ReadStream(std::span<const uint8> buffer);

        /* --- POLLING METHODS --- */
        [[nodiscard]] StreamResult Write(std::span<const uint8> buffer) override;
        StreamResult Read(std::span<uint8> outBuffer) override;

        /* --- SETTER METHODS --- */
        [[nodiscard]] StreamResult SetOffset(size offset) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetOffset() const override { return offset; }
        [[nodiscard]] size GetSize() const override { return buffer.size(); }
        [[nodiscard]] StreamAccess GetAccess() const noexcept override { return StreamAccess::ReadOnly; }

        /* --- COPY SEMANTICS --- */
        ReadStream(const ReadStream&) = delete;
        ReadStream& operator=(const ReadStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        ReadStream(ReadStream&&) noexcept = default;
        ReadStream& operator=(ReadStream&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~ReadStream() noexcept override = default;

    private:
        size offset = 0;
        std::span<const uint8> buffer = { };
        
    };

}