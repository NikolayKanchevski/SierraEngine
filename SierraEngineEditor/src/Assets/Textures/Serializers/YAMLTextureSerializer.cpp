//
// Created by Nikolay Kanchevski on 16.07.24.
//

#include "YAMLTextureSerializer.h"

#include "../../Formats/YAML.h"

namespace SierraEngine
{

    namespace
    {
        // Node + members
        constexpr size SETTINGS_NODE_COUNT = 1 + 1;
        constexpr size SETTINGS_ARENA_SIZE = 0;

        // Node + members
        constexpr size PROPERTIES_NODE_COUNT = 1 + 6;

        // Width + height + levels + layers
        constexpr size PROPERTIES_ARENA_SIZE = 6 + 6 + 2 + 1;

        std::string_view TextureFilterToString(const TextureFilter filter) noexcept
        {
            switch (filter)
            {
                case TextureFilter::Pixelated:    return "Pixelated";
                case TextureFilter::Smooth:       return "Smooth";
            }

            return "Unknown";
        }

        std::string_view TextureFormatToString(const TextureFormat format) noexcept
        {
            switch (format)
            {
                case TextureFormat::Undefined:             return "Undefined";
                case TextureFormat::R8_Int:                return "R8_Int";
                case TextureFormat::R8_UInt:               return "R8_UInt";
                case TextureFormat::R8_Norm:               return "R8_Norm";
                case TextureFormat::R8_UNorm:              return "R8_UNorm";
                case TextureFormat::R8_SRGB:               return "R8_SRGB";
                case TextureFormat::R8G8_Int:              return "R8G8_Int";
                case TextureFormat::R8G8_UInt:             return "R8G8_UInt";
                case TextureFormat::R8G8_Norm:             return "R8G8_Norm";
                case TextureFormat::R8G8_UNorm:            return "R8G8_UNorm";
                case TextureFormat::R8G8_SRGB:             return "R8G8_SRGB";
                case TextureFormat::R8G8B8_Int:            return "R8G8B8_Int";
                case TextureFormat::R8G8B8_UInt:           return "R8G8B8_UInt";
                case TextureFormat::R8G8B8_Norm:           return "R8G8B8_Norm";
                case TextureFormat::R8G8B8_UNorm:          return "R8G8B8_UNorm";
                case TextureFormat::R8G8B8_SRGB:           return "R8G8B8_SRGB";
                case TextureFormat::R8G8B8A8_Int:          return "R8G8B8A8_Int";
                case TextureFormat::R8G8B8A8_UInt:         return "R8G8B8A8_UInt";
                case TextureFormat::R8G8B8A8_Norm:         return "R8G8B8A8_Norm";
                case TextureFormat::R8G8B8A8_UNorm:        return "R8G8B8A8_UNorm";
                case TextureFormat::R8G8B8A8_SRGB:         return "R8G8B8A8_SRGB";
                case TextureFormat::B8G8R8A8_UNorm:        return "B8G8R8A8_UNorm";
                case TextureFormat::B8G8R8A8_SRGB:         return "B8G8R8A8_SRGB";
                case TextureFormat::R16_Int:               return "R16_Int";
                case TextureFormat::R16_UInt:              return "R16_UInt";
                case TextureFormat::R16_Norm:              return "R16_Norm";
                case TextureFormat::R16_UNorm:             return "R16_UNorm";
                case TextureFormat::R16_Float:             return "R16_Float";
                case TextureFormat::R16G16_Int:            return "R16G16_Int";
                case TextureFormat::R16G16_UInt:           return "R16G16_UInt";
                case TextureFormat::R16G16_Norm:           return "R16G16_Norm";
                case TextureFormat::R16G16_UNorm:          return "R16G16_UNorm";
                case TextureFormat::R16G16_Float:          return "R16G16_Float";
                case TextureFormat::R16G16B16_Int:         return "R16G16B16_Int";
                case TextureFormat::R16G16B16_UInt:        return "R16G16B16_UInt";
                case TextureFormat::R16G16B16_Norm:        return "R16G16B16_Norm";
                case TextureFormat::R16G16B16_UNorm:       return "R16G16B16_UNorm";
                case TextureFormat::R16G16B16_Float:       return "R16G16B16_Float";
                case TextureFormat::R16G16B16A16_Int:      return "R16G16B16A16_Int";
                case TextureFormat::R16G16B16A16_UInt:     return "R16G16B16A16_UInt";
                case TextureFormat::R16G16B16A16_Norm:     return "R16G16B16A16_Norm";
                case TextureFormat::R16G16B16A16_UNorm:    return "R16G16B16A16_UNorm";
                case TextureFormat::R16G16B16A16_Float:    return "R16G16B16A16_Float";
                case TextureFormat::R32_Int:               return "R32_Int";
                case TextureFormat::R32_UInt:              return "R32_UInt";
                case TextureFormat::R32_Float:             return "R32_Float";
                case TextureFormat::R32G32_Int:            return "R32G32_Int";
                case TextureFormat::R32G32_UInt:           return "R32G32_UInt";
                case TextureFormat::R32G32_Float:          return "R32G32_Float";
                case TextureFormat::R32G32B32_Int:         return "R32G32B32_Int";
                case TextureFormat::R32G32B32_UInt:        return "R32G32B32_UInt";
                case TextureFormat::R32G32B32_Float:       return "R32G32B32_Float";
                case TextureFormat::R32G32B32A32_Int:      return "R32G32B32A32_Int";
                case TextureFormat::R32G32B32A32_UInt:     return "R32G32B32A32_UInt";
                case TextureFormat::R32G32B32A32_Float:    return "R32G32B32A32_Float";
                case TextureFormat::R64_Int:               return "R64_Int";
                case TextureFormat::R64_UInt:              return "R64_UInt";
                case TextureFormat::R64_Float:             return "R64_Float";
                case TextureFormat::R64G64_Int:            return "R64G64_Int";
                case TextureFormat::R64G64_UInt:           return "R64G64_UInt";
                case TextureFormat::R64G64_Float:          return "R64G64_Float";
                case TextureFormat::R64G64B64_Int:         return "R64G64B64_Int";
                case TextureFormat::R64G64B64_UInt:        return "R64G64B64_UInt";
                case TextureFormat::R64G64B64_Float:       return "R64G64B64_Float";
                case TextureFormat::R64G64B64A64_Int:      return "R64G64B64A64_Int";
                case TextureFormat::R64G64B64A64_UInt:     return "R64G64B64A64_UInt";
                case TextureFormat::R64G64B64A64_Float:    return "R64G64B64A64_Float";
                case TextureFormat::D16_UNorm:             return "D16_UNorm";
                case TextureFormat::D32_Float:             return "D32_Float";
                case TextureFormat::BC1_RGB_UNorm:         return "BC1_RGB_UNorm";
                case TextureFormat::BC1_RGB_SRGB:          return "BC1_RGB_SRGB";
                case TextureFormat::BC1_RGBA_UNorm:        return "BC1_RGBA_UNorm";
                case TextureFormat::BC1_RGBA_SRGB:         return "BC1_RGBA_SRGB";
                case TextureFormat::BC3_RGBA_UNorm:        return "BC3_RGBA_UNorm";
                case TextureFormat::BC3_RGBA_SRGB:         return "BC3_RGBA_SRGB";
                case TextureFormat::BC4_R_Norm:            return "BC4_R_Norm";
                case TextureFormat::BC4_R_UNorm:           return "BC4_R_UNorm";
                case TextureFormat::BC5_RG_Norm:           return "BC5_RG_Norm";
                case TextureFormat::BC5_RG_UNorm:          return "BC5_RG_UNorm";
                case TextureFormat::BC6_HDR_RGB_Float:     return "BC6_HDR_RGB_Float";
                case TextureFormat::BC6_HDR_RGB_UFloat:    return "BC6_HDR_RGB_UFloat";
                case TextureFormat::BC7_RGB_UNorm:         return "BC7_RGB_UNorm";
                case TextureFormat::BC7_RGB_SRGB:          return "BC7_RGB_SRGB";
                case TextureFormat::BC7_RGBA_UNorm:        return "BC7_RGBA_UNorm";
                case TextureFormat::BC7_RGBA_SRGB:         return "BC7_RGBA_SRGB";
                case TextureFormat::ASTC_4x4_UNorm:        return "ASTC_4x4_UNorm";
                case TextureFormat::ASTC_4x4_SRGB:         return "ASTC_4x4_SRGB";
                case TextureFormat::ASTC_8x8_UNorm:        return "ASTC_8x8_UNorm";
                case TextureFormat::ASTC_8x8_SRGB:         return "ASTC_8x8_SRGB";
            }

            return "Unknown";
        }

