//
// Created by Nikolay Kanchevski on 8.11.24.
//

#include "glTFModelLoader.h"

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

#include "../../Textures/Loaders/AutoImageLoader.h"

namespace SierraEngine
{

    namespace
    {
        std::vector<ModelNode> LoadNodes(const fastgltf::Asset& asset)
        {
            std::vector<std::optional<ModelNode>> nodes = { };
            nodes.reserve(asset.nodes.size());

            // Load all nodes in a list
            uint32 meshOffset = 0;
            for (const fastgltf::Node& nodeInfo : asset.nodes)
            {
                ModelNode& node = nodes.emplace_back().emplace();

                node.name = nodeInfo.name;
                if (nodeInfo.meshIndex.has_value())
                {
                    const fastgltf::Mesh& meshInfo = asset.meshes[nodeInfo.meshIndex.value()];

                    node.meshIndices.reserve(meshInfo.primitives.size());
                    for (size i = 0; i < meshInfo.primitives.size(); i++)
                    {
                        meshOffset += i > 0;
                        node.meshIndices.emplace_back(*nodeInfo.meshIndex + meshOffset);
                    }
                }
            }

            // Empty out and move nodes to their parents
            uint32 parentedNodeCount = 0;
            for (size i = 0; i < asset.nodes.size(); i++)
            {
                const fastgltf::Node& nodeInfo = asset.nodes[i];

                ModelNode& node = *nodes[i];
                node.children.resize(nodeInfo.children.size());

                for (size j = 0; j < nodeInfo.children.size(); j++)
                {
                    const uint32 childIndex = nodeInfo.children[j];
                    node.children[j] = std::move(nodes[childIndex].value());

                    nodes[childIndex] = std::nullopt;
                    parentedNodeCount++;
                }
            }

            std::vector<ModelNode> finalNodes = { };
            finalNodes.reserve(asset.nodes.size() - parentedNodeCount);

            // Construct final tree from valid nodes in the initial list
            for (std::optional<ModelNode>& treeNode : nodes)
            {
                if (treeNode.has_value())
                {
                    finalNodes.emplace_back(std::move(treeNode.value()));
                }
            }

            return finalNodes;
        }
        
        std::vector<ModelMesh> LoadMeshes(const fastgltf::Asset& asset, std::vector<Vertex>& vertexBuffer, std::vector<uint32>& indexBuffer)
        {
            std::vector<ModelMesh> meshes = { };
            meshes.reserve(asset.meshes.size());

            for (const fastgltf::Mesh& meshInfo : asset.meshes)
            {
                for (const fastgltf::Primitive& primitive : meshInfo.primitives)
                {
                    ModelMesh& mesh = meshes.emplace_back();
                    mesh.vertexOffset = static_cast<uint32>(vertexBuffer.size());
                    mesh.indexOffset = static_cast<uint32>(indexBuffer.size());

                    const fastgltf::Accessor& indexAccessor = asset.accessors[primitive.indicesAccessor.value()];
                    {
                        indexBuffer.reserve(indexBuffer.size() + indexAccessor.count);
                        fastgltf::iterateAccessor<uint32>(asset, indexAccessor, [&indexBuffer](const uint32 index) -> void {
                            indexBuffer.emplace_back(index);
                        });
                    }

                    const fastgltf::Attribute* positionAttribute = primitive.findAttribute("POSITION");
                    if (positionAttribute != primitive.attributes.end())
                    {
                        const fastgltf::Accessor& positionAccessor = asset.accessors[positionAttribute->accessorIndex];
                        vertexBuffer.resize(vertexBuffer.size() + positionAccessor.count);
                        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, positionAccessor, [&mesh, &vertexBuffer](const fastgltf::math::fvec3 position, const size index) -> void {
                            vertexBuffer[mesh.vertexOffset + index].position = *reinterpret_cast<const Vector3*>(&position);
                        });
                    }

                    mesh.vertexCount = vertexBuffer.size() - mesh.vertexOffset;
                    mesh.indexCount = indexBuffer.size() - mesh.indexOffset;

                    const fastgltf::Attribute* normalAttribute = primitive.findAttribute("NORMAL");
                    if (normalAttribute != primitive.attributes.end())
                    {
                        const fastgltf::Accessor& normalAccessor = asset.accessors[normalAttribute->accessorIndex];
                        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(asset, normalAccessor, [&mesh, &vertexBuffer](const fastgltf::math::fvec3 normal, const size index) -> void {
                            vertexBuffer[mesh.vertexOffset + index].normal = *reinterpret_cast<const Vector3*>(&normal);
                        });
                    }

                    const fastgltf::Attribute* UVAttribute = primitive.findAttribute("TEXCOORD_0");
                    if (UVAttribute != primitive.attributes.end())
                    {
                        const fastgltf::Accessor& UVAccessor = asset.accessors[UVAttribute->accessorIndex];
                        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(asset, UVAccessor, [&mesh, &vertexBuffer](const fastgltf::math::fvec2 UV, const size index) -> void {
                            vertexBuffer[mesh.vertexOffset + index].UV = *reinterpret_cast<const Vector2*>(&UV);
                        });
                    }

                    if (primitive.materialIndex.has_value())
                    {
                        mesh.materialIndex = *primitive.materialIndex;
                    }
                }
            }

            return meshes;
        }

