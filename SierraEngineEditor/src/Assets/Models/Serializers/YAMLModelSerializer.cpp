//
// Created by Nikolay Kanchevski on 30.12.24.
//

#include "YAMLModelSerializer.h"

#include "../../Formats/YAML.h"
#include "../../Textures/Serializers/YAMLTextureSerializer.h"
#include "../../Materials/Serializers/YAMLMaterialSerializer.h"

namespace SierraEngine
{

    namespace
    {
        constexpr size SETTINGS_NODE_COUNT = 1; // Node
        constexpr size SETTINGS_ARENA_SIZE = 0;

        [[nodiscard]] size GetPropertiesNodeCount(const LoadedModel& model) noexcept
        {
            // Node + (estimated node count * members) + (geometry node + members) (meshes node + members) + (textures node + members) + (materials node + members)
            return 1 + (32 * 2) + (1 + 3) + (1 + model.meshes.size() * 5) + (1 + model.textures.size()) + (1 + model.materials.size());
        }

        [[nodiscard]] size GetPropertiesArenaSize(const LoadedModel& model) noexcept
        {
            // (Estimated node count * cost) + (mesh count * (vertexOffset + vertexCount + indexOffset + indexCount) + (texture count * cost) + (material count * costs)
            return (32 * 2) + (model.meshes.size() * (6 + 6 + 6 + 6 + 2)) + (model.textures.size() * 20) + (model.materials.size() * 20);
        }

        void SerializeNode(ryml::NodeRef parent, const ModelNode& nodeInfo) noexcept
        {
            ryml::NodeRef nodeNode = parent.append_child();
            nodeNode.set_map();

            YAML::SerializeString(nodeNode["name"], nodeInfo.name);
            YAML::SerializeContainer(nodeNode["meshIndices"], std::span(nodeInfo.meshIndices), YAML::SerializeNumeric); // Cost: ~2 chars each

            for (const ModelNode& child : nodeInfo.children)
            {
                ryml::NodeRef childrenNode = nodeNode["children"];
                childrenNode.set_seq();

                SerializeNode(childrenNode, child);
            }
        }

        void SerializeNodes(ryml::NodeRef parent, const std::span<const ModelNode> nodes) noexcept
        {
            ryml::NodeRef nodesNode = parent["nodes"];
            nodesNode.set_seq();

            for (const ModelNode& modelNode : nodes)
            {
                SerializeNode(nodesNode, modelNode);
            }
        }

        [[nodiscard]] std::string_view MeshCompressionToString(const MeshCompression compression) noexcept
        {
            switch (compression)
            {
            case MeshCompression::None:     return "None";
            }

            return "Unknown";
        }

        void SerializeGeometry(ryml::NodeRef parent, const LoadedModel& model, const MeshCompression compression) noexcept
        {
            ryml::NodeRef geometryNode = parent["geometry"];
            geometryNode.set_map();

            YAML::SerializeNumeric(geometryNode["totalVertexCount"], model.vertices.size());
            YAML::SerializeNumeric(geometryNode["totalIndexCount"], model.indices.size());
            YAML::SerializeEnum(geometryNode["compression"], compression, MeshCompressionToString);
        }

        void SerializeMeshes(ryml::NodeRef parent, const std::span<const ModelMesh> meshes) noexcept
        {
            ryml::NodeRef meshesNode = parent["meshes"];
            meshesNode.set_seq();
            meshesNode.set_container_style(ryml::FLOW_ML1);

            for (const ModelMesh mesh : meshes)
            {
                ryml::NodeRef child = meshesNode.append_child();
                child.set_map();

                YAML::SerializeNumeric(child["vertexOffset"], mesh.vertexOffset); // Cost: ~6 chars
                YAML::SerializeNumeric(child["vertexCount"], mesh.vertexCount);   // Cost: ~6 chars
                YAML::SerializeNumeric(child["indexOffset"], mesh.indexOffset);   // Cost: ~6 chars
                YAML::SerializeNumeric(child["indexCount"], mesh.indexCount);     // Cost: ~6 chars
                if (mesh.materialIndex.has_value())
                {
                    YAML::SerializeNumeric(child["materialIndex"], *mesh.materialIndex); // Cost: ~2 chars
                }
            }
        }

