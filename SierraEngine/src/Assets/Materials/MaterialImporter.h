//
// Created by Nikolay Kanchevski on 22.11.24.
//

#pragma once

#include "../AssetImporter.h"

#include "MaterialSerializer.h"

namespace SierraEngine
{

    struct MaterialImportInfo
    {
        const SerializedMaterial& serializedMaterial;
    };

    struct ImportedMaterial
    {
        AssetHeader header = { };
        AssetMetadata metadata = { };

        MaterialProperties properties = { };
    };

    class SIERRA_ENGINE_API MaterialImporter : public virtual AssetImporter
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<ImportedMaterial> Import(const MaterialImportInfo& importInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        MaterialImporter(const MaterialImporter&) = delete;
        MaterialImporter& operator=(const MaterialImporter&) = delete;

        /* --- DESTRUCTOR --- */
        ~MaterialImporter() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        MaterialImporter() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        MaterialImporter(MaterialImporter&&) noexcept = default;
        MaterialImporter& operator=(MaterialImporter&&) noexcept = default;

    };

}