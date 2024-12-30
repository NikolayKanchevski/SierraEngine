//
// Created by Nikolay Kanchevski on 28.12.24.
//

#include "AutoMaterialImporter.h"

#include "YAMLMaterialImporter.h"

#define TRY_IMPORTER(TYPE) if (const TYPE importer = TYPE(); signature == importer.GetSignature()) return importer.Import(importInfo);

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    std::optional<ImportedMaterial> AutoMaterialImporter::Import(const MaterialImportInfo& importInfo) const
    {
        const AssetSignature& signature = reinterpret_cast<const AssetSignature&>(*importInfo.serializedMaterial.memory.data());

        TRY_IMPORTER(YAMLMaterialImporter);

        APP_WARNING("Failed to not automatically import serialized material, as its importer type could not be deduced");
        return std::nullopt;
    }

}