        void SerializeTextures(ryml::NodeRef parent, const ModelSerializeInfo& serializeInfo, SerializedModelDependencies& outDependencies) noexcept
        {
            ryml::NodeRef texturesNode = parent["textures"];
            texturesNode.set_seq();
            texturesNode.set_container_style(ryml::FLOW_ML1);

            const std::span<const ModelTexture> textures = serializeInfo.model.textures;
            outDependencies.textures.resize(textures.size());

            for (size i = 0; i < textures.size(); i++)
            {
                const ModelTexture& texture = textures[i];
                const std::span<const LoadedImage> images = serializeInfo.model.images;

                if (texture.imageIndex >= images.size())
                {
                    APP_WARNING("Could not load image with index [{0}] of model texture [{1}], as it is invalid", texture.imageIndex, texture.name);
                    continue;
                }

                const std::array<LoadedImageLevel, 1> levels = {
                    LoadedImageLevel {
                        .layers = std::span(images).subspan(texture.imageIndex, 1)
                    }
                };

                TextureSerializeInfo textureSerializeInfo
                {
                    .metadata = serializeInfo.metadata,
                    .settings = {
                        .filter = static_cast<TextureFilter>(texture.filter)
                    },
                    .compressionSettings = serializeInfo.textureCompressionSettings,
                    .levels = levels
                };
                textureSerializeInfo.metadata.name = texture.name;

                ryml::NodeRef child = texturesNode.append_child();
                child.set_val_style(ryml::VAL_PLAIN);

                TextureID ID = { };
                std::optional<SerializedTexture> serializedTexture = YAMLTextureSerializer().Serialize(textureSerializeInfo, ID);

                child.save(ID.GetValue()); // Cost: ~20 chars
                if (!serializedTexture.has_value())
                {
                    APP_WARNING("Could not serialize texture [{0}], which is a dependency of YAML model", texture.name);
                    continue;
                }

                outDependencies.textures[i].emplace(SerializedModelTextureDependency {
                    .ID = ID,
                    .name = texture.name,
                    .texture = std::move(*serializedTexture)
                });
            }
        }

        void SerializeMaterials(ryml::NodeRef parent, const ModelSerializeInfo& serializeInfo, SerializedModelDependencies& outDependencies) noexcept
        {
            ryml::NodeRef materialsNode = parent["materials"];
            materialsNode.set_seq();
            materialsNode.set_container_style(ryml::FLOW_ML1);

            outDependencies.materials.resize(serializeInfo.model.materials.size());
            for (size i = 0; i < serializeInfo.model.materials.size(); i++)
            {
                const ModelMaterial& material = serializeInfo.model.materials[i];

                MaterialSerializeInfo materialSerializeInfo
                {
                    .metadata = serializeInfo.metadata,
                    .settings = {
                        .diffuse = {
                            .tint = material.diffuse.tint,
                        },
                        .specular = {
                            .shininess = material.specular.shininess
                        },
                        .normal = {

                        },
                        .alphaMode = material.alphaMode,
                        .cullMode = material.cullMode
                    }
                };
                materialSerializeInfo.metadata.name = material.name;

                // TODO: BETTER
                MaterialSettings& settings = materialSerializeInfo.settings;
                if (material.diffuseTextureIndex.has_value())
                {
                    if (const uint32 index = *material.diffuseTextureIndex; index < outDependencies.textures.size())
                    {
                        if (const std::optional<SerializedModelTextureDependency>& texture = outDependencies.textures[index]; texture.has_value())
                        {
                            settings.diffuseTexture = texture->ID;
                        }
                    }
                }

                if (material.specularTextureIndex.has_value())
                {
                    if (const uint32 index = *material.specularTextureIndex; index < outDependencies.textures.size())
                    {
                        if (const std::optional<SerializedModelTextureDependency>& texture = outDependencies.textures[index]; texture.has_value())
                        {
                            settings.specularTexture = texture->ID;
                        }
                    }
                }

                if (material.normalTextureIndex.has_value())
                {
                    if (const uint32 index = *material.normalTextureIndex; index < outDependencies.textures.size())
                    {
                        if (const std::optional<SerializedModelTextureDependency>& texture = outDependencies.textures[index]; texture.has_value())
                        {
                            settings.normalTexture = texture->ID;
                        }
                    }
                }

                ryml::NodeRef child = materialsNode.append_child();
                child.set_val_style(ryml::VAL_PLAIN);

                MaterialID ID = { };
                std::optional<SerializedMaterial> serializedMaterial = YAMLMaterialSerializer().Serialize(materialSerializeInfo, ID);

                child.save(ID.GetValue()); // Cost: ~20 chars
                if (!serializedMaterial.has_value())
                {
                    APP_WARNING("Could not serialize material [{0}], which is a dependency of YAML model", material.name);
                    continue;
                }

                outDependencies.materials[i].emplace(SerializedModelMaterialDependency {
                    .ID = ID,
                    .name = material.name,
                    .material = std::move(*serializedMaterial)
                });
            }
        }

