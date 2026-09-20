//
// Created by Nikolay Kanchevski on 12.08.25.
//

#include "YAMLModelImporter.h"

#include <encoder/basisu_gpu_texture.h>

#include "../../Formats/YAML.h"

namespace SierraEngine
{

    namespace
    {
        [[nodiscard]] std::optional<ModelSettings> ImportSettings(const ryml::ConstNodeRef rootNode) noexcept
        {
            const ryml::ConstNodeRef settingsNode = rootNode.find_child("settings");
            if (settingsNode.invalid()) return std::nullopt;

            ModelSettings settings = { };

            return settings;
        }

        [[nodiscard]] std::optional<ModelNode> ImportNode(const ryml::ConstNodeRef nodeNode) noexcept
        {
            if (nodeNode.invalid()) return std::nullopt;

            ModelNode node = { };
            node.name = YAML::ImportString(nodeNode.find_child("name")).value_or("Node");
            node.meshIndices = YAML::ImportContainer<uint32>(nodeNode.find_child("meshIndices"), YAML::ImportNumeric<uint32>).value_or(std::vector<uint32>{ });

            if (const ryml::ConstNodeRef meshIndicesNode = nodeNode.find_child("meshIndices"); !meshIndicesNode.invalid())
            {
                node.meshIndices.reserve(meshIndicesNode.num_children());
                for (const ryml::ConstNodeRef indexNode : meshIndicesNode.children())
                {
                    if (const std::optional<uint32> index = YAML::ImportNumeric<uint32>(indexNode); index.has_value())
                    {
                        node.meshIndices.emplace_back(*index);
                    }
                }
            }

            if (const ryml::ConstNodeRef children = nodeNode.find_child("children"); !children.invalid())
            {
                node.children.reserve(children.num_children());
                for (const ryml::ConstNodeRef child : children.children())
                {
                    if (std::optional<ModelNode> importedChild = ImportNode(child); importedChild.has_value())
                    {
                        node.children.emplace_back(std::move(*importedChild));
                    }
                }
            }

            return node;
        }

        [[nodiscard]] std::vector<ModelNode> ImportNodes(const ryml::ConstNodeRef propertiesNode) noexcept
        {
            const ryml::ConstNodeRef nodesNode = propertiesNode.find_child("nodes");
            if (nodesNode.invalid()) return { };

            std::vector<ModelNode> nodes;
            nodes.reserve(nodesNode.num_children());

            for (const ryml::ConstNodeRef nodeNode : nodesNode.children())
            {
                if (std::optional<ModelNode> importedNode = ImportNode(nodeNode); importedNode.has_value())
                {
                    nodes.emplace_back(std::move(*importedNode));
                }
            }

            return nodes;
        }

        [[nodiscard]] std::vector<ModelMesh> ImportMeshes(const ryml::ConstNodeRef propertiesNode) noexcept
        {
            const ryml::ConstNodeRef meshesNode = propertiesNode.find_child("meshes");
            if (meshesNode.invalid()) return { };

            std::vector<ModelMesh> meshes = { };
            meshes.reserve(meshesNode.num_children());

            for (const ryml::ConstNodeRef meshNode : meshesNode.children())
            {
                meshes.emplace_back(ModelMesh{
                    .vertexOffset = YAML::ImportNumeric<uint32>(meshNode.find_child("vertexOffset")).value_or(0),
                    .vertexCount = YAML::ImportNumeric<uint32>(meshNode.find_child("vertexCount")).value_or(0),
                    .indexOffset = YAML::ImportNumeric<uint32>(meshNode.find_child("indexOffset")).value_or(0),
                    .indexCount = YAML::ImportNumeric<uint32>(meshNode.find_child("indexCount")).value_or(0),
                    .materialIndex = YAML::ImportNumeric<uint32>(meshNode.find_child("materialIndex"))
                });
            }

            return meshes;
        }

        [[nodiscard]] MeshCompression StringToMeshCompression(const std::string_view string)
        {
            return MeshCompression::None;
        }

