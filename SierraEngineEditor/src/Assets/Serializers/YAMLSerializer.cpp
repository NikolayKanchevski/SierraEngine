//
// Created by Nikolay Kanchevski on 25.07.24.
//

#include "YAMLSerializer.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    void YAMLSerializer::SerializeID(ryml::NodeRef root, const AssetID ID) const
    {
        SerializeNumeric(root["ID"], ID.GetValue());
    }

    void YAMLSerializer::SerializeMetadata(ryml::NodeRef root, const AssetMetadata& metadata) const
    {
        ryml::NodeRef node = root["metadata"];
        node |= ryml::MAP;

        SerializeString(node["name"], metadata.name);
        SerializeString(node["author"], metadata.author);
        SerializeContainer(node["tags"], std::span(metadata.tags), &YAMLSerializer::SerializeString);
    }

    /* --- GETTER METHODS --- */

    size YAMLSerializer::GetMetadataNodeCount(const AssetMetadata& metadata) const noexcept
    {
        // Node + members + tag count
        return 1 + 3 + metadata.tags.size();
    };

    size YAMLSerializer::GetMetadataArenaSize(const AssetMetadata&) const noexcept
    {
        return 0;
    };

}