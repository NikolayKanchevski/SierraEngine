//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Model.h"

#include "../Classes/File.h"

using Sierra::Core::Debugger;

namespace Sierra::Engine::Classes
{

    /* --- CONSTRUCTORS --- */

    UniquePtr<Model> Model::Load(const String filePath)
    {
//        // NOTE: Logic is working but will wait until thread pool has been implemented
//        static std::vector<std::future<void>> asyncFutures;
//
//        UniquePtr<Model> model = std::make_unique<Model>();
//        asyncFutures.push_back(std::async(std::launch::async, LoadInternal, model.get(), filePath));
//        return model;

        UniquePtr<Model> model = std::make_unique<Model>();
        LoadInternal(model.get(), filePath);
        return model;
    }

    void Model::LoadInternal(Model *model, const String filePath)
    {
        model->modelName = File::GetFileNameFromPath(filePath);
        model->modelLocation = File::RemoveFileNameFromPath(filePath);

        if (modelPool.count(filePath) != 0)
        {
            PROFILE_FUNCTION();

            ModelData &loadedModelData = modelPool[filePath];

            // Store pointers to all loaded entities to be able to parent them
            std::vector<Entity> entities;
            entities.reserve(loadedModelData.entities.size());

            model->meshEntities.clear();
            model->meshEntities.reserve(loadedModelData.entities.size());

            // For each entity tag
            for (const auto &entityData : loadedModelData.entities)
            {
                // Create entity
                Entity entity(entityData.tag);
                if (model->originEntity == entt::null) model->originEntity = entity.GetEnttEntity();

                // Check if it has mesh
                if (entityData.correspondingMeshID != -1)
                {
                    // Add mesh component
                    auto &meshData = loadedModelData.meshes[entityData.correspondingMeshID];

                    // Add mesh component
                    auto &mesh = entity.AddComponent<MeshRenderer>(meshData.mesh);
                    mesh.material = meshData.material;

                    // Apply textures
                    for (uint i = TOTAL_TEXTURE_TYPES_COUNT; i--;)
                    {
                        if (meshData.textures[i] == nullptr) continue;
                        mesh.SetTexture(meshData.textures[i]);
                    }

                    // Increase vertex count
                    model->vertexCount += mesh.GetMesh()->GetVertexCount();
                    model->meshCount++;

                    Mesh::IncreaseTotalMeshCount();

                    model->meshEntities.push_back(entity.GetEnttEntity());
                }

                // Check if entity has a parent
                if (entityData.parentEntityID != -1)
                {
                    // Set its parent
                    entity.SetParent(entities[entityData.parentEntityID]);
                }

                // Store pointer to the newly created entity
                entities.push_back(entity);

                Mesh::IncreaseTotalVertexCount(model->vertexCount);
            }

            model->loaded = true;

            #if DEBUG
                ASSERT_INFO_FORMATTED("Total vertices count for the model [{0}] containing [{1}] mesh(es): {2}", model->modelName, loadedModelData.meshes.size(), model->vertexCount);
            #endif
        }
        else
        {
            PROFILE_FUNCTION();

            model->modelData = new ModelData();

            // Load the model file
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                ASSERT_ERROR("Error loading 3D model [" + model->modelLocation + "]: " + importer.GetErrorString());
            }

            model->meshEntities.reserve(scene->mNumMeshes);

            // Recursively load every node's mesh in the scene
            model->ListDeeperNode(scene->mRootNode, scene, nullptr);
            model->loaded = true;

            modelPool[filePath] = *model->modelData;
            delete model->modelData;

            #if DEBUG
                ASSERT_INFO_FORMATTED("Total vertices count for the model [{0}] containing [{1}] mesh(es): {2}", model->modelName, scene->mNumMeshes, model->vertexCount);
            #endif

            // Dispose the importer
            importer.FreeScene();
        }
    }

    /* --- SETTER METHODS --- */

    void Model::Dispose()
    {
        // Remove model from pool
        modelPool.erase(modelLocation + modelName);

        // For each mesh delete their textures
        for (const auto &meshEntity : meshEntities)
        {
            const MeshRenderer &meshRenderer = World::GetComponent<MeshRenderer>(meshEntity);

            for (uint i = TOTAL_TEXTURE_TYPES_COUNT; i--;)
            {
                auto texture = meshRenderer.GetTexture((TextureType) i);
                if (texture != nullptr) texture->Dispose();
            }
        }
    }

    void Model::DisposePool()
    {
        for (const auto &model : modelPool)
        {
            for (const auto &mesh : model.second.meshes)
            {
                mesh.mesh->Destroy();
            }
        }

        modelPool.clear();
    }

    void Model::ListDeeperNode(aiNode *node, const aiScene *assimpScene, Entity* parentEntity)
    {
        Entity nodeEntity = Entity(parentEntity == nullptr ? modelName : node->mName.C_Str());
        if (!parentEntity) this->originEntity = nodeEntity.GetEnttEntity();

        // Find index of parent using a reversed for loop
        int parentID = -1;
        if (parentEntity != nullptr)
        {
            nodeEntity.SetParent(*parentEntity);

            for (uint i = modelData->entities.size(); i--;)
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
            for (uint i = 0; i < node->mNumMeshes; i++)
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

    SharedPtr<Mesh> Model::LoadAssimpMesh(aiMesh *mesh)
    {
        std::vector<uint> indices;
        std::vector<Vertex> vertices(mesh->mNumVertices);

        // Load vertex data
        for (uint i = 0; i < mesh->mNumVertices; i++)
        {
            vertices[i].position = {mesh->mVertices[i].x, -mesh->mVertices[i].y, mesh->mVertices[i].z };
            vertices[i].normal = mesh->HasNormals() ? Vector3(mesh->mNormals[i].x, -mesh->mNormals[i].y, mesh->mNormals[i].z) : Vector3(0, 0, 0);
            vertices[i].UV = mesh->HasTextureCoords(0) ? Vector2(mesh->mTextureCoords[0][i].x, -mesh->mTextureCoords[0][i].y) : Vector2(0, 0);
        }

        // Iterate over indices through faces and copy across
        for (uint i = 0; i < mesh->mNumFaces; i++)
        {
            // Get a face
            aiFace face = mesh->mFaces[i];

            // Go through face's indices and add to list
            for (uint j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Increase vertex count
        this->vertexCount += mesh->mNumVertices;
        this->meshCount++;

        // Create and return the mesh
        return Mesh::Create({ .vertices = vertices, .indices = indices });
    }

    void Model::ApplyAssimpMeshTextures(MeshRenderer &meshComponent, aiMaterial *assimpMaterial)
    {
        using Sierra::Core::Rendering::Vulkan::ImageFormat;

        // Check if mesh has a diffuse texture
        for (uint i = assimpMaterial->GetTextureCount(aiTextureType_DIFFUSE); i--;)
        {
            // Get texture file path
            aiString textureFilePath;
            assimpMaterial->GetTexture(aiTextureType_DIFFUSE, i, &textureFilePath);

            // Create texture
            auto diffuseTexture = Texture::Create({
                .filePath = modelLocation + File::FindInSubdirectories(modelLocation, File::GetFileNameFromPath(textureFilePath.C_Str())),
                .textureType = TextureType::DIFFUSE,
                .imageFormat = ImageFormat::R8G8B8A8_SRGB,
                .mipMappingEnabled = true
            });

            // Apply texture
            meshComponent.SetTexture(diffuseTexture);
        }

        // Check if mesh has a specular texture
        for (uint i = assimpMaterial->GetTextureCount(aiTextureType_SPECULAR); i--;)
        {
            // Get texture file path
            aiString textureFilePath;
            assimpMaterial->GetTexture(aiTextureType_SPECULAR, i, &textureFilePath);

            // Create texture
            auto specularTexture = Texture::Create({
                .filePath = modelLocation + File::FindInSubdirectories(modelLocation, File::GetFileNameFromPath(textureFilePath.C_Str())),
                .textureType = TextureType::SPECULAR,
                .imageFormat = ImageFormat::R8_UNORM,
                .mipMappingEnabled = true
            });

            // Apply texture
            meshComponent.SetTexture(specularTexture);
        }

        // Check if mesh has a normal texture
        for (uint i = assimpMaterial->GetTextureCount(aiTextureType_NORMALS); i--;)
        {
            // Get texture file path
            aiString textureFilePath;
            assimpMaterial->GetTexture(aiTextureType_NORMALS, i, &textureFilePath);

            // Create texture
            auto normalTexture = Texture::Create({
                .filePath = modelLocation + File::FindInSubdirectories(modelLocation, File::GetFileNameFromPath(textureFilePath.C_Str())),
                .textureType = TextureType::NORMAL_MAP,
                .imageFormat = ImageFormat::R8G8B8A8_UNORM,
                .mipMappingEnabled = true
            });

            // Apply texture
            meshComponent.SetTexture(normalTexture);
        }

        // Check if mesh has a height map texture
        for (uint i = assimpMaterial->GetTextureCount(aiTextureType_HEIGHT); i--;)
        {
            // Get texture file path
            aiString textureFilePath;
            assimpMaterial->GetTexture(aiTextureType_HEIGHT, i, &textureFilePath);

            // Create texture
            auto heightMapTexture = Texture::Create({
                .filePath = modelLocation + File::FindInSubdirectories(modelLocation, File::GetFileNameFromPath(textureFilePath.C_Str())),
                .textureType = TextureType::HEIGHT_MAP,
                .imageFormat = ImageFormat::R8_UNORM
            });

            // Apply texture
            meshComponent.SetTexture(heightMapTexture);
        }

        // Get material properties
        aiGetMaterialFloat(assimpMaterial, AI_MATKEY_SHININESS, &meshComponent.material.shininess);
        meshComponent.material.shininess *= 3.0f;

        aiColor4D assimpColor;

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &assimpColor);
        meshComponent.material.diffuse = { assimpColor.r, assimpColor.g, assimpColor.b };

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_SPECULAR, &assimpColor);
        meshComponent.material.specular = assimpColor.r;
    }
}