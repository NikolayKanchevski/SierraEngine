//
// Created by Nikolay Kanchevski on 30.10.24.
//

#pragma once

#include "../../Importers/YAMLImporter.h"

namespace SierraEngine
{

    class YAMLTextureImporter final : public TextureImporter, public YAMLImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        YAMLTextureImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedTexture> Import(const TextureImportInfo& importInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetSignature GetSignature() const noexcept override { return { 'Y', 'A', 'T', 'X' }; }
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 1, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        YAMLTextureImporter(const YAMLTextureImporter&) = delete;
        YAMLTextureImporter& operator=(const YAMLTextureImporter&) = delete;

        /* --- MOVE SEMANTICS --- */
        YAMLTextureImporter(YAMLTextureImporter&&) noexcept = default;
        YAMLTextureImporter& operator=(YAMLTextureImporter&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~YAMLTextureImporter() noexcept override = default;

    private:
        [[nodiscard]] std::optional<TextureProperties> ImportProperties(ryml::ConstNodeRef root) const;

    };

}