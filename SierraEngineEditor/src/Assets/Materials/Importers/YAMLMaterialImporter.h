//
// Created by Nikolay Kanchevski on 22.11.24.
//

#pragma once

#include "../../Importers/YAMLImporter.h"

namespace SierraEngine
{

    class YAMLMaterialImporter final : public MaterialImporter, public YAMLImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        YAMLMaterialImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedMaterial> Import(const MaterialImportInfo& importInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetSignature GetSignature() const noexcept override { return { 'Y', 'A', 'M', 'T' }; }
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 1, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        YAMLMaterialImporter(const YAMLMaterialImporter&) = delete;
        YAMLMaterialImporter& operator=(const YAMLMaterialImporter&) = delete;

        /* --- MOVE SEMANTICS --- */
        YAMLMaterialImporter(YAMLMaterialImporter&&) noexcept = default;
        YAMLMaterialImporter& operator=(YAMLMaterialImporter&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~YAMLMaterialImporter() noexcept override = default;

    private:
        [[nodiscard]] std::optional<MaterialProperties> ImportProperties(ryml::ConstNodeRef root) const;

    };

}