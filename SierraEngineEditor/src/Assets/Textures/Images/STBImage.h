//
// Created by Nikolay Kanchevski on 20.07.24.
//

#pragma once

#include "../Image.h"

namespace SierraEngine
{

    class STBImage final : public Image
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit STBImage(const ImageCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Resize(uint32 width, uint32 height) override;
        void Crop(Vector2UInt bottomLeft, Vector2UInt topRight) override;

        /* --- SETTER METHODS --- */
        bool SetPixel(Vector2UInt pixel, Color color) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetWidth() const override { return width; }
        [[nodiscard]] uint32 GetHeight() const override { return height; }
        [[nodiscard]] Sierra::ImageFormat GetFormat() const override { return format; }

        [[nodiscard]] Color GetPixel(Vector2UInt coordinate) const override;
        [[nodiscard]] std::span<const uint8> GetMemory() const override { return { reinterpret_cast<const uint8*>(memory.get()), GetMemorySize() }; }
        [[nodiscard]] uint64 GetMemorySize() const override { return width * height * static_cast<uint8>(Sierra::ImageFormatToPixelMemorySize(format)); }

        /* --- DESTRUCTOR --- */
        ~STBImage() override = default;

    private:
        uint32 width = 0;
        uint32 height = 0;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        std::unique_ptr<void, void(*)(void*)> memory = { nullptr, std::free };

    };

}
