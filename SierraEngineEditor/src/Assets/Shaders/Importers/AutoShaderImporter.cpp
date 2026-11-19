//
// Created by Nikolay Kanchevski on 18.11.26.
//

#include "AutoShaderImporter.h"

#include "YAMLShaderImporter.h"

#define TRY_IMPORTER(TYPE) if (const TYPE importer = TYPE(); signature == importer.GetSignature()) return importer.Import(importInfo);

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    std::optional<ImportedShader> AutoShaderImporter::Import(const ShaderImportInfo& importInfo) noexcept
    {
        if (importInfo.serializedShader.blob.size() < sizeof(AssetSignature))
        {
            APP_WARNING("Could not automatically import serialized shader, as its blob is too small to contain an asset signature");
            return std::nullopt;
        }

        const AssetSignature& signature = reinterpret_cast<const AssetSignature&>(*importInfo.serializedShader.blob.data());

        TRY_IMPORTER(YAMLShaderImporter);

        APP_WARNING("Could not automatically import serialized shader, as its importer type could not be deduced");
        return std::nullopt;
    }

}
