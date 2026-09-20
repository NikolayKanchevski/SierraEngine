//
// Created by Nikolay Kanchevski on 25.07.24.
//

#include "YAMLSerializer.h"

#include "../Formats/YAML.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    void YAMLSerializer::SerializeID(ryml::NodeRef rootNode, const AssetID ID) const
    {
        YAML::SerializeNumeric(rootNode["ID"], ID.GetValue());
    }

    void YAMLSerializer::SerializeMetadata(ryml::NodeRef rootNode, const AssetMetadata& metadata) const
    {
        ryml::NodeRef metadataNode = rootNode["metadata"];
        metadataNode |= ryml::MAP;

        YAML::SerializeString(metadataNode["name"], metadata.name);
        YAML::SerializeString(metadataNode["author"], metadata.author);
        YAML::SerializeContainer(metadataNode["tags"], std::span(metadata.tags), YAML::SerializeString);
    }

    /* --- GETTER METHODS --- */

    size YAMLSerializer::GetMetadataNodeCount(const AssetMetadata& metadata) const noexcept
    {
        // Node + members + tag count
        return 1 + 3 + metadata.tags.size();
    }

    size YAMLSerializer::GetMetadataArenaSize(const AssetMetadata&) const noexcept
    {
        return 0;
    }

}