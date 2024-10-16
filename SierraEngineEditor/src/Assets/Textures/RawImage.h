//
// Created by Nikolay Kanchevski on 20.07.24.
//

#pragma once

namespace SierraEngine
{

    enum class ImageLoadChannels : uint8
    {
        All,
        R,
        RG,
        RGB,
        RGBA
    };

    struct RawImageCreateInfo
    {
        ImageLoadChannels loadChannels = ImageLoadChannels::All;
        std::span<const uint8> compressedMemory = { };
    };

    class RawImage
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Resize(uint32 width, uint32 height) = 0;
        virtual void Crop(Vector2UInt bottomLeft, Vector2UInt topRight) = 0;

        /* --- SETTER METHODS --- */
        bool SetPixel(Vector2UInt pixelCoordinate, Color64 color);

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetWidth() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetHeight() const noexcept = 0;
        [[nodiscard]] virtual Sierra::ImageFormat GetFormat() const noexcept = 0;
        [[nodiscard]] Color64 GetPixel(Vector2UInt pixelCoordinate) const;

        [[nodiscard]] virtual void* GetMemory() const noexcept = 0;
        [[nodiscard]] virtual size GetMemorySize() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        RawImage(const RawImage&) = delete;
        RawImage& operator=(const RawImage&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~RawImage() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit RawImage(const RawImageCreateInfo& createInfo);

        /* --- MOVE SEMANTICS --- */
        RawImage(RawImage&&) noexcept = default;
        RawImage& operator=(RawImage&&) noexcept = default;
    };

    struct RawImageLayer
    {
        RawImage& image;
    };

    struct ImageLevel
    {
        std::span<const RawImageLayer> layers = { };
    };

}
