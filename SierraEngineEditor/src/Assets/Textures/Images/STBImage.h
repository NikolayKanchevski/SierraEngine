//
// Created by Nikolay Kanchevski on 20.07.24.
//

#pragma once

#include "../RawImage.h"

namespace SierraEngine
{

    class STBImage final : public RawImage
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit STBImage(const RawImageCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Resize(uint32 width, uint32 height) override;
        void Crop(Vector2UInt bottomLeft, Vector2UInt topRight) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetWidth() const noexcept override { return width; }
        [[nodiscard]] uint32 GetHeight() const noexcept override { return height; }
        [[nodiscard]] Sierra::ImageFormat GetFormat() const noexcept override { return format; }

        [[nodiscard]] void* GetMemory() const noexcept override { return memory.get(); }
        [[nodiscard]] size GetMemorySize() const noexcept override { return width * height * static_cast<uint8>(Sierra::ImageFormatToPixelMemorySize(format)); }

        /* --- COPY SEMANTICS --- */
        STBImage(const STBImage&) = delete;
        STBImage& operator=(const STBImage&) = delete;

        /* --- MOVE SEMANTICS --- */
        STBImage(STBImage&&) noexcept = default;
        STBImage& operator=(STBImage&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~STBImage() noexcept override = default;

    private:
        uint32 width = 0;
        uint32 height = 0;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        std::unique_ptr<void, void(*)(void*)> memory = { nullptr, std::free };

    };

}
