//
// Created by Nikolay Kanchevski on 1.08.23.
//

#pragma once

#include "Mesh.h"
#include "Entity.h"
#include "Material.h"

namespace Sierra::Engine
{

    struct ModelCreateInfoMeshNode
    {
        uint64 vertexOffset;
        uint64 vertexCount;
        uint64 indexOffset;
        uint64 indexCount;
        SharedPtr<Material> material;
    };

    struct ModelCreateInfoNode
    {
        String name;
        Optional<ModelCreateInfoMeshNode> meshInfo;
        std::vector<ModelCreateInfoNode> children;
    };

    struct ModelCreateInfo
    {
        ModelCreateInfoNode rootNode{};

        uint32 meshCount = 0;
        const std::vector<Vertex> &allVertices;
        const std::vector<uint32> &allIndices;
    };

    class Model
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Model(const ModelCreateInfo &createInfo);
        static SharedPtr<Model> Create(const ModelCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        Entity Spawn(Optional<Vector3> position = std::nullopt);

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Model);

    private:
        struct ModelNode
        {
            String name;
            uint32 meshIndex = -1;
            std::vector<ModelNode> children;
        };

        ModelNode rootNode;
        std::vector<UniquePtr<Mesh>> meshes;
        std::vector<SharedPtr<Material>> materials;

        void LoadNode(const ModelCreateInfo &createInfo,  ModelNode &currentNode, const ModelCreateInfoNode &createInfoNode);
        Entity SpawnNode(const ModelNode &node, const Entity* parentEntity = nullptr);

    };

}