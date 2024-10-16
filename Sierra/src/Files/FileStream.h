//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once


namespace Sierra
{

    enum class FileStreamAccess : uint8
    {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };

    enum class FileStreamBuffering : uint8
    {
        Buffered,
        Unbuffered
    };

    class SIERRA_API FileStream
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Seek(size offset) = 0;
        void SeekToEnd();

        [[nodiscard]] virtual std::vector<uint8> Read(size memorySize) = 0;
        [[nodiscard]] std::vector<uint8> Read(size offset, size memorySize);

        virtual void Write(const void* memory, size sourceOffset, size memorySize) = 0;
        void Write(const void* memory, size sourceOffset, size destinationOffset, size memorySize);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual size GetCurrentOffset() const = 0;
        [[nodiscard]] virtual size GetMemorySize() const = 0;
        [[nodiscard]] virtual const std::filesystem::path& GetFilePath() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        FileStream(const FileStream&) = delete;
        FileStream& operator=(const FileStream&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~FileStream() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        FileStream() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        FileStream(FileStream&&) noexcept = default;
        FileStream& operator=(FileStream&&) noexcept = default;

    };

}
