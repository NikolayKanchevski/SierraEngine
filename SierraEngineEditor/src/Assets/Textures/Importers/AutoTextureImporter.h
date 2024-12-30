//
// Created by Nikolay Kanchevski on 28.12.24.
//

#pragma once

namespace SierraEngine
{

    class AutoTextureImporter final : public TextureImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        AutoTextureImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedTexture> Import(const TextureImportInfo& importInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetSignature GetSignature() const noexcept override { return { '\0', '\0', '\0', '\0' }; }
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 0, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        AutoTextureImporter(const AutoTextureImporter&) = delete;
        AutoTextureImporter& operator=(const AutoTextureImporter&) = delete;

        /* --- MOVE SEMANTICS --- */
        AutoTextureImporter(AutoTextureImporter&&) noexcept = default;
        AutoTextureImporter& operator=(AutoTextureImporter&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~AutoTextureImporter() noexcept override = default;
    };

}