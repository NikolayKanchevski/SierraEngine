//
// Created by Nikolay Kanchevski on 30.10.24.
//

#include "YAMLImporter.h"

#include "../Formats/YAML.h"

namespace SierraEngine
{

    /* --- GETTER METHODS --- */

    std::optional<AssetID> YAMLImporter::ImportID(const ryml::ConstNodeRef rootNode) const noexcept
    {
        const std::optional<AssetID::ValueType> ID = YAML::ImportNumeric<AssetID::ValueType>(rootNode.find_child("ID"));
        if (!ID.has_value()) return std::nullopt;

        return ID.value();
    }

    std::optional<AssetMetadata> YAMLImporter::ImportMetadata(const ryml::ConstNodeRef rootNode) const noexcept
    {
        const ryml::ConstNodeRef metadataNode = rootNode.find_child("metadata");
        if (metadataNode.invalid()) return std::nullopt;

        AssetMetadata metadata = { };

        metadata.name = YAML::ImportString(metadataNode.find_child("name")).value_or("Undefined");
        metadata.author = YAML::ImportString(metadataNode.find_child("author")).value_or("Unknown");

        for (const ryml::ConstNodeRef child : metadataNode["tags"].children())
        {
            if (std::optional<std::string> tag = YAML::ImportString(child); tag.has_value())
            {
                metadata.tags.emplace_back(std::move(*tag));
            }
        }

        return metadata;
    }

}
