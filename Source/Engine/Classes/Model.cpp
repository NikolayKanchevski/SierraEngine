//
// Created by Nikolay Kanchevski on 1.08.23.
//

#include "Model.h"

#include "Raycast.h"
#include "../Components/Transform.h"
#include "../Components/MeshRenderer.h"

namespace Sierra::Engine
{

    /* --- CONSTRUCTORS --- */

    Model::Model(const ModelCreateInfo &createInfo)
    {
        meshes.reserve(createInfo.meshCount);
        materials.reserve(createInfo.meshCount);
        LoadNode(createInfo, rootNode, createInfo.rootNode);
    }

    SharedPtr<Model> Model::Create(const ModelCreateInfo &createInfo)
    {
        return std::make_shared<Model>(createInfo);
    }

    /* --- POLLING METHODS --- */

    Entity Model::Spawn(const Optional<Vector3> position)
    {
        Entity rootEntity = SpawnNode(rootNode);
        rootEntity.GetComponent<Transform>().SetPosition(position.has_value() ? position.value() : Raycast::GetHoveredWorldPosition());
        return rootEntity;
    }

    /* --- PRIVATE METHODS --- */

    void Model::LoadNode(const ModelCreateInfo &createInfo, ModelNode &currentNode, const ModelCreateInfoNode &createInfoNode)
    {
        currentNode.name = std::move(createInfoNode.name);
        if (createInfoNode.meshInfo.has_value())
        {
            // Get mesh data
            auto vertices = std::vector<Vertex>(createInfo.allVertices.begin() + createInfoNode.meshInfo->vertexOffset, createInfo.allVertices.begin() + createInfoNode.meshInfo->vertexOffset + createInfoNode.meshInfo->vertexCount);
            auto indices = std::vector<uint32>(createInfo.allIndices.begin() + createInfoNode.meshInfo->indexOffset, createInfo.allIndices.begin() + createInfoNode.meshInfo->indexOffset + createInfoNode.meshInfo->indexCount);

            // Create mesh
            currentNode.meshIndex = meshes.size();
            meshes.push_back(Mesh::Create({
                .vertices = vertices,
                .indices = indices,
            }));
            materials.push_back(std::move(createInfoNode.meshInfo->material));
        }

        // Recursively load all children nodes
        currentNode.children.resize(createInfoNode.children.size());
        for (uint32 i = 0; i < createInfoNode.children.size(); i++)
        {
            LoadNode(createInfo, currentNode.children[i], createInfoNode.children[i]);
        }
    }

    Entity Model::SpawnNode(const ModelNode &node, const Entity* parentEntity)
    {
        // Spawn current node and assign a mesh renderer
        Entity nodeEntity = Entity(node.name);
        if (parentEntity != nullptr) nodeEntity.SetParent(*parentEntity);

        if (node.meshIndex != -1)
        {
            nodeEntity.AddComponent<MeshRenderer>(meshes[node.meshIndex], materials[node.meshIndex]);
        }

        // Spawn all children recursively
        for (const auto &childNode : node.children)
        {
            SpawnNode(childNode, &nodeEntity);
        }

        return nodeEntity;
    }

    /* --- DESTRUCTOR --- */

    void Model::Destroy()
    {
        for (const auto &mesh : meshes) mesh->Destroy();
    }

}
