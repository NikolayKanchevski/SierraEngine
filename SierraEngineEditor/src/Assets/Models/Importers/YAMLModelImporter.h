//
// Created by Nikolay Kanchevski on 12.08.25.
//

#pragma once

#include "../../Importers/YAMLImporter.h"

namespace SierraEngine
{

    class YAMLModelImporter final : public ModelImporter, public YAMLImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        YAMLModelImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedModel> Import(const ModelImportInfo& importInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetSignature GetSignature() const noexcept override { return { 'Y', 'A', 'M', 'D' }; }
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 1, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        YAMLModelImporter(const YAMLModelImporter&) = delete;
        YAMLModelImporter& operator=(const YAMLModelImporter&) = delete;

        /* --- MOVE SEMANTICS --- */
        YAMLModelImporter(YAMLModelImporter&&) noexcept = default;
        YAMLModelImporter& operator=(YAMLModelImporter&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~YAMLModelImporter() noexcept override = default;

    };

}