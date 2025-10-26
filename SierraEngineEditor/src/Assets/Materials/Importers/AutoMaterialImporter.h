//
// Created by Nikolay Kanchevski on 28.12.24.
//

#pragma once

namespace SierraEngine
{

    namespace AutoMaterialImporter
    {
        [[nodiscard]] std::optional<ImportedMaterial> Import(const MaterialImportInfo& importInfo) noexcept;
    };

}