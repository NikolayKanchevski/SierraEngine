//
// Created by Nikolay Kanchevski on 11.11.24.
//

#pragma once

namespace SierraEngine
{

    struct ImageCreateInfo
    {
        uint32 width = 0;
        uint32 height = 0;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        std::vector<uint8> memory = { };
    };

    class SIERRA_ENGINE_API Image final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Image(const ImageCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::vector<uint8> Release() noexcept;

        /* --- SETTER METHODS --- */
        void SetPixel(Vector2UInt coordinate, Color64 color);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetWidth() const noexcept { return width; }
        [[nodiscard]] uint32 GetHeight() const noexcept { return height; }

        [[nodiscard]] Sierra::ImageFormat GetFormat() const noexcept { return format; }
        [[nodiscard]] Color64 GetPixel(Vector2UInt coordinate) const;

        [[nodiscard]] const void* GetMemory() const noexcept { return memory.data(); }
        [[nodiscard]] size GetMemorySize() const noexcept { return memory.size(); }

        /* --- COPY SEMANTICS --- */
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        /* --- MOVE SEMANTICS --- */
        Image(Image&&) noexcept = default;
        Image& operator=(Image&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Image() noexcept = default;

    private:
        uint32 width = 0;
        uint32 height = 0;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        std::vector<uint8> memory = { };

    };

    struct ImageLevel
    {
        std::span<const Image> layers = { };
    };

}