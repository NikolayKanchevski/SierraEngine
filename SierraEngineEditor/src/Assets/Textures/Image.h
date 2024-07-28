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

    struct ImageCreateInfo
    {
        ImageLoadChannels loadChannels = ImageLoadChannels::All;
        std::span<const uint8> compressedMemory = { };
    };

    class Image
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Resize(uint32 width, uint32 height) = 0;
        virtual void Crop(Vector2UInt bottomLeft, Vector2UInt topRight) = 0;

        /* --- SETTER METHODS --- */
        virtual bool SetPixel(Vector2UInt coordinate, Color color) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetWidth() const = 0;
        [[nodiscard]] virtual uint32 GetHeight() const = 0;
        [[nodiscard]] virtual Sierra::ImageFormat GetFormat() const = 0;

        [[nodiscard]] virtual Color GetPixel(Vector2UInt coordinate) const = 0;
        [[nodiscard]] virtual std::span<const uint8> GetMemory() const = 0;
        [[nodiscard]] virtual uint64 GetMemorySize() const = 0;

        /* --- OPERATORS --- */
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Image() = default;

    protected:
        explicit Image(const ImageCreateInfo &createInfo);

        template<typename T> requires std::is_arithmetic_v<T>
        void SetPixelMemory(void* pixel, const uint8 channelCount, const Color color) const
        {
            constexpr Color::value_type MULTIPLIER = std::is_floating_point_v<T> ? 1 : std::numeric_limits<T>::max();
            for (int i = 0; i < channelCount; i++) reinterpret_cast<T*>(pixel)[i] = static_cast<T>(color[i] * MULTIPLIER);
        }

        template<typename T> requires std::is_arithmetic_v<T>
        void GetPixelColor(const void* pixel, const uint8 channelCount, Color &color) const
        {
            constexpr Color::value_type DIVISOR = std::is_floating_point_v<T> ? 1 : std::numeric_limits<T>::max();
            for (int i = 0; i < channelCount; i++) color[i] = static_cast<Color::value_type>(reinterpret_cast<const T*>(pixel)[i] / DIVISOR);
        }

    };

    struct ImageLayer
    {
        Image &image;
    };

    struct ImageLevel
    {
        std::span<const ImageLayer> layers = { };
    };

}
