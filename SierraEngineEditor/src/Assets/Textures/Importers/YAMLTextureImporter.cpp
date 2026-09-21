//
// Created by Nikolay Kanchevski on 30.10.24.
//

#include "YAMLTextureImporter.h"

#include "../../Formats/YAML.h"

namespace SierraEngine
{

    namespace
    {
        [[nodiscard]] TextureFilter StringToTextureFilter(const std::string_view string) noexcept
        {
            if (string == "Pixelated")  return TextureFilter::Pixelated;
            if (string == "Smooth")     return TextureFilter::Smooth;

            return TextureFilter::Pixelated;
        }

        [[nodiscard]] TextureFormat StringToTextureFormat(const std::string_view string) noexcept
        {
            if (string == "Undefined")             return TextureFormat::Undefined;
            if (string == "R8_Int")                return TextureFormat::R8_Int;
            if (string == "R8_UInt")               return TextureFormat::R8_UInt;
            if (string == "R8_Norm")               return TextureFormat::R8_Norm;
            if (string == "R8_UNorm")              return TextureFormat::R8_UNorm;
            if (string == "R8_SRGB")               return TextureFormat::R8_SRGB;
            if (string == "R8G8_Int")              return TextureFormat::R8G8_Int;
            if (string == "R8G8_UInt")             return TextureFormat::R8G8_UInt;
            if (string == "R8G8_Norm")             return TextureFormat::R8G8_Norm;
            if (string == "R8G8_UNorm")            return TextureFormat::R8G8_UNorm;
            if (string == "R8G8_SRGB")             return TextureFormat::R8G8_SRGB;
            if (string == "R8G8B8_Int")            return TextureFormat::R8G8B8_Int;
            if (string == "R8G8B8_UInt")           return TextureFormat::R8G8B8_UInt;
            if (string == "R8G8B8_Norm")           return TextureFormat::R8G8B8_Norm;
            if (string == "R8G8B8_UNorm")          return TextureFormat::R8G8B8_UNorm;
            if (string == "R8G8B8_SRGB")           return TextureFormat::R8G8B8_SRGB;
            if (string == "R8G8B8A8_Int")          return TextureFormat::R8G8B8A8_Int;
            if (string == "R8G8B8A8_UInt")         return TextureFormat::R8G8B8A8_UInt;
            if (string == "R8G8B8A8_Norm")         return TextureFormat::R8G8B8A8_Norm;
            if (string == "R8G8B8A8_UNorm")        return TextureFormat::R8G8B8A8_UNorm;
            if (string == "R8G8B8A8_SRGB")         return TextureFormat::R8G8B8A8_SRGB;
            if (string == "B8G8R8A8_UNorm")        return TextureFormat::B8G8R8A8_UNorm;
            if (string == "B8G8R8A8_SRGB")         return TextureFormat::B8G8R8A8_SRGB;
            if (string == "R16_Int")               return TextureFormat::R16_Int;
            if (string == "R16_UInt")              return TextureFormat::R16_UInt;
            if (string == "R16_Norm")              return TextureFormat::R16_Norm;
            if (string == "R16_UNorm")             return TextureFormat::R16_UNorm;
            if (string == "R16_Float")             return TextureFormat::R16_Float;
            if (string == "R16G16_Int")            return TextureFormat::R16G16_Int;
            if (string == "R16G16_UInt")           return TextureFormat::R16G16_UInt;
            if (string == "R16G16_Norm")           return TextureFormat::R16G16_Norm;
            if (string == "R16G16_UNorm")          return TextureFormat::R16G16_UNorm;
            if (string == "R16G16_Float")          return TextureFormat::R16G16_Float;
            if (string == "R16G16B16_Int")         return TextureFormat::R16G16B16_Int;
            if (string == "R16G16B16_UInt")        return TextureFormat::R16G16B16_UInt;
            if (string == "R16G16B16_Norm")        return TextureFormat::R16G16B16_Norm;
            if (string == "R16G16B16_UNorm")       return TextureFormat::R16G16B16_UNorm;
            if (string == "R16G16B16_Float")       return TextureFormat::R16G16B16_Float;
            if (string == "R16G16B16A16_Int")      return TextureFormat::R16G16B16A16_Int;
            if (string == "R16G16B16A16_UInt")     return TextureFormat::R16G16B16A16_UInt;
            if (string == "R16G16B16A16_Norm")     return TextureFormat::R16G16B16A16_Norm;
            if (string == "R16G16B16A16_UNorm")    return TextureFormat::R16G16B16A16_UNorm;
            if (string == "R16G16B16A16_Float")    return TextureFormat::R16G16B16A16_Float;
            if (string == "R32_Int")               return TextureFormat::R32_Int;
            if (string == "R32_UInt")              return TextureFormat::R32_UInt;
            if (string == "R32_Float")             return TextureFormat::R32_Float;
            if (string == "R32G32_Int")            return TextureFormat::R32G32_Int;
            if (string == "R32G32_UInt")           return TextureFormat::R32G32_UInt;
            if (string == "R32G32_Float")          return TextureFormat::R32G32_Float;
            if (string == "R32G32B32_Int")         return TextureFormat::R32G32B32_Int;
            if (string == "R32G32B32_UInt")        return TextureFormat::R32G32B32_UInt;
            if (string == "R32G32B32_Float")       return TextureFormat::R32G32B32_Float;
            if (string == "R32G32B32A32_Int")      return TextureFormat::R32G32B32A32_Int;
            if (string == "R32G32B32A32_UInt")     return TextureFormat::R32G32B32A32_UInt;
            if (string == "R32G32B32A32_Float")    return TextureFormat::R32G32B32A32_Float;
            if (string == "R64_Int")               return TextureFormat::R64_Int;
            if (string == "R64_UInt")              return TextureFormat::R64_UInt;
            if (string == "R64_Float")             return TextureFormat::R64_Float;
            if (string == "R64G64_Int")            return TextureFormat::R64G64_Int;
            if (string == "R64G64_UInt")           return TextureFormat::R64G64_UInt;
            if (string == "R64G64_Float")          return TextureFormat::R64G64_Float;
            if (string == "R64G64B64_Int")         return TextureFormat::R64G64B64_Int;
            if (string == "R64G64B64_UInt")        return TextureFormat::R64G64B64_UInt;
            if (string == "R64G64B64_Float")       return TextureFormat::R64G64B64_Float;
            if (string == "R64G64B64A64_Int")      return TextureFormat::R64G64B64A64_Int;
            if (string == "R64G64B64A64_UInt")     return TextureFormat::R64G64B64A64_UInt;
            if (string == "R64G64B64A64_Float")    return TextureFormat::R64G64B64A64_Float;
            if (string == "D16_UNorm")             return TextureFormat::D16_UNorm;
            if (string == "D32_Float")             return TextureFormat::D32_Float;
            if (string == "BC1_RGB_UNorm")         return TextureFormat::BC1_RGB_UNorm;
            if (string == "BC1_RGB_SRGB")          return TextureFormat::BC1_RGB_SRGB;
            if (string == "BC1_RGBA_UNorm")        return TextureFormat::BC1_RGBA_UNorm;
            if (string == "BC1_RGBA_SRGB")         return TextureFormat::BC1_RGBA_SRGB;
            if (string == "BC3_RGBA_UNorm")        return TextureFormat::BC3_RGBA_UNorm;
            if (string == "BC3_RGBA_SRGB")         return TextureFormat::BC3_RGBA_SRGB;
            if (string == "BC4_R_Norm")            return TextureFormat::BC4_R_Norm;
            if (string == "BC4_R_UNorm")           return TextureFormat::BC4_R_UNorm;
            if (string == "BC5_RG_Norm")           return TextureFormat::BC5_RG_Norm;
            if (string == "BC5_RG_UNorm")          return TextureFormat::BC5_RG_UNorm;
            if (string == "BC6_HDR_RGB_Float")     return TextureFormat::BC6_HDR_RGB_Float;
            if (string == "BC6_HDR_RGB_UFloat")    return TextureFormat::BC6_HDR_RGB_UFloat;
            if (string == "BC7_RGB_UNorm")         return TextureFormat::BC7_RGB_UNorm;
            if (string == "BC7_RGB_SRGB")          return TextureFormat::BC7_RGB_SRGB;
            if (string == "BC7_RGBA_UNorm")        return TextureFormat::BC7_RGBA_UNorm;
            if (string == "BC7_RGBA_SRGB")         return TextureFormat::BC7_RGBA_SRGB;
            if (string == "ASTC_4x4_UNorm")        return TextureFormat::ASTC_4x4_UNorm;
            if (string == "ASTC_4x4_SRGB")         return TextureFormat::ASTC_4x4_SRGB;
            if (string == "ASTC_8x8_UNorm")        return TextureFormat::ASTC_8x8_UNorm;
            if (string == "ASTC_8x8_SRGB")         return TextureFormat::ASTC_8x8_SRGB;

            return TextureFormat::Undefined;
        }

