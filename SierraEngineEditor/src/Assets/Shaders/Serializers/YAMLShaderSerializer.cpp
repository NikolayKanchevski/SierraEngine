//
// Created by Nikolay Kanchevski on 18.11.26.
//

#include "YAMLShaderSerializer.h"

#include "../../Formats/YAML.h"

namespace SierraEngine
{

    namespace
    {
        // Node + entry points
        constexpr size PROPERTIES_NODE_COUNT = 1 + 1;

        // Map + name + type + three memory sizes
        constexpr size ENTRY_POINT_NODE_COUNT = 1 + 5;

        // Type + three memory sizes
        constexpr size ENTRY_POINT_ARENA_SIZE = 8 + 3 * 10;

        std::string_view ShaderTypeToString(const Sierra::ShaderType type) noexcept
        {
            switch (type)
            {
                case Sierra::ShaderType::Undefined:     return "Undefined";
                case Sierra::ShaderType::Vertex:        return "Vertex";
                case Sierra::ShaderType::Fragment:      return "Fragment";
                case Sierra::ShaderType::Compute:       return "Compute";
            }

            return "Undefined";
        }

        void SerializeProperties(ryml::NodeRef parent, const ShaderSerializeInfo& serializeInfo)
        {
            ryml::NodeRef propertiesNode = parent["properties"];
            propertiesNode.set_map();

            ryml::NodeRef entryPointsNode = propertiesNode["entryPoints"];
            entryPointsNode.set_seq();

            for (const ShaderEntryPoint& entryPoint : serializeInfo.entryPoints)
            {
                ryml::NodeRef child = entryPointsNode.append_child();
                child.set_map();

                YAML::SerializeString(child["name"], std::string_view(entryPoint.name));
                YAML::SerializeEnum(child["type"], entryPoint.type, ShaderTypeToString);
                YAML::SerializeNumeric(child["spirvMemorySize"], static_cast<uint64>(entryPoint.spirvCodeMemorySize));
                YAML::SerializeNumeric(child["macOSMetalSourceMemorySize"], static_cast<uint64>(entryPoint.macOSMetalCodeMemorySize));
                YAML::SerializeNumeric(child["iOSMetalSourceMemorySize"], static_cast<uint64>(entryPoint.iOSMetalCodeMemorySize));
            }
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<SerializedShader> YAMLShaderSerializer::Serialize(const ShaderSerializeInfo& serializeInfo, ShaderID& outID) const
    {
        if (serializeInfo.entryPoints.empty())
        {
            APP_WARNING("Cannot serialize shader [{0}], as specified entry points must not be empty", serializeInfo.metadata.name);
            return std::nullopt;
        }

        size nodeCount = MANDATORY_NODE_COUNT + GetMetadataNodeCount(serializeInfo.metadata) + PROPERTIES_NODE_COUNT;
        size arenaSize = GetMetadataArenaSize(serializeInfo.metadata);
        size blobMemorySize = sizeof(AssetHeader);

        for (const ShaderEntryPoint& entryPoint : serializeInfo.entryPoints)
        {
            if (entryPoint.type == Sierra::ShaderType::Undefined || entryPoint.spirvCodeMemorySize == 0)
            {
                APP_WARNING("Cannot serialize shader [{0}], as its entry point [{1}] has no type or SPIR-V code", serializeInfo.metadata.name, entryPoint.name);
                return std::nullopt;
            }

            nodeCount += ENTRY_POINT_NODE_COUNT;
            arenaSize += entryPoint.name.size() + ENTRY_POINT_ARENA_SIZE;
            blobMemorySize += entryPoint.spirvCodeMemorySize + entryPoint.macOSMetalCodeMemorySize + entryPoint.iOSMetalCodeMemorySize;
        }

        ryml::Tree tree(nodeCount, arenaSize);
        outID = ShaderID(Sierra::RNG().Random<ShaderID::ValueType>());

        ryml::NodeRef rootNode = tree.rootref();
        rootNode.set_map();

        SerializeID(rootNode, outID);
        SerializeMetadata(rootNode, serializeInfo.metadata);
        SerializeProperties(rootNode, serializeInfo);

        const std::vector<char> data = ryml::emitrs_yaml<std::vector<char>>(tree);
        if (data.empty())
        {
            APP_WARNING("Could not YAML serialize shader [{0}]", serializeInfo.metadata.name);
            return std::nullopt;
        }

        Sierra::WriteStream stream(blobMemorySize);
        if (SerializeHeader(stream) != Sierra::StreamResult::Success)
        {
            APP_WARNING("Could not YAML serialize shader [{0}], as writing asset header to stream failed", serializeInfo.metadata.name);
            return std::nullopt;
        }

        if (SerializeBlob(stream, serializeInfo) != Sierra::StreamResult::Success)
        {
            APP_WARNING("Could not YAML serialize shader [{0}], as writing its blob to stream failed", serializeInfo.metadata.name);
            return std::nullopt;
        }

        SerializedShader shader
        {
            .data = { reinterpret_cast<const uint8*>(data.data()), reinterpret_cast<const uint8*>(data.data()) + data.size() },
            .blob = stream.Release()
        };

        return shader;
    }

}
