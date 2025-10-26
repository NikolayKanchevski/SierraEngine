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

    class SIERRA_API Stream
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::vector<uint8> Read(size memorySize) = 0;
        [[nodiscard]] std::vector<uint8> Read(size sourceOffset, size memorySize);

        template<typename T>
        [[nodiscard]] T ReadAs()
        {
            return reinterpret_cast<const T&>(Read(sizeof(T)).front());
        }

        virtual void Write(const void* memory, size memorySize);
        void Write(const void* memory, size destinationOffset, size memorySize);

        void WriteMemory(std::span<const uint8> memory);
        void WriteMemory(std::span<const uint8> memory, size destinationOffset);

        template<typename T>
        void Write(const T value)
        {
            Write(&value, sizeof(T));
        }

        [[nodiscard]] std::vector<uint8> Peek(size memorySize);
        [[nodiscard]] std::vector<uint8> Peek(size sourceOffset, size memorySize);

        [[nodiscard]] std::vector<uint8> ReadAll();
        [[nodiscard]] std::vector<uint8> ReadToEnd();

        /* --- SETTER METHODS --- */
        virtual void SetOffset(size offset);

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