        [[nodiscard]] TextureCompression StringToTextureCompression(const std::string_view string) noexcept
        {
            if (string == "None")           return TextureCompression::None;
            if (string == "BasisUniversal") return TextureCompression::BasisUniversal;

            return TextureCompression::None;
        }

        [[nodiscard]] std::optional<TextureSettings> ImportSettings(const ryml::ConstNodeRef rootNode)
        {
            const ryml::ConstNodeRef settingsNode = rootNode.find_child("settings");
            if (settingsNode.invalid()) return std::nullopt;

            TextureSettings settings = { };
            settings.filter = YAML::ImportEnum(settingsNode.find_child("filter"), StringToTextureFilter).value_or(TextureFilter::Pixelated);

            return settings;
        }

        [[nodiscard]] std::optional<TextureProperties> ImportProperties(const ryml::ConstNodeRef rootNode)
        {
            const ryml::ConstNodeRef propertiesNode = rootNode.find_child("properties");
            if (propertiesNode.invalid()) return std::nullopt;

            const std::optional<uint32> width = YAML::ImportNumeric<uint32>(propertiesNode.find_child("width"));
            if (!width.has_value()) return std::nullopt;

            const std::optional<uint32> height = YAML::ImportNumeric<uint32>(propertiesNode.find_child("height"));
            if (!height.has_value()) return std::nullopt;

            const std::optional<uint32> levelCount = YAML::ImportNumeric<uint32>(propertiesNode.find_child("levelCount"));
            if (!levelCount.has_value()) return std::nullopt;

            const std::optional<uint32> layerCount = YAML::ImportNumeric<uint32>(propertiesNode.find_child("layerCount"));
            if (!layerCount.has_value()) return std::nullopt;

            const std::optional<TextureFormat> format = YAML::ImportEnum(propertiesNode.find_child("format"), StringToTextureFormat);
            if (!format.has_value()) return std::nullopt;

            const std::optional<TextureCompression> compression = YAML::ImportEnum(propertiesNode.find_child("compression"), StringToTextureCompression);
            if (!compression.has_value()) return std::nullopt;

            TextureProperties properties = {
                .width = *width,
                .height = *height,
                .levelCount = *levelCount,
                .layerCount = *layerCount,
                .format = *format,
                .compression = *compression
            };

            return properties;
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<ImportedTexture> YAMLTextureImporter::Import(const TextureImportInfo& importInfo) const
    {
        Sierra::MemoryReadStream blobStream(importInfo.serializedTexture.blob);
        AssetHeader header = blobStream.ReadAs<AssetHeader>();

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

        const ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(std::string_view(reinterpret_cast<const char*>(importInfo.serializedTexture.data.data()), importInfo.serializedTexture.data.size())));
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
            APP_WARNING("Cannot YAML import texture, as its metadata is corrupted");
            return std::nullopt;
        }

        std::optional<TextureSettings> settings = ImportSettings(rootNode);
        if (!settings.has_value())
        {
            APP_WARNING("Cannot YAML import texture, as its settings are corrupted");
            return std::nullopt;
        }

        std::optional<TextureProperties> properties = ImportProperties(rootNode);
        if (!properties.has_value())
        {
            APP_WARNING("Cannot YAML import texture, as its properties are corrupted");
            return std::nullopt;
        }

        std::optional<ImportedTextureBuffer> blob = ImportBlob(blobStream, importInfo.format, *properties);
        if (!blob.has_value())
        {
            APP_WARNING("Could not YAML import texture [{0}], its blob is corrupted", metadata->name);
            return std::nullopt;
        }

        ImportedTexture texture
        {
            .ID = TextureID(ID->GetValue()),
            .header = std::move(header),
            .metadata = std::move(*metadata),
            .settings = std::move(*settings),
            .properties = std::move(*properties),
            .buffer = std::move(*blob)
        };

        return texture;
    }

}
