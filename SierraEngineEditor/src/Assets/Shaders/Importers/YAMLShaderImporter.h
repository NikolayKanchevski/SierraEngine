//
// Created by Nikolay Kanchevski on 18.11.26.
//

#pragma once

#include "../../Importers/YAMLImporter.h"

namespace SierraEngine
{

    class YAMLShaderImporter final : public ShaderImporter, public YAMLImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        YAMLShaderImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedShader> Import(const ShaderImportInfo& importInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 1, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        YAMLShaderImporter(const YAMLShaderImporter&) = delete;
        YAMLShaderImporter& operator=(const YAMLShaderImporter&) = delete;

        /* --- MOVE SEMANTICS --- */
        YAMLShaderImporter(YAMLShaderImporter&&) noexcept = default;
        YAMLShaderImporter& operator=(YAMLShaderImporter&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~YAMLShaderImporter() noexcept override = default;

    };

}
