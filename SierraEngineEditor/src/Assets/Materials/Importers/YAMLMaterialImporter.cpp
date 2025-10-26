//
// Created by Nikolay Kanchevski on 22.11.24.
//

#include "YAMLMaterialImporter.h"

namespace SierraEngine
{

    namespace
    {
        MaterialAlphaMode StringToMaterialAlphaMode(const std::string_view string)
        {
             if (string == "Opaque")        return MaterialAlphaMode::Opaque;
             if (string == "Transparent")   return MaterialAlphaMode::Transparent;

            return MaterialAlphaMode::Opaque;
        }

        MaterialCullMode StringToMaterialCullMode(const std::string_view string)
        {
             if (string == "SingleSided")   return MaterialCullMode::SingleSided;
             if (string == "DoubleSided")   return MaterialCullMode::DoubleSided;

            return MaterialCullMode::SingleSided;
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<ImportedMaterial> YAMLMaterialImporter::Import(const MaterialImportInfo& importInfo) const
    {
        Sierra::MemoryReadStream blobStream(importInfo.serializedMaterial.blob);

        AssetHeader header = blobStream.ReadAs<AssetHeader>();

        if (header.signature != GetSignature())
        {
            APP_WARNING("Cannot YAML import material, as it was not serialized with YAML");
            return std::nullopt;
        }

        if (header.version > GetVersion())
        {
            APP_WARNING("Cannot YAML import material, as it was serialized with a newer version of YAML");
            return std::nullopt;
        }

        const ryml::Tree tree = ryml::parse_in_arena(c4::to_csubstr(std::string_view(reinterpret_cast<const char*>(importInfo.serializedMaterial.data.data()), importInfo.serializedMaterial.data.size())));
        const ryml::ConstNodeRef root = tree.rootref();

        std::optional<AssetMetadata> metadata = ImportMetadata(root);
        if (!metadata.has_value())
        {
            APP_WARNING("Cannot YAML import material, as its metadata is corrupted");
            return std::nullopt;
        }

        std::optional<MaterialProperties> properties = ImportProperties(root);
        if (!properties.has_value())
        {
            APP_WARNING("Cannot YAML import material, as its properties are corrupted");
            return std::nullopt;
        }

        ImportedMaterial material
        {
            .header = std::move(header),
            .metadata = std::move(*metadata),
            .properties = std::move(*properties)
        };

        return material;
    }

    std::optional<MaterialProperties> YAMLMaterialImporter::ImportProperties(const ryml::ConstNodeRef root) const
    {
        const ryml::ConstNodeRef node = root["properties"];
        if (node.key_is_null()) return std::nullopt;

        MaterialProperties properties = { };

        if (const ryml::ConstNodeRef diffuse = node["diffuse"]; diffuse.key_is_null()) return std::nullopt;
        else
        {
            properties.diffuse.tint = ImportVector<Color32>(diffuse["tint"]).value_or(Color32(1.0f, 1.0f, 1.0f, 1.0f));
            properties.diffuse.texture = TextureID(ImportNumeric<AssetID::ValueType>(diffuse["texture"]).value_or(0));
        }

        if (const ryml::ConstNodeRef specular = node["specular"]; specular.key_is_null()) return std::nullopt;
        else
        {
            properties.specular.shininess = glm::clamp(ImportNumeric<float32>(specular["shininess"]).value_or(1.0f), 0.0f, 512.0f);
            properties.specular.texture = TextureID(ImportNumeric<AssetID::ValueType>(specular["texture"]).value_or(0));
        }

        if (const ryml::ConstNodeRef normal = node["normal"]; normal.key_is_null()) return std::nullopt;
        else
        {
            properties.normal.texture = TextureID(ImportNumeric<AssetID::ValueType>(normal["texture"]).value_or(0));
        }

        properties.alphaMode = ImportEnum(node["alphaMode"], StringToMaterialAlphaMode).value_or(MaterialAlphaMode::Opaque);
        properties.cullMode = ImportEnum(node["cullMode"], StringToMaterialCullMode).value_or(MaterialCullMode::SingleSided);

        return properties;
    }

}