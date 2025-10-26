//
// Created by Nikolay Kanchevski on 28.12.24.
//

#pragma once

namespace SierraEngine
{

    namespace AutoTextureImporter
    {
        [[nodiscard]] std::optional<ImportedTexture> Import(const TextureImportInfo& importInfo) noexcept;
    }

}