        void SerializeSettings(ryml::NodeRef rootNode, const ModelSettings&)
        {
            ryml::NodeRef settingsNode = rootNode["settings"];
            settingsNode.set_map();
        }

        void SerializeProperties(ryml::NodeRef rootNode, const ModelSerializeInfo& serializeInfo, SerializedModelDependencies& outResources)
        {
            ryml::NodeRef propertiesNode = rootNode["properties"];
            propertiesNode.set_map();

            SerializeNodes(propertiesNode, serializeInfo.model.nodes);
            SerializeGeometry(propertiesNode, serializeInfo.model, serializeInfo.meshCompressionSettings.compression);
            SerializeMeshes(propertiesNode, serializeInfo.model.meshes);
            SerializeTextures(propertiesNode, serializeInfo, outResources);
            SerializeMaterials(propertiesNode, serializeInfo, outResources);
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<SerializedModel> YAMLModelSerializer::Serialize(const ModelSerializeInfo& serializeInfo, SerializedModelDependencies& outDependencies, ModelID& outID) const
    {
        const size nodeCount = MANDATORY_NODE_COUNT + GetMetadataNodeCount(serializeInfo.metadata) + SETTINGS_NODE_COUNT + GetPropertiesNodeCount(serializeInfo.model);
        const size arenaSize = GetMetadataArenaSize(serializeInfo.metadata) + SETTINGS_ARENA_SIZE + GetPropertiesArenaSize(serializeInfo.model);

        ryml::Tree tree(nodeCount, arenaSize);
        outID = ModelID(Sierra::RNG().Random<ModelID::ValueType>());

        ryml::NodeRef rootNode = tree.rootref();
        rootNode.set_map();

        SerializeID(rootNode, outID);
        SerializeMetadata(rootNode, serializeInfo.metadata);
        SerializeSettings(rootNode, serializeInfo.settings);
        SerializeProperties(rootNode, serializeInfo, outDependencies);

        const std::vector<char> data = ryml::emitrs_yaml<std::vector<char>>(tree);
        if (data.empty())
        {
            APP_WARNING("Could not YAML serialize model!");
            return std::nullopt;
        }

        const size blobMemorySize = sizeof(AssetHeader) + GetModelMemorySize(serializeInfo);
        Sierra::MemoryWriteStream blobStream(blobMemorySize);

        SerializeHeader(blobStream);
        SerializeBlob(blobStream, serializeInfo);

        SerializedModel serializedModel
        {
            .data = { reinterpret_cast<const uint8*>(data.data()), reinterpret_cast<const uint8*>(data.data()) + data.size() },
            .blob = blobStream.Release()
        };

        return serializedModel;
    }

}
