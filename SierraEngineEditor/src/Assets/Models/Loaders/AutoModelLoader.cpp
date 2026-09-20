//
// Created by Nikolay Kanchevski on 11.11.24.
//

#include "AutoModelLoader.h"

#include "glTFModelLoader.h"

#define TRY_LOADER(TYPE) if (std::optional<LoadedModel> model = TYPE().Load(loadInfo); model.has_value()) return model;

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    std::optional<LoadedModel> AutoModelLoader::Load(const ModelLoadInfo& loadInfo) noexcept
    {
        TRY_LOADER(glTFModelLoader);

        APP_WARNING("Failed to automatically load model, as its format could not be deduced");
        return std::nullopt;
    }

}