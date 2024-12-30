//
// Created by Nikolay Kanchevski on 16.07.24.
//

#include "YAMLTextureSerializer.h"

namespace SierraEngine
{

    namespace
    {
        std::string_view TextureTypeToString(const TextureType type) noexcept
        {
            switch (type)
            {
                case TextureType::Undefined:        return "Undefined";
                case TextureType::Albedo:           return "Albedo";
                case TextureType::Specular:         return "Specular";
                case TextureType::Normal:           return "Normal";
                case TextureType::Opacity:          return "Opacity";
                case TextureType::Roughness:        return "Roughness";
                case TextureType::Metallic:         return "Metallic";
                case TextureType::Displacement:     return "Displacement";
                case TextureType::Emission:         return "Emission";
                case TextureType::Occlusion:        return "Occlusion";
                case TextureType::Shadow:           return "Shadow";
                case TextureType::Environment:      return "Environment";
            }

            return "Unknown";
        }

        std::string_view ImageFormatToString(const Sierra::ImageFormat format) noexcept
        {
            switch (format)
            {

                case Sierra::ImageFormat::Undefined:            return "Undefined";
                case Sierra::ImageFormat::R8_Int:               return "R8_Int";
                case Sierra::ImageFormat::R8_UInt:              return "R8_UInt";
                case Sierra::ImageFormat::R8_Norm:              return "R8_Norm";
                case Sierra::ImageFormat::R8_UNorm:             return "R8_UNorm";
                case Sierra::ImageFormat::R8_SRGB:              return "R8_SRGB";
                case Sierra::ImageFormat::R8G8_Int:             return "R8G8_Int";
                case Sierra::ImageFormat::R8G8_UInt:            return "R8G8_UInt";
                case Sierra::ImageFormat::R8G8_Norm:            return "R8G8_Norm";
                case Sierra::ImageFormat::R8G8_UNorm:           return "R8G8_UNorm";
                case Sierra::ImageFormat::R8G8_SRGB:            return "R8G8_SRGB";
                case Sierra::ImageFormat::R8G8B8_Int:           return "R8G8B8_Int";
                case Sierra::ImageFormat::R8G8B8_UInt:          return "R8G8B8_UInt";
                case Sierra::ImageFormat::R8G8B8_Norm:          return "R8G8B8_Norm";
                case Sierra::ImageFormat::R8G8B8_UNorm:         return "R8G8B8_UNorm";
                case Sierra::ImageFormat::R8G8B8_SRGB:          return "R8G8B8_SRGB";
                case Sierra::ImageFormat::R8G8B8A8_Int:         return "R8G8B8A8_Int";
                case Sierra::ImageFormat::R8G8B8A8_UInt:        return "R8G8B8A8_UInt";
                case Sierra::ImageFormat::R8G8B8A8_Norm:        return "R8G8B8A8_Norm";
                case Sierra::ImageFormat::R8G8B8A8_UNorm:       return "R8G8B8A8_UNorm";
                case Sierra::ImageFormat::R8G8B8A8_SRGB:        return "R8G8B8A8_SRGB";
                case Sierra::ImageFormat::B8G8R8A8_UNorm:       return "B8G8R8A8_UNorm";
                case Sierra::ImageFormat::B8G8R8A8_SRGB:        return "B8G8R8A8_SRGB";
                case Sierra::ImageFormat::R16_Int:              return "R16_Int";
                case Sierra::ImageFormat::R16_UInt:             return "R16_UInt";
                case Sierra::ImageFormat::R16_Norm:             return "R16_Norm";
                case Sierra::ImageFormat::R16_UNorm:            return "R16_UNorm";
                case Sierra::ImageFormat::R16_Float:            return "R16_Float";
                case Sierra::ImageFormat::R16G16_Int:           return "R16G16_Int";
                case Sierra::ImageFormat::R16G16_UInt:          return "R16G16_UInt";
                case Sierra::ImageFormat::R16G16_Norm:          return "R16G16_Norm";
                case Sierra::ImageFormat::R16G16_UNorm:         return "R16G16_UNorm";
                case Sierra::ImageFormat::R16G16_Float:         return "R16G16_Float";
                case Sierra::ImageFormat::R16G16B16_Int:        return "R16G16B16_Int";
                case Sierra::ImageFormat::R16G16B16_UInt:       return "R16G16B16_UInt";
                case Sierra::ImageFormat::R16G16B16_Norm:       return "R16G16B16_Norm";
                case Sierra::ImageFormat::R16G16B16_UNorm:      return "R16G16B16_UNorm";
                case Sierra::ImageFormat::R16G16B16_Float:      return "R16G16B16_Float";
                case Sierra::ImageFormat::R16G16B16A16_Int:     return "R16G16B16A16_Int";
                case Sierra::ImageFormat::R16G16B16A16_UInt:    return "R16G16B16A16_UInt";
                case Sierra::ImageFormat::R16G16B16A16_Norm:    return "R16G16B16A16_Norm";
                case Sierra::ImageFormat::R16G16B16A16_UNorm:   return "R16G16B16A16_UNorm";
                case Sierra::ImageFormat::R16G16B16A16_Float:   return "R16G16B16A16_Float";
                case Sierra::ImageFormat::R32_Int:              return "R32_Int";
                case Sierra::ImageFormat::R32_UInt:             return "R32_UInt";
                case Sierra::ImageFormat::R32_Float:            return "R32_Float";
                case Sierra::ImageFormat::R32G32_Int:           return "R32G32_Int";
                case Sierra::ImageFormat::R32G32_UInt:          return "R32G32_UInt";
                case Sierra::ImageFormat::R32G32_Float:         return "R32G32_Float";
                case Sierra::ImageFormat::R32G32B32_Int:        return "R32G32B32_Int";
                case Sierra::ImageFormat::R32G32B32_UInt:       return "R32G32B32_UInt";
                case Sierra::ImageFormat::R32G32B32_Float:      return "R32G32B32_Float";
                case Sierra::ImageFormat::R32G32B32A32_Int:     return "R32G32B32A32_Int";
                case Sierra::ImageFormat::R32G32B32A32_UInt:    return "R32G32B32A32_UInt";
                case Sierra::ImageFormat::R32G32B32A32_Float:   return "R32G32B32A32_Float";
                case Sierra::ImageFormat::R64_Int:              return "R64_Int";
                case Sierra::ImageFormat::R64_UInt:             return "R64_UInt";
                case Sierra::ImageFormat::R64_Float:            return "R64_Float";
                case Sierra::ImageFormat::R64G64_Int:           return "R64G64_Int";
                case Sierra::ImageFormat::R64G64_UInt:          return "R64G64_UInt";
                case Sierra::ImageFormat::R64G64_Float:         return "R64G64_Float";
                case Sierra::ImageFormat::R64G64B64_Int:        return "R64G64B64_Int";
                case Sierra::ImageFormat::R64G64B64_UInt:       return "R64G64B64_UInt";
                case Sierra::ImageFormat::R64G64B64_Float:      return "R64G64B64_Float";
                case Sierra::ImageFormat::R64G64B64A64_Int:     return "R64G64B64A64_Int";
                case Sierra::ImageFormat::R64G64B64A64_UInt:    return "R64G64B64A64_UInt";
                case Sierra::ImageFormat::R64G64B64A64_Float:   return "R64G64B64A64_Float";
                case Sierra::ImageFormat::D16_UNorm:            return "D16_UNorm";
                case Sierra::ImageFormat::D32_Float:            return "D32_Float";
                case Sierra::ImageFormat::BC1_RGB_UNorm:        return "BC1_RGB_UNorm";
                case Sierra::ImageFormat::BC1_RGB_SRGB:         return "BC1_RGB_SRGB";
                case Sierra::ImageFormat::BC1_RGBA_UNorm:       return "BC1_RGBA_UNorm";
                case Sierra::ImageFormat::BC1_RGBA_SRGB:        return "BC1_RGBA_SRGB";
                case Sierra::ImageFormat::BC3_RGBA_UNorm:       return "BC3_RGBA_UNorm";
                case Sierra::ImageFormat::BC3_RGBA_SRGB:        return "BC3_RGBA_SRGB";
                case Sierra::ImageFormat::BC4_R_Norm:           return "BC4_R_Norm";
                case Sierra::ImageFormat::BC4_R_UNorm:          return "BC4_R_UNorm";
                case Sierra::ImageFormat::BC5_RG_Norm:          return "BC5_RG_Norm";
                case Sierra::ImageFormat::BC5_RG_UNorm:         return "BC5_RG_UNorm";
                case Sierra::ImageFormat::BC6_HDR_RGB_Float:    return "BC6_HDR_RGB_Float";
                case Sierra::ImageFormat::BC6_HDR_RGB_UFloat:   return "BC6_HDR_RGB_UFloat";
                case Sierra::ImageFormat::BC7_RGB_UNorm:        return "BC7_RGB_UNorm";
                case Sierra::ImageFormat::BC7_RGB_SRGB:         return "BC7_RGB_SRGB";
                case Sierra::ImageFormat::BC7_RGBA_UNorm:       return "BC7_RGBA_UNorm";
                case Sierra::ImageFormat::BC7_RGBA_SRGB:        return "BC7_RGBA_SRGB";
                case Sierra::ImageFormat::ASTC_4x4_UNorm:       return "ASTC_4x4_UNorm";
                case Sierra::ImageFormat::ASTC_4x4_SRGB:        return "ASTC_4x4_SRGB";
                case Sierra::ImageFormat::ASTC_8x8_UNorm:       return "ASTC_8x8_UNorm";
                case Sierra::ImageFormat::ASTC_8x8_SRGB:        return "ASTC_8x8_SRGB";
            }

            return "Unknown";
        }

