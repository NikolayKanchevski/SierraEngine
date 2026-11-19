//
// Created by Nikolay Kanchevski on 18.11.26.
//

#pragma once

namespace SierraEngine
{

    namespace AutoShaderImporter
    {
        [[nodiscard]] std::optional<ImportedShader> Import(const ShaderImportInfo& importInfo) noexcept;
    }

}
