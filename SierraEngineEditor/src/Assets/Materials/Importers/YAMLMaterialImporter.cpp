//
// Created by Nikolay Kanchevski on 22.11.24.
//

#include "YAMLMaterialImporter.h"

#include "../../Formats/YAML.h"

namespace SierraEngine
{

    namespace
    {
        AlphaMode StringToMaterialAlphaMode(const std::string_view string)
        {
             if (string == "Opaque")        return AlphaMode::Opaque;
             if (string == "Transparent")   return AlphaMode::Transparent;

            return AlphaMode::Opaque;
        }

        CullMode StringToMaterialCullMode(const std::string_view string)
        {
             if (string == "SingleSided")   return CullMode::SingleSided;
             if (string == "DoubleSided")   return CullMode::DoubleSided;

            return CullMode::SingleSided;
        }

        std::optional<MaterialSettings> ImportSettings(const ryml::ConstNodeRef rootNode)
        {
            const ryml::ConstNodeRef settingsNode = rootNode.find_child("settings");
            if (settingsNode.invalid()) return std::nullopt;

            MaterialSettings settings = { };

            if (const ryml::ConstNodeRef diffuse = settingsNode.find_child("diffuse"); !diffuse.invalid())
            {
                settings.diffuse.tint = YAML::ImportVector<Color32>(diffuse.find_child("tint")).value_or(Color32(1.0f, 1.0f, 1.0f, 1.0f));
                settings.diffuseTexture = TextureID(YAML::ImportNumeric<AssetID::ValueType>(diffuse["texture"]).value_or(0));
            }

            if (const ryml::ConstNodeRef specular = settingsNode.find_child("specular"); !specular.invalid())
            {
                settings.specular.shininess = glm::clamp(YAML::ImportNumeric<float32>(specular.find_child("shininess")).value_or(1.0f), 0.0f, 512.0f);
                settings.specularTexture = TextureID(YAML::ImportNumeric<AssetID::ValueType>(specular["texture"]).value_or(0));
            }

            if (const ryml::ConstNodeRef normal = settingsNode.find_child("normal"); !normal.invalid())
            {
                settings.normalTexture = TextureID(YAML::ImportNumeric<AssetID::ValueType>(normal["texture"]).value_or(0));
            }

            settings.alphaMode = YAML::ImportEnum(settingsNode.find_child("alphaMode"), StringToMaterialAlphaMode).value_or(AlphaMode::Opaque);
            settings.cullMode = YAML::ImportEnum(settingsNode.find_child("cullMode"), StringToMaterialCullMode).value_or(CullMode::SingleSided);

            return settings;
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

        const ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(std::string_view(reinterpret_cast<const char*>(importInfo.serializedMaterial.data.data()), importInfo.serializedMaterial.data.size())));
        const ryml::ConstNodeRef rootNode = tree.rootref();

        const std::optional<AssetID> ID = ImportID(rootNode);
        if (!ID.has_value())
        {
            APP_WARNING("Cannot YAML import texture, as its ID is corrupted");
            return std::nullopt;
        }

        std::optional<AssetMetadata> metadata = ImportMetadata(rootNode);
        if (!metadata.has_value())
        {
            APP_WARNING("Cannot YAML import material, as its metadata is corrupted");
            return std::nullopt;
        }

        std::optional<MaterialSettings> settings = ImportSettings(rootNode);
        if (!settings.has_value())
        {
            APP_WARNING("Cannot YAML import material, as its settings are corrupted");
            return std::nullopt;
        }

        ImportedMaterial material
        {
            .ID = MaterialID(ID->GetValue()),
            .header = std::move(header),
            .metadata = std::move(*metadata),
            .settings = std::move(*settings)
        };

        return material;
    }

}