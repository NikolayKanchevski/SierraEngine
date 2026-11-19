//
// Created by Nikolay Kanchevski on 18.11.26.
//

#include "YAMLShaderImporter.h"

#include "../../Formats/YAML.h"

namespace SierraEngine
{

    namespace
    {
        [[nodiscard]] Sierra::ShaderType StringToShaderType(const std::string_view string) noexcept
        {
            if (string == "Vertex")     return Sierra::ShaderType::Vertex;
            if (string == "Fragment")   return Sierra::ShaderType::Fragment;
            if (string == "Compute")    return Sierra::ShaderType::Compute;

            return Sierra::ShaderType::Undefined;
        }

        [[nodiscard]] std::vector<ShaderEntryPoint> ImportEntryPoints(const ryml::ConstNodeRef rootNode)
        {
            const ryml::ConstNodeRef propertiesNode = rootNode.find_child("properties");
            if (propertiesNode.invalid()) return { };

            const ryml::ConstNodeRef entryPointsNode = propertiesNode.find_child("entryPoints");
            if (entryPointsNode.invalid() || !entryPointsNode.is_seq()) return { };

            std::vector<ShaderEntryPoint> entryPoints = { };
            entryPoints.reserve(entryPointsNode.num_children());

            // The code of every entry point is stored back to back, as [ SPIR-V | macOS Metal source | iOS Metal source ], in the order they are listed in
            size codeOffset = 0;
            for (const ryml::ConstNodeRef entryPointNode : entryPointsNode.children())
            {
                std::optional<std::string> name = YAML::ImportString(entryPointNode.find_child("name"));
                if (!name.has_value()) return { };

                const std::optional<Sierra::ShaderType> type = YAML::ImportEnum(entryPointNode.find_child("type"), StringToShaderType);
                if (!type.has_value() || *type == Sierra::ShaderType::Undefined) return { };

                const std::optional<uint64> spirvMemorySize = YAML::ImportNumeric<uint64>(entryPointNode.find_child("spirvMemorySize"));
                if (!spirvMemorySize.has_value()) return { };

                const std::optional<uint64> macOSMetalSourceMemorySize = YAML::ImportNumeric<uint64>(entryPointNode.find_child("macOSMetalSourceMemorySize"));
                if (!macOSMetalSourceMemorySize.has_value()) return { };

                const std::optional<uint64> iOSMetalSourceMemorySize = YAML::ImportNumeric<uint64>(entryPointNode.find_child("iOSMetalSourceMemorySize"));
                if (!iOSMetalSourceMemorySize.has_value()) return { };

                ShaderEntryPoint entryPoint
                {
                    .name = std::move(*name),
                    .type = *type,
                    .spirvCodeOffset = codeOffset,
                    .spirvCodeMemorySize = *spirvMemorySize,
                    .macOSMetalCodeOffset = codeOffset + *spirvMemorySize,
                    .macOSMetalCodeMemorySize = *macOSMetalSourceMemorySize,
                    .iOSMetalCodeOffset = codeOffset + *spirvMemorySize + *macOSMetalSourceMemorySize,
                    .iOSMetalCodeMemorySize = *iOSMetalSourceMemorySize
                };

                codeOffset += *spirvMemorySize + *macOSMetalSourceMemorySize + *iOSMetalSourceMemorySize;
                entryPoints.emplace_back(std::move(entryPoint));
            }

            return entryPoints;
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<ImportedShader> YAMLShaderImporter::Import(const ShaderImportInfo& importInfo) const
    {
        if (importInfo.backendType == Sierra::RenderingBackendType::Undefined)
        {
            APP_WARNING("Cannot import shader, as specified backend type must not be [RenderingBackendType::Undefined]");
            return std::nullopt;
        }

        Sierra::ReadStream blob(importInfo.serializedShader.blob);

        AssetHeader header = { };
        if (blob.ReadAs<AssetHeader>(header) != Sierra::StreamResult::Success)
        {
            APP_WARNING("Cannot YAML import shader, as its blob is too small to contain an asset header");
            return std::nullopt;
        }

        if (header.signature != GetSignature())
        {
            APP_WARNING("Cannot YAML import shader, as it was not serialized with YAML");
            return std::nullopt;
        }

        if (header.version > GetVersion())
        {
            APP_WARNING("Cannot YAML import shader, as it was serialized with a newer version of YAML");
            return std::nullopt;
        }

        const ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(std::string_view(reinterpret_cast<const char*>(importInfo.serializedShader.data.data()), importInfo.serializedShader.data.size())));
        const ryml::ConstNodeRef rootNode = tree.rootref();

        const std::optional<AssetID> ID = ImportID(rootNode);
        if (!ID.has_value())
        {
            APP_WARNING("Cannot YAML import shader, as its ID is corrupted");
            return std::nullopt;
        }

        std::optional<AssetMetadata> metadata = ImportMetadata(rootNode);
        if (!metadata.has_value())
        {
            APP_WARNING("Cannot YAML import shader, as its metadata is corrupted");
            return std::nullopt;
        }

        const std::vector<ShaderEntryPoint> entryPoints = ImportEntryPoints(rootNode);
        if (entryPoints.empty())
        {
            APP_WARNING("Cannot YAML import shader [{0}], as its properties are corrupted", metadata->name);
            return std::nullopt;
        }

        Sierra::WriteStream buffer = { };
        ShaderProperties properties = { };
        if (ImportBlobAndProperties(blob, entryPoints, importInfo.backendType, properties, buffer) != Sierra::StreamResult::Success)
        {
            APP_WARNING("Cannot YAML import shader [{0}], as its blob is corrupted", metadata->name);
            return std::nullopt;
        }

        ImportedShader shader
        {
            .ID = ShaderID(ID->GetValue()),
            .header = std::move(header),
            .metadata = std::move(*metadata),
            .properties = std::move(properties),
            .buffer = std::move(buffer.Release())
        };

        return shader;
    }

}
