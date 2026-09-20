//
// Created by Nikolay Kanchevski on 11.11.24.
//

#pragma once

namespace SierraEngine
{

    namespace AutoModelLoader
    {
        [[nodiscard]] std::optional<LoadedModel> Load(const ModelLoadInfo& loadInfo) noexcept;
    }

}