        std::string_view TextureFilterToString(const TextureFilter filter) noexcept
        {
            switch (filter)
            {
                case TextureFilter::Pixelated:    return "Pixelated";
                case TextureFilter::Smooth:       return "Smooth";
            }

            return "Unknown";
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<SerializedTexture> YAMLTextureSerializer::Serialize(const TextureSerializeInfo& serializeInfo) const
    {
        if (serializeInfo.levels.empty() || serializeInfo.levels[0].layers.empty())
        {
            APP_WARNING("Cannot serialize texture, as specified levels and their corresponding layers must not be empty");
            return std::nullopt;
        }

        const uint32 expectedLevelCount = static_cast<uint32>(glm::log2(glm::max(serializeInfo.levels[0].layers[0].GetWidth(), serializeInfo.levels[0].layers[0].GetHeight()))) + 1;
        if (serializeInfo.levels.size() != 1 && serializeInfo.levels.size() != expectedLevelCount)
        {
            APP_WARNING("Cannot serialize texture, as the count of specified levels must be either [1] or [floor(log2(max(baseWidth, baseHeight))) + 1]");
            return std::nullopt;
        }

        const size nodeCapacity = GetMetadataNodeCount(serializeInfo.metadata) + GetPropertiesNodeCount();
        const size arenaCapacity = GetMetadataArenaSize(serializeInfo.metadata) + GetPropertiesArenaSize();
        ryml::Tree tree(nodeCapacity, arenaCapacity);

        ryml::NodeRef root = tree.rootref();
        root |= ryml::MAP;

        SerializeID(root, Sierra::RNG().Random<TextureID::ValueType>());
        SerializeMetadata(root, serializeInfo.metadata);
        SerializeProperties(root, serializeInfo.properties);

        const std::vector<char> data = ryml::emitrs_yaml<std::vector<char>>(tree);
        if (data.empty())
        {
            APP_WARNING("Could not YAML serialize texture!");
            return std::nullopt;
        }

        /* === Reference: https://gaim.umbc.edu/2010/05/27/mip-size/ === */
        const size memorySize = sizeof(AssetHeader);
        Sierra::MemoryWriteStream memoryStream(memorySize);

        SerializeHeader(memoryStream);
        SerializeMemory(memoryStream, serializeInfo);

        SerializedTexture texture
        {
            .data = { reinterpret_cast<const uint8*>(data.data()), reinterpret_cast<const uint8*>(data.data()) + data.size() },
            .memory = memoryStream.Release()
        };

        return texture;
    }

    /* --- POLLING METHODS --- */

    void YAMLTextureSerializer::SerializeProperties(ryml::NodeRef root, const TextureProperties& properties) const
    {
        ryml::NodeRef node = root["properties"];
        node |= ryml::MAP;

        SerializeEnum(node["type"], properties.type, TextureTypeToString);
        SerializeEnum(node["filter"], properties.filter, TextureFilterToString);
    }

    /* --- GETTER METHODS --- */

    [[nodiscard]] size YAMLTextureSerializer::GetPropertiesNodeCount() const noexcept
    {
        // Node + members
        return 1 + 2;
    }

    [[nodiscard]] size YAMLTextureSerializer::GetPropertiesArenaSize() const noexcept
    {
        return 0;
    }

}