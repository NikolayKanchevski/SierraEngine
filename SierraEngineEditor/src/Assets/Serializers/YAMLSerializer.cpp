//
// Created by Nikolay Kanchevski on 25.07.24.
//

#include "YAMLSerializer.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    void YAMLSerializer::SerializeMetadata(ryml::NodeRef root, const AssetMetadata &metadata)
    {
        ryml::NodeRef node = root["metadata"];
        node |= ryml::MAP;

        ryml::NodeRef name = node["name"]; name |= ryml::VAL_PLAIN;
        name = c4::to_csubstr(metadata.name);

        ryml::NodeRef author = node["author"]; author |= ryml::VAL_PLAIN;
        author = c4::to_csubstr(metadata.author);

        ryml::NodeRef version = node["version"]; version |= ryml::VAL_PLAIN;
        version << fmt::format("{0}.{1}.{2}", metadata.version.GetMajor(), metadata.version.GetMinor(), metadata.version.GetPatch());

        ryml::NodeRef tags = node["tags"]; tags |= ryml::SEQ;
        for (size i = 0; i < metadata.tags.size(); i++)
        {
            tags[i] |= ryml::VAL_PLAIN;
            tags[i] = c4::to_csubstr(metadata.tags[i]);
        }
    }

    /* --- GETTER METHODS --- */

    size YAMLSerializer::GetMetadataNodeSize(const AssetMetadata &metadata)
    {
        // Metadata node + metadata fields + container size of tags
        return 1 + 4 + metadata.tags.size();
    }

    size YAMLSerializer::GetMetadataArenaSize(const AssetMetadata &metadata)
    {
        // Only length of version string, rest of strings are pre-allocated
        return 8;
    }

}