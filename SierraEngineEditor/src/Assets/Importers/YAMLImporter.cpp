//
// Created by Nikolay Kanchevski on 30.10.24.
//

#include "YAMLImporter.h"

namespace SierraEngine
{

    /* --- GETTER METHODS --- */

    std::optional<AssetID> YAMLImporter::ImportID(const ryml::ConstNodeRef root) const noexcept
    {
        const std::optional<AssetID::ValueType> ID = ImportNumeric<AssetID::ValueType>(root["ID"]);
        if (!ID.has_value()) return std::nullopt;

        return ID.value();
    }

    std::optional<AssetMetadata> YAMLImporter::ImportMetadata(const ryml::ConstNodeRef root) const noexcept
    {
        const ryml::ConstNodeRef node = root["metadata"];
        if (node.key_is_null()) return std::nullopt;

        AssetMetadata metadata = { };

        metadata.name = std::move(ImportString(node["name"]).value_or("Undefined"));
        metadata.author = std::move(ImportString(node["author"]).value_or("Unknown"));

        for (const ryml::ConstNodeRef child : node["tags"].children())
        {
            if (std::optional<std::string> tag = ImportString(child); tag.has_value())
            {
                metadata.tags.emplace_back(std::move(*tag));
            }
        }

        return metadata;
    }

}