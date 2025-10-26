//
// Created by Nikolay Kanchevski on 30.10.24.
//

#include "YAMLTextureImporter.h"

namespace SierraEngine
{

    namespace
    {
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
        Sierra::MemoryReadStream blobStream(importInfo.serializedTexture.blob);

        AssetHeader header = blobStream.ReadAs<AssetHeader>();
        TextureHeader textureHeader = blobStream.ReadAs<TextureHeader>();

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
            APP_WARNING("Cannot YAML import texture, as its properties are corrupted");
            return std::nullopt;
        }

        std::vector<uint8> blob = ImportBlob(textureHeader, importInfo.format, blobStream);
        if (blob.empty())
        {
            APP_WARNING("Could not YAML import texture [{0}], its blob is corrupted", metadata->name);
            return std::nullopt;
        }

        ImportedTexture texture
        {
            .header = std::move(header),
            .metadata = std::move(*metadata),
            .properties = std::move(*properties),
            .textureHeader = std::move(textureHeader),
            .memory = std::move(blob)
        };

        return texture;
    }

    std::optional<TextureProperties> YAMLTextureImporter::ImportProperties(ryml::ConstNodeRef root) const
    {
        const ryml::ConstNodeRef node = root["properties"];
        if (node.key_is_null()) return std::nullopt;

        TextureProperties properties = { };
        properties.filter = ImportEnum(node["filter"], StringToTextureFilter).value_or(TextureFilter::Pixelated);

        return properties;
    }

}
