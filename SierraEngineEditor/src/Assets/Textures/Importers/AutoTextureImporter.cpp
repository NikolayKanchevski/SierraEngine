//
// Created by Nikolay Kanchevski on 28.12.24.
//

#include "AutoTextureImporter.h"

#include "YAMLTextureImporter.h"

#define TRY_IMPORTER(TYPE) if (const TYPE importer = TYPE(); signature == importer.GetSignature()) return importer.Import(importInfo);

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    std::optional<ImportedTexture> AutoTextureImporter::Import(const TextureImportInfo& importInfo) noexcept
    {
        const AssetSignature& signature = reinterpret_cast<const AssetSignature&>(*importInfo.serializedTexture.blob.data());

        TRY_IMPORTER(YAMLTextureImporter);

        APP_WARNING("Failed to not automatically import serialized texture, as its importer type could not be deduced");
        return std::nullopt;
    }

}