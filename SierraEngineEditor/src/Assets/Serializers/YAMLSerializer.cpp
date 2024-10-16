//
// Created by Nikolay Kanchevski on 25.07.24.
//

#include "YAMLSerializer.h"

namespace SierraEngine
{

    /*
     *     [=============== Serialized Metadata ===============]
     *
     *      metadata:
     *        name: Asset
     *        author: Unknown
     *        version: 1.0.0
     *        hash: 12345678
     *        tags:
     *          - Tag
     *
     */

    /* --- POLLING METHODS --- */

    void YAMLSerializer::SerializeMetadata(ryml::NodeRef root, const AssetMetadata& metadata) const noexcept
    {
        ryml::NodeRef node = root["metadata"];
        node |= ryml::MAP;

        ryml::NodeRef name = node["name"]; name |= ryml::VAL_PLAIN;
        name = c4::to_csubstr(metadata.name);

        ryml::NodeRef author = node["author"]; author |= ryml::VAL_PLAIN;
        author = c4::to_csubstr(metadata.author);

        ryml::NodeRef version = node["version"]; version |= ryml::VAL_PLAIN;
        version << SR_FORMAT("{0}.{1}.{2}", metadata.version.GetMajor(), metadata.version.GetMinor(), metadata.version.GetPatch());

        ryml::NodeRef hash = node["hash"]; hash |= ryml::VAL_PLAIN;
        hash << std::to_string(metadata.hash.GetValue());

        ryml::NodeRef tags = node["tags"]; tags |= ryml::SEQ;
        for (size i = 0; i < metadata.tags.size(); i++)
        {
            tags[i] |= ryml::VAL_PLAIN;
            tags[i] = c4::to_csubstr(metadata.tags[i]);
        }
    }

    /* --- GETTER METHODS --- */

    size YAMLSerializer::GetMetadataNodeSize(const AssetMetadata& metadata) const noexcept
    {
        // Metadata node + metadata fields + container size of tags
        return 1 + 5 + metadata.tags.size();
    }

    size YAMLSerializer::GetMetadataArenaSize(const AssetMetadata&) const noexcept
    {
        // Length of version string + hash
        return 8 + 19;
    }

}