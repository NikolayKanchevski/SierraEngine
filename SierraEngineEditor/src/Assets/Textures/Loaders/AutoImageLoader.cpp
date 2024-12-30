//
// Created by Nikolay Kanchevski on 11.11.24.
//

#include "AutoImageLoader.h"

#include "STBImageLoader.h"

#define TRY_LOADER(TYPE) if (std::optional<Image> image = TYPE().Load(loadInfo); image.has_value()) return image;

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    std::optional<Image> AutoImageLoader::Load(const ImageLoadInfo& loadInfo) const noexcept
    {
        TRY_LOADER(STBImageLoader);

        APP_WARNING("Failed to automatically load image, as its format could not be deduced");
        return std::nullopt;
    }

}