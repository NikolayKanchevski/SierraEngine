//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "MeshObject.h"
#include "Stopwatch.h"
#include "File.h"

using Sierra::Core::Debugger;

namespace Sierra::Engine::Classes
{

    /* --- CONSTRUCTORS --- */

    MeshObject::MeshObject(const std::string filePath)
        : modelName(File::GetFileNameFromPath(filePath)), modelLocation(File::RemoveFileNameFromPath(filePath))
    {
        // Load the model file
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            ASSERT_ERROR("Error loading 3D model [" + modelLocation + "]: " + importer.GetErrorString());
            return;
        }

        meshEntities.reserve(scene->mNumMeshes);

        #if DEBUG
            Stopwatch stopwatch;
        #endif

        // Recursively load every node's mesh in the scene
        ListDeeperNode(scene->mRootNode, scene, nullptr);

        #if DEBUG
            ASSERT_INFO("Total vertices count for the model [" + modelName + "] containing [" + std::to_string(scene->mNumMeshes) + "] mesh(es): " + std::to_string(vertexCount) + ". Time elapsed during model loading: " + std::to_string(stopwatch.GetElapsedMilliseconds()) + "ms");
        #endif

        // Dispose the importer
        importer.FreeScene();
    }

    std::unique_ptr<MeshObject> MeshObject::LoadModel(const std::string filePath)
    {
        return std::make_unique<MeshObject>(filePath);
    }

    /* --- SETTER METHODS --- */

    void MeshObject::ListDeeperNode(aiNode *node, const aiScene *assimpScene, Entity* parentEntity)
    {
        Entity nodeEntity = Entity(parentEntity == nullptr ? modelName : node->mName.C_Str());

        if (parentEntity != nullptr) nodeEntity.SetParent(*parentEntity);

        // For each mesh in the node
        if (node->mNumMeshes == 1)
        {
            // Cache the assimp mesh
            aiMesh *currentAssimpMesh = assimpScene->mMeshes[node->mMeshes[0]];

            // Create an abstracted mesh object
            Mesh &meshComponent = nodeEntity.AddComponent<Mesh>(LoadAssimpMesh(currentAssimpMesh));
            meshEntities.push_back(nodeEntity.GetEnttEntity());

            // Check if mesh has any materials
            if (currentAssimpMesh->mMaterialIndex >= 0)
            {
                // Get material data and write to mesh
                ApplyAssimpMeshTextures(meshComponent, assimpScene->mMaterials[currentAssimpMesh->mMaterialIndex]);
            }
        }
        else
        {
            for (uint32_t i = 0; i < node->mNumMeshes; i++)
            {
                // Create a new entity for each submesh as an entity cannot have more than 1 of the same components
                Entity submeshEntity = Entity(nodeEntity.GetTag() + "_" + std::to_string(i), nodeEntity);

                // Cache the assimp mesh
                aiMesh *currentAssimpMesh = assimpScene->mMeshes[node->mMeshes[i]];

                // Create an abstracted mesh object
                Mesh &meshComponent = submeshEntity.AddComponent<Mesh>(LoadAssimpMesh(currentAssimpMesh));
                meshEntities.push_back(submeshEntity.GetEnttEntity());

                // Check if mesh has any materials
                if (currentAssimpMesh->mMaterialIndex >= 0)
                {
                    // Get material data and write to mesh
                    ApplyAssimpMeshTextures(meshComponent, assimpScene->mMaterials[currentAssimpMesh->mMaterialIndex]);
                }
            }
        }

        // Load every child node
        for (int j = 0; j < node->mNumChildren; j++)
        {
            ListDeeperNode(node->mChildren[j], assimpScene, &nodeEntity);
        }
    }

    Mesh MeshObject::LoadAssimpMesh(aiMesh *mesh)
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

        // Create and return the mesh
        return Mesh(meshVertices, indices);
    }

    void MeshObject::ApplyAssimpMeshTextures(Mesh &mesh, aiMaterial *assimpMaterial)
    {
        // Check if mesh has a diffuse texture
        for (int i = assimpMaterial->GetTextureCount(aiTextureType_DIFFUSE); i--;)
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
            mesh.SetTexture(diffuseTexture);

            mesh.ResetTexture(TEXTURE_TYPE_DIFFUSE);
        }

        // Check if mesh has a specular texture
        for (int i = assimpMaterial->GetTextureCount(aiTextureType_SPECULAR); i--;)
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
            mesh.SetTexture(specularTexture);
        }

        // Get material properties
        aiGetMaterialFloat(assimpMaterial, AI_MATKEY_SHININESS, &mesh.material.shininess);

        aiColor4D assimpColor;

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_AMBIENT, &assimpColor);
        mesh.material.ambient = { assimpColor.r, assimpColor.g, assimpColor.b };

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &assimpColor);
        mesh.material.diffuse = { assimpColor.r, assimpColor.g, assimpColor.b };

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_SPECULAR, &assimpColor);
        mesh.material.specular = { assimpColor.r, assimpColor.g, assimpColor.b };
    }
}