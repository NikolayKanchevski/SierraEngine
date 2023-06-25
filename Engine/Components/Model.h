    //
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include "MeshRenderer.h"
#include "../Classes/Mesh.h"
#include "../Classes/Entity.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Texture.h"

namespace Sierra::Engine::Classes
{

    /// A wrapper around the Mesh class used for loading 3D objects - collections of meshes.

    // TODO: Convert to component
    class Model
    {
    private:
        struct MeshData
        {
            SharedPtr<Mesh> mesh;
            SharedPtr<Texture>* textures;
            Material material {};
        };

        struct EntityData
        {
            String tag;
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
        Model() = default;
        static UniquePtr<Model> Load(const String &filePath);

        /* --- SETTER METHODS --- */
        void Dispose();
        static void DisposePool();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline String GetName() const { return modelName; }
        [[nodiscard]] inline String GetModelLocation() const { return modelLocation; }
        [[nodiscard]] inline MeshRenderer& GetMesh(const uint meshIndex) const { return Entity(meshEntities[meshIndex]).GetComponent<MeshRenderer>(); }
        [[nodiscard]] inline uint GetMeshCount() const { return meshCount; }
        [[nodiscard]] inline Entity GetOriginEntity() const { return Entity(originEntity); }
        [[nodiscard]] inline bool IsLoaded() const { return loaded; };

        /* --- DESTRUCTOR --- */
        DELETE_COPY(Model);

    private:
        bool loaded = false;
        ModelData *modelData = nullptr;

        uint vertexCount = 0;
        uint meshCount = 0;

        String modelName;
        String modelLocation;

        entt::entity originEntity = entt::null;
        std::vector<entt::entity> meshEntities;

        static void LoadInternal(Model *model, const String &filePath);
        SharedPtr<Mesh> LoadAssimpMesh(aiMesh* mesh);
        void ListDeeperNode(aiNode* node, const aiScene* assimpScene, Entity* parentEntity);
        void ApplyAssimpMeshTextures(MeshRenderer &meshComponent, aiMaterial *assimpMaterial);
        static inline std::unordered_map<String, ModelData> modelPool;
    };

}