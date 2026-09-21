//
// Created by Nikolay Kanchevski on 19.11.24.
//

#include "YAMLMaterialSerializer.h"

#include "../../Formats/YAML.h"

namespace SierraEngine
{

    namespace
    {
        // Node + members + diffuse children + specular children + normal children
        constexpr size SETTINGS_NODE_COUNT = 1 + 5 + 3 + 2;

        // Diffuse tint + diffuse texture + specular shininess + specular texture + normal texture
        constexpr size SETTINGS_ARENA_SIZE =  15 + 20 + 6 + 20 + 20;

        std::string_view MaterialAlphaModeToString(const AlphaMode alphaMode)
        {
            switch (alphaMode)
            {
                case AlphaMode::Opaque:         return "Opaque";
                case AlphaMode::Transparent:    return "Transparent";
                default:                        break;
            }

            return "Undefined";
        }

        std::string_view MaterialCullModeToString(const CullMode cullMode)
        {
            switch (cullMode)
            {
                case CullMode::SingleSided:     return "SingleSided";
                case CullMode::DoubleSided:     return "DoubleSided";
                default:                                break;
            }

            return "Undefined";
        }

        void SerializeSettings(ryml::NodeRef rootNode, const MaterialSettings& settings)
        {
            ryml::NodeRef settingsNode = rootNode["settings"];
            settingsNode.set_map();

            ryml::NodeRef diffuseNode = settingsNode["diffuse"];
            diffuseNode.set_map();
            {
                YAML::SerializeVector(diffuseNode["tint"], glm::clamp(settings.diffuse.tint, 0.0f, 1.0f)); // Cost: ~15 chars
                YAML::SerializeNumeric(diffuseNode["texture"], settings.diffuseTexture.GetValue());       // Cost: ~20 chars
            }

            ryml::NodeRef specularNode = settingsNode["specular"];
            specularNode.set_map();
            {
                YAML::SerializeNumeric(specularNode["shininess"], glm::clamp(settings.specular.shininess, 0.0f, 512.0f)); // Cost: ~6 chars
                YAML::SerializeNumeric(specularNode["texture"], settings.specularTexture.GetValue());                    // Cost: ~20 chars
            }

            ryml::NodeRef normalNode = settingsNode["normal"];
            normalNode.set_map();
            {
                YAML::SerializeNumeric(normalNode["texture"], settings.normalTexture.GetValue()); // Cost: ~20 chars
            }

            YAML::SerializeEnum(settingsNode["alphaMode"], settings.alphaMode, MaterialAlphaModeToString);
            YAML::SerializeEnum(settingsNode["cullMode"], settings.cullMode, MaterialCullModeToString);
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<SerializedMaterial> YAMLMaterialSerializer::Serialize(const MaterialSerializeInfo& serializeInfo, MaterialID& outID) const
    {
        const size nodeCount = MANDATORY_NODE_COUNT + GetMetadataNodeCount(serializeInfo.metadata) + SETTINGS_NODE_COUNT;
        const size arenaSize = GetMetadataArenaSize(serializeInfo.metadata) + SETTINGS_ARENA_SIZE;

        ryml::Tree tree(nodeCount, arenaSize);
        outID = MaterialID(Sierra::RNG().Random<MaterialID::ValueType>());

        ryml::NodeRef rootNode = tree.rootref();
        rootNode.set_map();

        SerializeID(rootNode, outID);
        SerializeMetadata(rootNode, serializeInfo.metadata);
        SerializeSettings(rootNode, serializeInfo.settings);

        const std::vector<char> data = ryml::emitrs_yaml<std::vector<char>>(tree);
        if (data.empty())
        {
            APP_WARNING("Could not YAML serialize material!");
            return std::nullopt;
        }

        const size blobMemorySize = sizeof(AssetHeader);
        Sierra::MemoryWriteStream blobStream(blobMemorySize);

        SerializeHeader(blobStream);

        SerializedMaterial material
        {
            .data = { reinterpret_cast<const uint8*>(data.data()), reinterpret_cast<const uint8*>(data.data()) + data.size() },
            .blob = blobStream.Release()
        };

        return material;
    }

}