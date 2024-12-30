//
// Created by Nikolay Kanchevski on 30.10.24.
//

#include "YAMLTextureImporter.h"

namespace SierraEngine
{

    namespace
    {
        TextureType StringToTextureType(const std::string_view string) noexcept
        {
            if (string == "Albedo")           return TextureType::Albedo;
            if (string == "Specular")         return TextureType::Specular;
            if (string == "Normal")           return TextureType::Normal;
            if (string == "Opacity")          return TextureType::Opacity;
            if (string == "Roughness")        return TextureType::Roughness;
            if (string == "Metallic")         return TextureType::Metallic;
            if (string == "Displacement")     return TextureType::Displacement;
            if (string == "Emission")         return TextureType::Emission;
            if (string == "Occlusion")        return TextureType::Occlusion;
            if (string == "Shadow")           return TextureType::Shadow;
            if (string == "Environment")      return TextureType::Environment;
            return TextureType::Undefined;
        }

        TextureFilter StringToTextureFilter(const std::string_view string) noexcept
        {
            if (string == "Pixelated")  return TextureFilter::Pixelated;
            if (string == "Smooth")     return TextureFilter::Smooth;
            return TextureFilter::Pixelated;
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<ImportedTexture> YAMLTextureImporter::Import(const TextureImportInfo& importInfo) const
    {
        Sierra::MemoryReadStream memoryStream(importInfo.serializedTexture.memory);
        AssetHeader header = ImportHeader(memoryStream);

        if (header.signature != GetSignature())
        {
            APP_WARNING("Cannot YAML import texture, as it was not serialized with YAML");
            return std::nullopt;
        }

        if (header.version > GetVersion())
        {
            APP_WARNING("Cannot YAML import texture, as it was serialized with a newer version of YAML");
            return std::nullopt;
        }

        const ryml::Tree tree = ryml::parse_in_arena(c4::to_csubstr(std::string_view(reinterpret_cast<const char*>(importInfo.serializedTexture.data.data()), importInfo.serializedTexture.data.size())));
        const ryml::ConstNodeRef root = tree.rootref();

        std::optional<AssetMetadata> metadata = ImportMetadata(root);
        if (!metadata.has_value())
        {
            APP_WARNING("Cannot YAML import texture, as its metadata is corrupted");
            return std::nullopt;
        }

        std::optional<TextureProperties> properties = ImportProperties(root);
        if (!properties.has_value())
        {
            APP_WARNING("Cannot YAML import texture [{0}], as its properties are corrupted", metadata->name);
            return std::nullopt;
        }

        std::vector<uint8> memory = { };
        std::optional<TextureDetails> details = ImportMemory(memoryStream, importInfo, memory);
        if (!details.has_value())
        {
            APP_WARNING("Could not YAML import texture [{0}], as its memory is corrupted", metadata->name);
            return std::nullopt;
        }

        ImportedTexture texture
        {
            .header = std::move(header),
            .metadata = std::move(*metadata),
            .properties = std::move(*properties),
            .details = std::move(*details),
            .memory = std::move(memory)
        };

        return texture;
    }

    std::optional<TextureProperties> YAMLTextureImporter::ImportProperties(ryml::ConstNodeRef root) const
    {
        const ryml::ConstNodeRef node = root["properties"];
        if (node.key_is_null()) return std::nullopt;

        TextureProperties properties = { };

        properties.type = ImportEnum(node["type"], StringToTextureType).value_or(TextureType::Undefined);
        properties.filter = ImportEnum(node["filter"], StringToTextureFilter).value_or(TextureFilter::Pixelated);

        return properties;
    }

}