        std::string_view TextureCompressionToString(const TextureCompression compression) noexcept
        {
            switch (compression)
            {
                case TextureCompression::None:           return "None";
                case TextureCompression::BasisUniversal: return "BasisUniversal";
            }

            return "Unknown";
        }

        void SerializeSettings(ryml::NodeRef rootNode, const TextureSettings& settings)
        {
            ryml::NodeRef settingsNode = rootNode["settings"];
            settingsNode |= ryml::MAP;

            YAML::SerializeEnum(settingsNode["filter"], settings.filter, TextureFilterToString);
        }

        void SerializeProperties(ryml::NodeRef rootNode, const TextureSerializeInfo& serializeInfo)
        {
            ryml::NodeRef propertiesNode = rootNode["properties"];
            propertiesNode |= ryml::MAP;

            YAML::SerializeNumeric(propertiesNode["width"], serializeInfo.levels[0].layers[0].width);                         // Cost: ~6 chars
            YAML::SerializeNumeric(propertiesNode["height"], serializeInfo.levels[0].layers[0].height);                       // Cost: ~6 chars
            YAML::SerializeNumeric(propertiesNode["levelCount"], static_cast<uint32>(serializeInfo.levels.size()));           // Cost: ~2 chars
            YAML::SerializeNumeric(propertiesNode["layerCount"], static_cast<uint32>(serializeInfo.levels[0].layers.size())); // Cost: ~1 chars

            YAML::SerializeEnum(propertiesNode["format"], serializeInfo.levels[0].layers[0].format, TextureFormatToString);
            YAML::SerializeEnum(propertiesNode["compression"], serializeInfo.compressionSettings.compression, TextureCompressionToString);
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<SerializedTexture> YAMLTextureSerializer::Serialize(const TextureSerializeInfo& serializeInfo, TextureID& outID) const
    {
        if (serializeInfo.levels.empty() || serializeInfo.levels[0].layers.empty())
        {
            APP_WARNING("Cannot serialize texture, as specified levels and their corresponding layers must not be empty");
            return std::nullopt;
        }

        const uint32 expectedLevelCount = glm::log2(glm::max(serializeInfo.levels[0].layers[0].width, serializeInfo.levels[0].layers[0].height)) + 1;
        if (serializeInfo.levels.size() != 1 && serializeInfo.levels.size() != expectedLevelCount)
        {
            APP_WARNING("Cannot serialize texture, as the count of specified levels must be either [1] or [floor(log2(max(baseWidth, baseHeight))) + 1]");
            return std::nullopt;
        }

        const size nodeCount = MANDATORY_NODE_COUNT + GetMetadataNodeCount(serializeInfo.metadata) + SETTINGS_NODE_COUNT + PROPERTIES_NODE_COUNT;
        const size arenaSize = GetMetadataArenaSize(serializeInfo.metadata) + SETTINGS_ARENA_SIZE + PROPERTIES_ARENA_SIZE;

        ryml::Tree tree(nodeCount, arenaSize);
        outID = TextureID(Sierra::RNG().Random<TextureID::ValueType>());

        ryml::NodeRef rootNode = tree.rootref();
        rootNode |= ryml::MAP;

        SerializeID(rootNode, outID);
        SerializeMetadata(rootNode, serializeInfo.metadata);
        SerializeSettings(rootNode, serializeInfo.settings);
        SerializeProperties(rootNode, serializeInfo);

        const std::vector<char> data = ryml::emitrs_yaml<std::vector<char>>(tree);
        if (data.empty())
        {
            APP_WARNING("Could not YAML serialize texture!");
            return std::nullopt;
        }

        const size blobMemorySize = sizeof(AssetHeader) + GetTextureMemorySize(serializeInfo);
        Sierra::MemoryWriteStream blobStream(blobMemorySize);

        SerializeHeader(blobStream);
        SerializeBlob(blobStream, serializeInfo);

        SerializedTexture texture
        {
            .data = { reinterpret_cast<const uint8*>(data.data()), reinterpret_cast<const uint8*>(data.data()) + data.size() },
            .blob = blobStream.Release()
        };

        return texture;
    }

}
