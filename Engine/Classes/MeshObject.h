//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include "../Components/Mesh.h"
#include "Entity.h"

#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace Sierra::Engine::Components;

namespace Sierra::Engine::Classes
{

    /// A wrapper around the Mesh class used for loading 3D objects - collections of meshes.
    class MeshObject
    {
    public:
        /* --- CONSTRUCTORS --- */
        MeshObject(const std::string filePath);
        static std::unique_ptr<MeshObject> LoadModel(const std::string filePath);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32_t GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline std::string GetName() const { return modelName; }
        [[nodiscard]] inline std::string GetModelLocation() const { return modelLocation; }
        [[nodiscard]] inline Mesh& GetMesh(const uint32_t meshIndex) const { return World::GetEnttRegistry().get<Mesh>(meshEntities[meshIndex]); }
        [[nodiscard]] inline uint32_t GetMeshCount() const { return meshCount; }

        /* --- DESTRUCTOR --- */
        MeshObject(const MeshObject &) = delete;
        MeshObject &operator=(const MeshObject &) = delete;
    private:
        uint32_t vertexCount = 0;
        uint32_t meshCount = 0;

        std::string modelName;
        std::string modelLocation;
        std::vector<entt::entity> meshEntities;

        void ListDeeperNode(aiNode* node, const aiScene* assimpScene, Entity* parentEntity);
        Mesh LoadAssimpMesh(aiMesh* mesh);
        void ApplyAssimpMeshTextures(Mesh &meshComponent, aiMaterial *assimpMaterial);
    };

}