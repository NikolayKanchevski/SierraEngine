//
// Created by Nikolay Kanchevski on 11.11.24.
//

#pragma once

namespace SierraEngine
{

    class STBImageLoader final : public ImageLoader
    {
    public:
        /* --- CONSTRUCTORS --- */
        STBImageLoader() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<LoadedImage> Load(const ImageLoadInfo& loadInfo) const noexcept override;

        /* --- COPY SEMANTICS --- */
        STBImageLoader(const STBImageLoader&) = delete;
        STBImageLoader& operator=(const STBImageLoader&) = delete;

        /* --- MOVE SEMANTICS --- */
        STBImageLoader(STBImageLoader&&) noexcept = default;
        STBImageLoader& operator=(STBImageLoader&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~STBImageLoader() noexcept override = default;
    };

}