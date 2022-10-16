//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "MeshObject.h"
#include "Stopwatch.h"
#include "File.h"

using Sierra::Core::Rendering::Vulkan::VulkanDebugger;

namespace Sierra::Engine::Classes
{

    /* --- CONSTRUCTORS --- */

    MeshObject::MeshObject(const std::string filePath)
        : modelName(File::GetFileNameFromPath(filePath)), modelLocation(File::RemoveFileNameFromPath(filePath))
    {
        // Load the model file
        Assimp::Importer importer;
        const aiScene *assimpScene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

        #if DEBUG
            Stopwatch stopwatch;
        #endif

        // Recursively load every node's mesh in the scene
        ListDeeperNode(assimpScene->mRootNode, assimpScene);

        #if DEBUG
            VulkanDebugger::DisplayInfo("Total vertices count for the model [" + modelName + "] containing [" + std::to_string(assimpScene->mNumMeshes) + "] mesh(es): " + std::to_string(vertexCount) + ". Time elapsed during model loading: " + std::to_string(stopwatch.GetElapsedMilliseconds()) + "ms");
        #endif

        // Dispose the importer
        importer.FreeScene();
    }

    std::unique_ptr<MeshObject> MeshObject::LoadModel(const std::string filePath)
    {
        return std::make_unique<MeshObject>(filePath);
    }

    /* --- SETTER METHODS --- */

    void MeshObject::ListDeeperNode(aiNode *node, const aiScene *assimpScene)
    {
//        GameObject nodeGameObject = new GameObject(firstTime ? modelName : node.Name);
//        if (firstTime) rootGameObject = nodeGameObject;

//        if (parentObject != null) nodeGameObject.SetParent(parentObject);

        // For each mesh in the node
        for (int i = 0; i < node->mNumMeshes; i++)
        {
            // Cache the assimp mesh
            aiMesh *currentAssimpMesh = assimpScene->mMeshes[node->mMeshes[i]];

            // Create an abstracted mesh object
            auto &mesh = LoadAssimpMesh(currentAssimpMesh);

            // Check if mesh has any materials
            if (currentAssimpMesh->mMaterialIndex >= 0)
            {
                // Get material data and write to mesh
                ApplyAssimpMeshTextures(mesh, assimpScene->mMaterials[currentAssimpMesh->mMaterialIndex]);
            }
        }

        // Load every child node
        for (int j = 0; j < node->mNumChildren; j++)
        {
            ListDeeperNode(node->mChildren[j], assimpScene);
        }
    }

    std::unique_ptr<Mesh>& MeshObject::LoadAssimpMesh(aiMesh *mesh)
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
        return Mesh::RegisterMesh(meshVertices, indices);
    }

    void MeshObject::ApplyAssimpMeshTextures(std::unique_ptr<Mesh> &mesh, aiMaterial *assimpMaterial)
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
            mesh->SetTexture(diffuseTexture);
        }

        // Check if mesh has a specular texture
        for (int i = assimpMaterial->GetTextureCount(aiTextureType_SPECULAR); i--;)
        {
            // Get texture file path
            aiString textureFilePath;
            assimpMaterial->GetTexture(aiTextureType_DIFFUSE, i, &textureFilePath);

            // Create texture
            auto diffuseTexture = Texture::Create({
                .filePath = modelLocation + File::FindInSubdirectories(modelLocation, File::GetFileNameFromPath(textureFilePath.C_Str())),
                .textureType = TEXTURE_TYPE_SPECULAR
            });

            // Apply texture
            mesh->SetTexture(diffuseTexture);
        }

        // Get material properties
        aiGetMaterialFloat(assimpMaterial, AI_MATKEY_SHININESS, &mesh->material.shininess);

        aiColor4D assimpColor;

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_AMBIENT, &assimpColor);
        mesh->material.ambient = { assimpColor.r, assimpColor.g, assimpColor.b };

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &assimpColor);
        mesh->material.diffuse = { assimpColor.r, assimpColor.g, assimpColor.b };

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_SPECULAR, &assimpColor);
        mesh->material.specular = { assimpColor.r, assimpColor.g, assimpColor.b };
    }
}