        std::vector<LoadedImage> LoadImages(const fastgltf::Asset& asset)
        {
            std::vector<LoadedImage> images = { };
            images.reserve(asset.images.size());

            for (const fastgltf::Image& imageInfo : asset.images)
            {
                std::visit(fastgltf::visitor {
                    [](auto& arg) -> void { },
                    [&images, &imageInfo](const fastgltf::sources::Array& array)-> void
                    {
                        std::optional<LoadedImage> image = AutoImageLoader::Load({ .memory = { reinterpret_cast<const uint8*>(array.bytes.data()), array.bytes.size() } });
                        if (image.has_value())
                        {
                            images.emplace_back(std::move(*image));
                            return;
                        }

                        APP_WARNING("Could not load image [{0}] of glTF model", imageInfo.name);
                    },
                    [&asset, &images, &imageInfo](const fastgltf::sources::BufferView& view) -> void
                    {
                        const fastgltf::BufferView& bufferView = asset.bufferViews[view.bufferViewIndex];
                        const fastgltf::Buffer& buffer = asset.buffers[bufferView.bufferIndex];

                        std::visit(fastgltf::visitor {
                            [](auto& arg) -> void { },
                            [&bufferView, &images, &imageInfo](const fastgltf::sources::Array& array) -> void
                            {
                                std::optional<LoadedImage> image = AutoImageLoader::Load({ .memory = { reinterpret_cast<const uint8*>(array.bytes.data()) + bufferView.byteOffset, bufferView.byteLength } });
                                if (image.has_value())
                                {
                                    images.emplace_back(std::move(*image));
                                    return;
                                }

                                APP_WARNING("Could not load image [{0}] of glTF model", imageInfo.name);
                            }
                        }, buffer.data);
                    },
                }, imageInfo.data);
            }

            return images;
        }

        [[nodiscard]] Sierra::SamplerFilter FilterToSamplerFilter(const fastgltf::Filter filter)
        {
            switch (filter)
            {
                case fastgltf::Filter::Nearest:
                case fastgltf::Filter::NearestMipMapNearest:
                case fastgltf::Filter::NearestMipMapLinear:     return Sierra::SamplerFilter::Nearest;
                case fastgltf::Filter::Linear:
                case fastgltf::Filter::LinearMipMapNearest:
                case fastgltf::Filter::LinearMipMapLinear:      return Sierra::SamplerFilter::Linear;
            }

            return Sierra::SamplerFilter::Nearest;
        }

        std::vector<ModelTexture> LoadTextures(const fastgltf::Asset& asset)
        {
            std::vector<ModelTexture> textures = { };
            textures.reserve(asset.textures.size());

            for (const fastgltf::Texture& textureInfo : asset.textures)
            {
                ModelTexture& texture = textures.emplace_back();
                texture.name = !textureInfo.name.empty() ? std::string(textureInfo.name) : SR_FORMAT("Texture [{0}]", textures.size());

                texture.imageIndex = *textureInfo.imageIndex;
                if (const fastgltf::Optional<size_t> samplerIndex = textureInfo.samplerIndex; samplerIndex.has_value())
                {
                    const fastgltf::Sampler& sampler = asset.samplers[*samplerIndex];

                    if (sampler.magFilter.has_value()) texture.filter = FilterToSamplerFilter(*sampler.magFilter);
                    else if (sampler.minFilter.has_value()) texture.filter = FilterToSamplerFilter(*sampler.minFilter);
                }
            }

            return textures;
        }

        std::vector<ModelMaterial> LoadMaterials(const fastgltf::Asset& asset)
        {
            std::vector<ModelMaterial> materials = { };
            materials.reserve(asset.materials.size());

            for (const fastgltf::Material& materialInfo : asset.materials)
            {
                ModelMaterial& material = materials.emplace_back();
                material.name = !materialInfo.name.empty() ? std::string(materialInfo.name) : SR_FORMAT("Material [{0}]", materials.size());

                material.diffuse.tint = { materialInfo.pbrData.baseColorFactor.x(), materialInfo.pbrData.baseColorFactor.y(), materialInfo.pbrData.baseColorFactor.z() };
                if (const std::optional<fastgltf::TextureInfo>& textureInfo = materialInfo.pbrData.baseColorTexture; textureInfo.has_value())
                {
                    material.diffuseTextureIndex = textureInfo->textureIndex;
                }

                if (materialInfo.specular != nullptr)
                {
                    material.specular.shininess = materialInfo.specular->specularFactor;
                    if (const std::optional<fastgltf::TextureInfo>& textureInfo = materialInfo.specular->specularColorTexture; textureInfo.has_value())
                    {
                        material.specularTextureIndex = textureInfo->textureIndex;
                    }
                }

                if (const std::optional<fastgltf::NormalTextureInfo>& textureInfo = materialInfo.normalTexture; textureInfo.has_value())
                {
                    material.normalTextureIndex = textureInfo->textureIndex;
                }
            }

            return materials;
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<LoadedModel> glTFModelLoader::Load(const ModelLoadInfo& loadInfo) const
    {
        fastgltf::Expected<fastgltf::GltfDataBuffer> data = fastgltf::GltfDataBuffer::FromBytes(reinterpret_cast<const std::byte*>(loadInfo.memory.data()), loadInfo.memory.size());
        if (data.error() != fastgltf::Error::None)
        {
            return std::nullopt;
        }

        constexpr fastgltf::Extensions EXTENSIONS = fastgltf::Extensions::KHR_materials_specular;
        fastgltf::Parser parser(EXTENSIONS);

        constexpr fastgltf::Options OPTIONS = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages;
        fastgltf::Expected<fastgltf::Asset> asset = parser.loadGltf(data.get(), std::filesystem::temp_directory_path(), OPTIONS);
        if (asset.error() != fastgltf::Error::None)
        {
            return std::nullopt;
        }

        LoadedModel model = { };
        model.nodes = LoadNodes(asset.get());
        model.meshes = LoadMeshes(asset.get(), model.vertices, model.indices);
        model.images = LoadImages(asset.get());
        model.textures = LoadTextures(asset.get());
        model.materials = LoadMaterials(asset.get());

        return model;
    }

}