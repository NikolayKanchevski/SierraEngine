//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Classes/Mesh.h"
#include "../Classes/Entity.h"
#include "MeshRenderer.h"

using namespace Sierra::Engine::Components;

namespace Sierra::Engine::Classes
{

    /// A wrapper around the Mesh class used for loading 3D objects - collections of meshes.
    class Model
    {
    private:
        struct MeshData
        {
            std::shared_ptr<Mesh> mesh;
            std::shared_ptr<Texture>* textures;
            Material material {};
        };

        struct EntityData
        {
            std::string tag;
            int selfID = -1;
            int parentEntityID = -1;
            int correspondingMeshID = -1;
        };

        struct ModelData
        {
            std::vector<MeshData> meshes;
            std::vector<EntityData> entities;
        };

    public:
        /* --- CONSTRUCTORS --- */
        Model(const std::string filePath);
        static std::unique_ptr<Model> Load(const std::string &filePath);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32_t GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline std::string GetName() const { return modelName; }
        [[nodiscard]] inline std::string GetModelLocation() const { return modelLocation; }
        [[nodiscard]] inline MeshRenderer& GetMesh(const uint32_t meshIndex) const { return Entity(meshEntities[meshIndex]).GetComponent<MeshRenderer>(); }
        [[nodiscard]] inline uint32_t GetMeshCount() const { return meshCount; }

        /* --- DESTRUCTOR --- */
        Model(const Model &) = delete;
        Model &operator=(const Model &) = delete;

    private:
        ModelData *modelData;

        uint32_t vertexCount = 0;
        uint32_t meshCount = 0;

        std::string modelName;
        std::string modelLocation;
        std::vector<entt::entity> meshEntities;

        std::shared_ptr<Mesh> LoadAssimpMesh(aiMesh* mesh);
        void ListDeeperNode(aiNode* node, const aiScene* assimpScene, Entity* parentEntity);
        void ApplyAssimpMeshTextures(MeshRenderer &meshComponent, aiMaterial *assimpMaterial);

        static inline std::unordered_map<std::string, ModelData> modelPool;
    };

}