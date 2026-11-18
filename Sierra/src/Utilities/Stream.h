//
// Created by Nikolay Kanchevski on 24.12.24.
//

#pragma once

namespace Sierra
{

    enum class StreamAccess : uint8
    {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };

    enum class StreamResult : uint8
    {
        Unknown,
        Success,
        InvalidAccess,
        InvalidOffset,
        InvalidRange
    };

    class SIERRA_API Stream
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual StreamResult Write(std::span<const uint8> memory);
        virtual StreamResult Read(std::span<uint8> outMemory);

        [[nodiscard]] StreamResult ReadToEnd(std::vector<uint8>& outMemory);
        [[nodiscard]] StreamResult ReadAll(std::vector<uint8>& outMemory);

        template<typename T>
        StreamResult ReadAs(T& outT)
        {
            std::vector<uint8> buffer = { };
            buffer.resize(sizeof(T));

            if (const StreamResult result = Read(buffer); result != StreamResult::Success)
                return result;

            outT = reinterpret_cast<const T&>(buffer.front());
            return StreamResult::Success;
        }

        template<typename T>
        [[nodiscard]] StreamResult WriteAs(const T& value)
        {
            return Write({ reinterpret_cast<const uint8*>(&value), sizeof(T) });
        }

        /* --- SETTER METHODS --- */
        [[nodiscard]] virtual StreamResult SetOffset(size offset);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual size GetOffset() const = 0;
        [[nodiscard]] virtual size GetSize() const = 0;
        [[nodiscard]] virtual StreamAccess GetAccess() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        Stream(const Stream&) = delete;
        Stream& operator=(const Stream&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Stream() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        Stream() = default;

        /* --- MOVE SEMANTICS --- */
        Stream(Stream&&) noexcept = default;
        Stream& operator=(Stream&&) noexcept = default;

    };

}