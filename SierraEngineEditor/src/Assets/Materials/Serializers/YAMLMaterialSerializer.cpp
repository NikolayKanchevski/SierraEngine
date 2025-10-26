//
// Created by Nikolay Kanchevski on 19.11.24.
//

#include "YAMLMaterialSerializer.h"

namespace SierraEngine
{

    namespace
    {
        std::string_view MaterialAlphaModeToString(const MaterialAlphaMode alphaMode)
        {
            switch (alphaMode)
            {
                case MaterialAlphaMode::Opaque:         return "Opaque";
                case MaterialAlphaMode::Transparent:    return "Transparent";
                default:                                break;
            }

            return "Undefined";
        }

        std::string_view MaterialCullModeToString(const MaterialCullMode cullMode)
        {
            switch (cullMode)
            {
                case MaterialCullMode::SingleSided:     return "SingleSided";
                case MaterialCullMode::DoubleSided:     return "DoubleSided";
                default:                                break;
            }

            return "Undefined";
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<SerializedMaterial> YAMLMaterialSerializer::Serialize(const MaterialSerializeInfo& serializeInfo) const
    {
        const size nodeCapacity = GetMetadataNodeCount(serializeInfo.metadata) + GetPropertiesNodeCount();
        const size arenaCapacity = GetMetadataArenaSize(serializeInfo.metadata) + GetPropertiesArenaSize();
        ryml::Tree tree(nodeCapacity, arenaCapacity);

        ryml::NodeRef root = tree.rootref();
        root |= ryml::MAP;

        SerializeID(root, Sierra::RNG().Random<MaterialID::ValueType>());
        SerializeMetadata(root, serializeInfo.metadata);
        SerializeProperties(root, serializeInfo.properties);

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

    /* --- POLLING METHODS --- */

    void YAMLMaterialSerializer::SerializeProperties(ryml::NodeRef root, const MaterialProperties& properties) const
    {
        ryml::NodeRef node = root["properties"];
        node |= ryml::MAP;

        ryml::NodeRef diffuse = node["diffuse"];
        diffuse |= ryml::MAP;
        {
            SerializeVector(diffuse["tint"], glm::clamp(properties.diffuse.tint, 0.0f, 1.0f)); // Cost: ~15 chars
            SerializeNumeric(diffuse["texture"], properties.diffuse.texture.GetValue());       // Cost: ~19 chars
        }

        ryml::NodeRef specular = node["specular"];
        specular |= ryml::MAP;
        {
            SerializeNumeric(specular["shininess"], glm::clamp(properties.specular.shininess, 0.0f, 512.0f));        // Cost: ~6 chars
            SerializeNumeric(specular["texture"], properties.specular.texture.GetValue()); // Cost: ~19 chars
        }

        ryml::NodeRef normal = node["normal"];
        normal |= ryml::MAP;
        {
            SerializeNumeric(normal["texture"], properties.normal.texture.GetValue()); // Cost: ~19 chars
        }

        SerializeEnum(node["alphaMode"], properties.alphaMode, MaterialAlphaModeToString);
        SerializeEnum(node["cullMode"], properties.cullMode, MaterialCullModeToString);
    }

    /* --- GETTER METHODS --- */

    [[nodiscard]] size YAMLMaterialSerializer::GetPropertiesNodeCount() const noexcept
    {
        // Node + members + diffuse children + specular children + normal children
        return 1 + 5 + 3 + 2;
    }

    [[nodiscard]] size YAMLMaterialSerializer::GetPropertiesArenaSize() const noexcept
    {
        // Diffuse tint + diffuse texture + specular shininess + specular texture + normal texture
        return 15 + 19 + 6 + 19 + 19;
    }

}