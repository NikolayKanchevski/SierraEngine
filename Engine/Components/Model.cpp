//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "../Classes/File.h"
#include "../Classes/Stopwatch.h"

using Sierra::Core::Debugger;

namespace Sierra::Engine::Classes
{

    /* --- CONSTRUCTORS --- */

    Model::Model(const std::string &filePath)
        : modelName(File::GetFileNameFromPath(filePath)), modelLocation(File::RemoveFileNameFromPath(filePath))
    {
        if (modelPool.count(filePath) != 0)
        {
            PROFILE_FUNCTION();

            ModelData &loadedModelData = modelPool[filePath];

            // Store pointers to all loaded entities to be able to parent them
            std::vector<Entity*> entities;
            entities.reserve(loadedModelData.entities.size());

            meshEntities.clear();
            meshEntities.reserve(loadedModelData.entities.size());

            // For each entity tag
            for (const auto &entityData : loadedModelData.entities)
            {
                // Create entity
                Entity entity(entityData.tag);

                // Check if it has mesh
                if (entityData.correspondingMeshID != -1)
                {
                    // Add mesh component
                    auto &meshData = loadedModelData.meshes[entityData.correspondingMeshID];

                    // Add mesh component
                    auto &mesh = entity.AddComponent<MeshRenderer>(meshData.mesh);
                    mesh.material = meshData.material;

                    // Apply textures
                    for (uint32_t i = TOTAL_TEXTURE_TYPES_COUNT; i--;)
                    {
                        if (meshData.textures[i] == nullptr) continue;
                        mesh.SetTexture(meshData.textures[i]);
                    }

                    // Increase vertex count
                    vertexCount += mesh.GetMesh()->GetVertexCount();
                    meshCount++;

                    Mesh::IncreaseTotalMeshCount();

                    meshEntities.push_back(entity.GetEnttEntity());
                }

                // Check if entity has a parent
                if (entityData.parentEntityID != -1)
                {
                    // Set its parent
                    entity.SetParent(*entities[entityData.parentEntityID]);
                }

                // Store pointer to the newly created entity
                entities.push_back(&entity);

                Mesh::IncreaseTotalVertexCount(vertexCount);
            }

            #if DEBUG
                ASSERT_INFO("Total vertices count for the model [" + modelName + "] containing [" + std::to_string(loadedModelData.meshes.size()) + "] mesh(es): " + std::to_string(vertexCount));
            #endif
        }
        else
        {
            PROFILE_FUNCTION();

            modelData = new ModelData();

            // Load the model file
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                ASSERT_ERROR("Error loading 3D model [" + modelLocation + "]: " + importer.GetErrorString());
                return;
            }

            meshEntities.reserve(scene->mNumMeshes);

            // Recursively load every node's mesh in the scene
            ListDeeperNode(scene->mRootNode, scene, nullptr);

            modelPool[filePath] = *modelData;
            delete modelData;

            #if DEBUG
                ASSERT_INFO("Total vertices count for the model [" + modelName + "] containing [" + std::to_string(scene->mNumMeshes) + "] mesh(es): " + std::to_string(vertexCount));
            #endif

            // Dispose the importer
            importer.FreeScene();
        }
    }

    std::unique_ptr<Model> Model::Load(const std::string filePath)
    {
        return std::make_unique<Model>(filePath);
    }

    /* --- SETTER METHODS --- */

    void Model::Dispose()
    {
        // Remove model from pool
        modelPool.erase(modelLocation + modelName);

        // For each mesh delete their textures
        for (const auto &meshEntity : meshEntities)
        {
            const MeshRenderer &meshRenderer = World::GetEnttRegistry().get<MeshRenderer>(meshEntity);

            for (uint32_t i = TOTAL_TEXTURE_TYPES_COUNT; i--;)
            {
                auto texture = meshRenderer.GetTexture((TextureType) i);
                if (texture != nullptr) texture->Dispose();
            }
        }
    }

    void Model::DisposePool()
    {
        modelPool.clear();
    }

    void Model::ListDeeperNode(aiNode *node, const aiScene *assimpScene, Entity* parentEntity)
    {
        Entity nodeEntity = Entity(parentEntity == nullptr ? modelName : node->mName.C_Str());

        // Find index of parent using a reversed for loop
        int parentID = -1;
        if (parentEntity != nullptr)
        {
            nodeEntity.SetParent(*parentEntity);

            for (uint32_t i = modelData->entities.size(); i--;)
            {
                if (modelData->entities[i].selfID == static_cast<int>(parentEntity->GetEnttEntity()))
                {
                    parentID = i;
                    break;
                }
            }
        }

        // Add entity data to model data
        modelData->entities.push_back({ nodeEntity.GetTag(), static_cast<int>(nodeEntity.GetEnttEntity()), parentID });

        // For each mesh in the node
        if (node->mNumMeshes == 1)
        {
            // Cache the assimp mesh
            aiMesh *currentAssimpMesh = assimpScene->mMeshes[node->mMeshes[0]];

            // Change mesh ID to be valid
            modelData->entities.back().correspondingMeshID = static_cast<int>(modelData->meshes.size());

            // Load and save mesh
            auto mesh = LoadAssimpMesh(currentAssimpMesh);

            // Add mesh component
            auto &meshComponent = nodeEntity.AddComponent<MeshRenderer>(mesh);

            // Check if mesh has any materials
            if (currentAssimpMesh->mMaterialIndex >= 0)
            {
                // Get material data and write to mesh
                ApplyAssimpMeshTextures(meshComponent, assimpScene->mMaterials[currentAssimpMesh->mMaterialIndex]);
            }

            // Add mesh & entity data
            modelData->meshes.push_back({ mesh, meshComponent.GetTextures(), meshComponent.material });
            meshEntities.push_back(nodeEntity.GetEnttEntity());
        }
        else
        {
            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                // Create a new entity for each submesh as an entity cannot have more than 1 of the same components
                Entity submeshEntity = Entity(nodeEntity.GetTag() + "_" + std::to_string(i), nodeEntity);
                modelData->entities.push_back({ submeshEntity.GetTag(), static_cast<int>(submeshEntity.GetEnttEntity()), static_cast<int>(parentID + 1), static_cast<int>(modelData->meshes.size()) });

                // Cache the assimp mesh
                aiMesh *currentAssimpMesh = assimpScene->mMeshes[node->mMeshes[i]];

                // Change mesh ID to be valid
                modelData->entities.back().correspondingMeshID = static_cast<int>(modelData->meshes.size());

                // Create an abstracted mesh object
                auto mesh = LoadAssimpMesh(currentAssimpMesh);

                // Add mesh component
                auto &meshComponent = submeshEntity.AddComponent<MeshRenderer>(mesh);

                // Check if mesh has any materials
                if (currentAssimpMesh->mMaterialIndex >= 0)
                {
                    // Get material data and write to mesh
                    ApplyAssimpMeshTextures(meshComponent, assimpScene->mMaterials[currentAssimpMesh->mMaterialIndex]);
                }

                // Add mesh & entity data
                modelData->meshes.push_back({ mesh, meshComponent.GetTextures(), meshComponent.material });
                meshEntities.push_back(submeshEntity.GetEnttEntity());
            }
        }

        // Load every child node
        for (int j = 0; j < node->mNumChildren; j++)
        {
            ListDeeperNode(node->mChildren[j], assimpScene, &nodeEntity);
        }
    }

    std::shared_ptr<Mesh> Model::LoadAssimpMesh(aiMesh *mesh)
    {
        std::vector<Vertex> meshVertices(mesh->mNumVertices);
        std::vector<uint32_t> indices;

        // Load vertex data
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            meshVertices[i].position = {mesh->mVertices[i].x, -mesh->mVertices[i].y, mesh->mVertices[i].z };
            meshVertices[i].normal = mesh->HasNormals() ? glm::vec3{mesh->mNormals[i].x, -mesh->mNormals[i].y, mesh->mNormals[i].z } : glm::vec3{0, 0, 0 };
            meshVertices[i].textureCoordinates = mesh->HasTextureCoords(0) ? glm::vec2{mesh->mTextureCoords[0][i].x, -mesh->mTextureCoords[0][i].y } : glm::vec2{0, 0 };
        }

        // Iterate over indices through faces and copy across
        for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            // Get a face
            aiFace face = mesh->mFaces[i];

            // Go through face's indices and add to list
            for (uint32_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Increase vertex count
        this->vertexCount += mesh->mNumVertices;
        this->meshCount++;

        // Create and return the mesh
        return std::make_shared<Mesh>(meshVertices, indices);
    }

    void Model::ApplyAssimpMeshTextures(MeshRenderer &meshComponent, aiMaterial *assimpMaterial)
    {
        // Check if mesh has a diffuse texture
        for (uint32_t i = assimpMaterial->GetTextureCount(aiTextureType_DIFFUSE); i--;)
        {
            // Get texture file path
            aiString textureFilePath;
            assimpMaterial->GetTexture(aiTextureType_DIFFUSE, i, &textureFilePath);

            // Create texture
            auto diffuseTexture = Texture::Create({
                .filePath = modelLocation + File::FindInSubdirectories(modelLocation, File::GetFileNameFromPath(textureFilePath.C_Str())),
                .textureType = TEXTURE_TYPE_DIFFUSE
            });

            // Apply texture
            meshComponent.SetTexture(diffuseTexture);
        }

        // Check if mesh has a specular texture
        for (uint32_t i = assimpMaterial->GetTextureCount(aiTextureType_SPECULAR); i--;)
        {
            // Get texture file path
            aiString textureFilePath;
            assimpMaterial->GetTexture(aiTextureType_SPECULAR, i, &textureFilePath);

            // Create texture
            auto specularTexture = Texture::Create({
                .filePath = modelLocation + File::FindInSubdirectories(modelLocation, File::GetFileNameFromPath(textureFilePath.C_Str())),
                .textureType = TEXTURE_TYPE_SPECULAR
            });

            // Apply texture
            meshComponent.SetTexture(specularTexture);
        }

        // Get material properties
        aiGetMaterialFloat(assimpMaterial, AI_MATKEY_SHININESS, &meshComponent.material.shininess);
        meshComponent.material.shininess /= 512.0f;

        aiColor4D assimpColor;

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_AMBIENT, &assimpColor);
        meshComponent.material.ambient = { assimpColor.r, assimpColor.g, assimpColor.b };

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &assimpColor);
        meshComponent.material.diffuse = { assimpColor.r, assimpColor.g, assimpColor.b };

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_SPECULAR, &assimpColor);
        meshComponent.material.specular = { assimpColor.r, assimpColor.g, assimpColor.b };

    }
}