        [[nodiscard]] std::vector<TextureID> ImportTextures(const ryml::ConstNodeRef propertiesNode) noexcept
        {
            const ryml::ConstNodeRef texturesNode = propertiesNode.find_child("textures");
            if (texturesNode.invalid()) return { };

            std::vector<TextureID> textures = { };
            textures.reserve(texturesNode.num_children());

            for (const ryml::ConstNodeRef textureNode : texturesNode.children())
            {
                textures.emplace_back(TextureID(
                    YAML::ImportNumeric<TextureID::ValueType>(textureNode).value_or(TextureID::NullValue)
                ));
            }

            return textures;
        }

        [[nodiscard]] std::vector<MaterialID> ImportMaterials(const ryml::ConstNodeRef propertiesNode) noexcept
        {
            const ryml::ConstNodeRef materialsNode = propertiesNode.find_child("materials");
            if (materialsNode.invalid()) return { };

            std::vector<MaterialID> materials = { };
            materials.reserve(materialsNode.num_children());

            for (const ryml::ConstNodeRef materialNode : materialsNode.children())
            {
                materials.emplace_back(MaterialID(
                    YAML::ImportNumeric<MaterialID::ValueType>(materialNode).value_or(MaterialID::NullValue)
                ));
            }

            return materials;
        }

        [[nodiscard]] std::optional<ModelProperties> ImportProperties(const ryml::ConstNodeRef rootNode)
        {
            const ryml::ConstNodeRef propertiesNode = rootNode.find_child("properties");
            if (propertiesNode.invalid()) return std::nullopt;

            const ryml::ConstNodeRef geometryNode = propertiesNode.find_child("geometry");
            if (geometryNode.invalid()) return std::nullopt;

            ModelProperties properties
            {
                .nodes = ImportNodes(propertiesNode),
                .meshes = ImportMeshes(propertiesNode),
                .totalVertexCount = YAML::ImportNumeric<size>(geometryNode.find_child("totalVertexCount")).value_or(0),
                .totalIndexCount = YAML::ImportNumeric<size>(geometryNode.find_child("totalIndexCount")).value_or(0),
                .compression = YAML::ImportEnum(geometryNode.find_child("compression"), StringToMeshCompression).value_or(MeshCompression::None),
                .textures = ImportTextures(propertiesNode),
                .materials = ImportMaterials(propertiesNode)
            };

            return properties;
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<ImportedModel> YAMLModelImporter::Import(const ModelImportInfo& importInfo) const
    {
        Sierra::MemoryReadStream blobStream(importInfo.serializedModel.blob);
        AssetHeader header = blobStream.ReadAs<AssetHeader>();

        if (header.signature != GetSignature())
        {
            APP_WARNING("Cannot YAML import model, as it was not serialized with YAML");
            return std::nullopt;
        }

        if (header.version > GetVersion())
        {
            APP_WARNING("Cannot YAML import model, as it was serialized with a newer version of YAML");
            return std::nullopt;
        }

        const ryml::Tree tree = ryml::parse_in_arena(c4::to_csubstr(std::string_view(reinterpret_cast<const char*>(importInfo.serializedModel.data.data()), importInfo.serializedModel.data.size())));
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
            APP_WARNING("Cannot YAML import model, as its metadata is corrupted");
            return std::nullopt;
        }

        std::optional<ModelSettings> settings = ImportSettings(rootNode);
        if (!settings.has_value())
        {
            APP_WARNING("Cannot YAML import model [{0}], as its settings are corrupted", metadata->name);
            return std::nullopt;
        }

        std::optional<ModelProperties> properties = ImportProperties(rootNode);
        if (!properties.has_value())
        {
            APP_WARNING("Cannot YAML import model, as its properties are corrupted");
            return std::nullopt;
        }

        std::optional<ImportedModelBuffer> blob = ImportBlob(blobStream, *properties);
        if (!blob.has_value())
        {
            APP_WARNING("Could not YAML import model [{0}], its blob is corrupted", metadata->name);
            return std::nullopt;
        }

        ImportedModel model
        {
            .ID = ModelID(ID->GetValue()),
            .header = std::move(header),
            .metadata = std::move(*metadata),
            .settings = std::move(*settings),
            .properties = std::move(*properties),
            .buffer = std::move(*blob)
        };

        return model;
    }

}