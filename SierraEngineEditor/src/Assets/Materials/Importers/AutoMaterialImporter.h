//
// Created by Nikolay Kanchevski on 28.12.24.
//

#pragma once

namespace SierraEngine
{

    class AutoMaterialImporter final : public MaterialImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        AutoMaterialImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedMaterial> Import(const MaterialImportInfo& importInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetSignature GetSignature() const noexcept override { return { '\0', '\0', '\0', '\0' }; }
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 0, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        AutoMaterialImporter(const AutoMaterialImporter&) = delete;
        AutoMaterialImporter& operator=(const AutoMaterialImporter&) = delete;

        /* --- MOVE SEMANTICS --- */
        AutoMaterialImporter(AutoMaterialImporter&&) noexcept = default;
        AutoMaterialImporter& operator=(AutoMaterialImporter&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~AutoMaterialImporter() noexcept override = default;
    };

}