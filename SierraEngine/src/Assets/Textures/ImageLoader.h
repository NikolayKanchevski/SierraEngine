//
// Created by Nikolay Kanchevski on 11.11.24.
//

#pragma once

namespace SierraEngine
{

    enum class ImageLoadChannels : uint8
    {
        R,
        RG,
        RGB,
        RGBA,
        All
    };

    struct ImageLoadInfo
    {
        std::span<const uint8> memory = { };
        ImageLoadChannels loadChannels = ImageLoadChannels::All;
    };

    struct LoadedImage
    {
        uint32 width = 0;
        uint32 height = 0;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        std::vector<uint8> memory = { };
    };

    struct LoadedImageLevel
    {
        std::span<const LoadedImage> layers = { };
    };

    class SIERRA_ENGINE_API ImageLoader
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<LoadedImage> Load(const ImageLoadInfo& loadInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        ImageLoader(const ImageLoader&) = delete;
        ImageLoader& operator=(const ImageLoader&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ImageLoader() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ImageLoader() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        ImageLoader(ImageLoader&&) noexcept = default;
        ImageLoader& operator=(ImageLoader&&) noexcept = default;

    };

}