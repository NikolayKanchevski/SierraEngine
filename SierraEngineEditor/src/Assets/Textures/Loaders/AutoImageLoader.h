//
// Created by Nikolay Kanchevski on 11.11.24.
//

#pragma once

namespace SierraEngine
{

    namespace AutoImageLoader
    {
        [[nodiscard]] std::optional<LoadedImage> Load(const ImageLoadInfo& loadInfo) noexcept;
    }

}