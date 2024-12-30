//
// Created by Nikolay Kanchevski on 11.11.24.
//

#pragma once

namespace SierraEngine
{

    class AutoImageLoader final : public ImageLoader
    {
    public:
        /* --- CONSTRUCTORS --- */
        AutoImageLoader() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<Image> Load(const ImageLoadInfo& loadInfo) const noexcept override;

        /* --- COPY SEMANTICS --- */
        AutoImageLoader(const AutoImageLoader&) = delete;
        AutoImageLoader& operator=(const AutoImageLoader&) = delete;

        /* --- MOVE SEMANTICS --- */
        AutoImageLoader(AutoImageLoader&&) noexcept = default;
        AutoImageLoader& operator=(AutoImageLoader&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~AutoImageLoader() noexcept = default;
    };

}