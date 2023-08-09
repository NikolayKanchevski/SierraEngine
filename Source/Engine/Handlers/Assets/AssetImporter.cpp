//
// Created by Nikolay Kanchevski on 8.08.23.
//

#include "AssetImporter.h"

#include "../Project.h"
#include "AssetManager.h"
#include "../../Classes/File.h"

namespace Sierra::Engine
{

    #pragma region Asset Importer

        /* --- PROTECTED METHODS --- */

        YAML::Node Engine::AssetImporter::GenerateBaseAssetConfig(const AssetType assetType, const AssetID &ID)
        {
            YAML::Node config;
            config["version"] = ENGINE_VERSION;
            config["time_created"] = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            config["asset_type"] = AssetType_ToString(assetType);
            if (!ID.IsNull()) config["asset_file"] = ID.GetFilePath().filename().c_str();
            return config;
        }

    #pragma endregion

    #pragma region Texture Importer

        /* --- GETTER METHODS --- */

        SharedPtr<Rendering::Texture> TextureImporter::Import(const AssetID &ID, const bool recursionProtection)
        {
            // Retrieve config
            bool loadingSerializedAsset = ID.GetFilePath().extension() == ".texture";
            FilePath configFilePath = loadingSerializedAsset ? Project::GetAssetDirectory() / ID.GetFilePath() : Project::GetAssetDirectory() / (ID.GetFilePath().string() + ".texture");
            FilePath dataFilePath = FilePath(configFilePath).replace_extension(".data");
            YAML::Node config = File::FileExists(configFilePath) ? YAML::LoadFile(configFilePath) : GenerateDefaultAssetConfig(ID, configFilePath, dataFilePath);

            // If config provided load data from it, otherwise use default settings
            try
            {
                // Load settings & data
                Rendering::BinaryTextureCreateInfo createInfo{};
                createInfo.width = config["width"].as<uint>();
                createInfo.height = config["height"].as<uint>();
                createInfo.channels = Rendering::ImageChannels_FromString(config["properties"]["channels"].as<String>().c_str());
                createInfo.memoryType = Rendering::ImageMemoryType_FromString(config["properties"]["memory_type"].as<String>().c_str());
                createInfo.enableSmoothFiltering = config["properties"]["smooth_filtering_enabled"].as<bool>();
                createInfo.generateMipMaps = config["properties"]["mip_map_generation_enabled"].as<bool>();
                auto binaryTextureData = File::ReadBinaryFile(dataFilePath);
                createInfo.data = binaryTextureData.data();

                // Load texture
                return Rendering::Texture::Load(createInfo);
            }
            catch (const YAML::BadConversion &exception)
            {
                // Show warning
                ASSERT_WARNING("Importing texture asset [{0}] failed, due to a corrupted {1} file: {2}", ID.GetFilePath().c_str(), configFilePath.extension().c_str(), exception.what());
                if (recursionProtection) return nullptr;

                // Delete corrupted asset config and re-create it
                File::RemoveFile(configFilePath);
                return Import(ID, true);
            }
        }

        /* --- PRIVATE METHODS --- */

        YAML::Node TextureImporter::GenerateDefaultAssetConfig(const AssetID &ID, const FilePath &configFilePath, const FilePath &dataFilePath)
        {
            // Get base config
            YAML::Node config = GenerateBaseAssetConfig(AssetType::TEXTURE, ID);

            // Load image data
            int32 width, height, channelCount;
            auto imagePath = Project::GetAssetDirectory() / ID.GetFilePath().string();
            stbi_info(imagePath.c_str(), &width, &height, &channelCount);
            stbi_uc* stbImage = stbi_load(imagePath.c_str(), &width, &height, &channelCount, channelCount == 3 ? 4 : channelCount);

            // Set default settings
            config["width"] = width;
            config["height"] = height;
            config["properties"]["channels"] = ImageChannels_ToString(static_cast<Rendering::ImageChannels>(channelCount));
            config["properties"]["memory_type"] = ImageMemoryType_ToString(Rendering::ImageMemoryType::UINT8_NORM);
            config["properties"]["smooth_filtering_enabled"] = true;
            config["properties"]["mip_map_generation_enabled"] = false;

            // Write asset data to file
            YAML::Emitter emitter;
            emitter << config;
            File::WriteDataToFile(configFilePath, emitter.c_str(), true, true);

            // Write binary data to file
            File::WriteBinaryDataToFile(dataFilePath, stbImage, width * height * (channelCount == 3 ? 4 : channelCount), true, true);
            stbi_image_free(stbImage);

            return config;
        }

    #pragma endregion

    #pragma region Cubemap Importer

        /* --- GETTER METHODS --- */

        SharedPtr<Rendering::Cubemap> CubemapImporter::Import(const AssetID &ID, const bool recursionProtection)
        {
            // TODO: Implement cubemap importing
            return nullptr;
        }

        /* --- PRIVATE METHODS --- */

        YAML::Node CubemapImporter::GenerateDefaultAssetConfig(const AssetID &ID, const FilePath &configFilePath, const FilePath &dataFilePath)
        {
            return YAML::Node();
        }

    #pragma endregion

    #pragma region Material Importer

        /* --- GETTER METHODS --- */

        SharedPtr<Material> MaterialImporter::Import(const AssetID &ID, bool recursionProtection)
        {
            // Retrieve config
            FilePath configFilePath = Project::GetAssetDirectory() / ID.GetFilePath().string();
            YAML::Node config = File::FileExists(configFilePath) ? YAML::LoadFile(configFilePath) : GenerateDefaultAssetConfig(ID, configFilePath);

            // If config provided load data from it, otherwise use default settings
            try
            {
                // Assign values to material
                auto material = Material::Create();
                material->diffuse = Vector3(config["properties"]["diffuse"]["r"].as<float>(), config["properties"]["diffuse"]["g"].as<float>(), config["properties"]["diffuse"]["b"].as<float>());
                material->specular = config["properties"]["specular"].as<float>();
                material->shininess = config["properties"]["shininess"].as<float>();
                material->vertexExaggeration = config["properties"]["vertex_exaggeration"].as<float>();

                // Assign textures if present, otherwise use default textures
                if (config["properties"]["texture_data"]["has_diffuse_texture"].as<bool>()) material->SetTexture(AssetManager::ImportTexture(ID.GetFilePath().parent_path() / config["properties"]["texture_data"]["diffuse_texture_path"].as<String>()), TextureType::DIFFUSE);
                if (config["properties"]["texture_data"]["has_specular_texture"].as<bool>()) material->SetTexture(AssetManager::ImportTexture(ID.GetFilePath().parent_path() / config["properties"]["texture_data"]["specular_texture_path"].as<String>()), TextureType::SPECULAR);
                if (config["properties"]["texture_data"]["has_normal_texture"].as<bool>()) material->SetTexture(AssetManager::ImportTexture(ID.GetFilePath().parent_path() / config["properties"]["texture_data"]["normal_texture_path"].as<String>()), TextureType::NORMAL);
                if (config["properties"]["texture_data"]["has_height_texture"].as<bool>()) material->SetTexture(AssetManager::ImportTexture(ID.GetFilePath().parent_path() / config["properties"]["texture_data"]["height_texture_path"].as<String>()), TextureType::HEIGHT);

                // Load texture
                return material;
            }
            catch (const YAML::BadConversion &exception)
            {
                // Show warning
                if (!recursionProtection) ASSERT_WARNING("Importing material asset [{0}] failed, due to a corrupted {1} file! Error: {2}", ID.GetFilePath().c_str(), configFilePath.extension().c_str(), exception.what());
                else return nullptr;

                // Delete corrupted asset config and re-create it
                File::RemoveFile(configFilePath);
                return Import(ID, true);
            }
        }

        SharedPtr<Material> MaterialImporter::Import(const FilePath &materialPath, const aiMaterial* assimpMaterial)
        {
            // Create base config
            FilePath materialDirectory = materialPath.parent_path();
            YAML::Node config = GenerateBaseAssetConfig(AssetType::MATERIAL);

            auto material = Material::Create();
            aiColor4D assimpColor;

            // Load values
            aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &assimpColor);
            material->diffuse = { assimpColor.r, assimpColor.g, assimpColor.b };
            config["properties"]["diffuse"]["r"] = assimpColor.r;
            config["properties"]["diffuse"]["g"] = assimpColor.g;
            config["properties"]["diffuse"]["b"] = assimpColor.b;

            aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_SPECULAR, &assimpColor);
            material->specular = assimpColor.r;
            config["properties"]["specular"] = assimpColor.r;

            aiGetMaterialFloat(assimpMaterial, AI_MATKEY_SHININESS, &assimpColor.r);
            material->shininess = assimpColor.r;
            config["properties"]["shininess"] = assimpColor.r;

            material->vertexExaggeration = 0.0f;
            config["properties"]["vertex_exaggeration"] = 0.0f;

            // Load diffuse texture
            if (aiGetMaterialTextureCount(assimpMaterial, aiTextureType_DIFFUSE) > 0)
            {
                // Get texture file path
                aiString textureFilePath;
                assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

                // Try to find texture in subdirectories
                FilePath texturePath = File::FindInSubdirectories(Project::GetAssetDirectory() / materialDirectory, FilePath(textureFilePath.C_Str()).filename());
                if (!texturePath.empty())
                {
                    // Set material's texture
                    material->SetTexture(AssetID(materialDirectory / texturePath), TextureType::DIFFUSE);

                    // Save texture reference
                    config["properties"]["texture_data"]["has_diffuse_texture"] = true;
                    config["properties"]["texture_data"]["diffuse_texture_path"] = texturePath.c_str();
                }
            }
            else
            {
                config["properties"]["texture_data"]["has_diffuse_texture"] = false;
            }

            // Load specular texture
            if (aiGetMaterialTextureCount(assimpMaterial, aiTextureType_SPECULAR) > 0)
            {
                // Get texture file path
                aiString textureFilePath;
                assimpMaterial->GetTexture(aiTextureType_SPECULAR, 0, &textureFilePath);

                // Try to find texture in subdirectories
                FilePath texturePath = File::FindInSubdirectories(Project::GetAssetDirectory() / materialDirectory, FilePath(textureFilePath.C_Str()).filename());
                if (!texturePath.empty())
                {
                    // Set material's texture
                    material->SetTexture(AssetID(materialDirectory / texturePath), TextureType::SPECULAR);

                    // Save texture reference
                    config["properties"]["texture_data"]["has_specular_texture"] = true;
                    config["properties"]["texture_data"]["specular_texture_path"] = texturePath.c_str();
                }
            }
            else
            {
                config["properties"]["texture_data"]["has_specular_texture"] = false;
            }

            // Load normal map
            if (aiGetMaterialTextureCount(assimpMaterial, aiTextureType_NORMALS) > 0)
            {
                // Get texture file path
                aiString textureFilePath;
                assimpMaterial->GetTexture(aiTextureType_NORMALS, 0, &textureFilePath);

                // Try to find texture in subdirectories
                FilePath texturePath = File::FindInSubdirectories(Project::GetAssetDirectory() / materialDirectory, FilePath(textureFilePath.C_Str()).filename());
                if (!texturePath.empty())
                {
                    // Set material's texture
                    material->SetTexture(AssetID(materialDirectory / texturePath), TextureType::NORMAL);

                    // Save texture reference
                    config["properties"]["texture_data"]["has_normal_texture"] = true;
                    config["properties"]["texture_data"]["normal_texture_path"] = texturePath.c_str();
                }
            }
            else
            {
                config["properties"]["texture_data"]["has_normal_texture"] = false;
            }

            // Load height map
            if (aiGetMaterialTextureCount(assimpMaterial, aiTextureType_HEIGHT) > 0)
            {
                // Get texture file path
                aiString textureFilePath;
                assimpMaterial->GetTexture(aiTextureType_HEIGHT, 0, &textureFilePath);

                // Try to find texture in subdirectories
                FilePath texturePath = File::FindInSubdirectories(Project::GetAssetDirectory() / materialDirectory, FilePath(textureFilePath.C_Str()).filename());
                if (!texturePath.empty())
                {
                    // Set material's texture
                    material->SetTexture(AssetID(materialDirectory / texturePath), TextureType::HEIGHT);

                    // Save texture reference
                    config["properties"]["texture_data"]["has_height_texture"] = true;
                    config["properties"]["texture_data"]["height_texture_path"] = texturePath.c_str();
                }
            }
            else
            {
                config["properties"]["texture_data"]["has_height_texture"] = false;
            }

            // Write asset data to file
            YAML::Emitter emitter;
            emitter << config;
            File::WriteDataToFile(Project::GetAssetDirectory() / materialPath, emitter.c_str(), true, true);

            return material;
        }

        /* --- PRIVATE METHODS --- */

        YAML::Node MaterialImporter::GenerateDefaultAssetConfig(const AssetID &ID, const FilePath &configFilePath, const FilePath &dataFilePath)
        {
            // Get base config
            YAML::Node config = GenerateBaseAssetConfig(AssetType::MATERIAL);

            // Set default settings
            config["properties"]["diffuse"]["r"] = 1.0f;
            config["properties"]["diffuse"]["g"] = 1.0f;
            config["properties"]["diffuse"]["b"] = 1.0f;
            config["properties"]["specular"] = 1.0f;
            config["properties"]["shininess"] = 0.001953125f;
            config["properties"]["vertex_exaggeration"] = 0.0f;
            config["properties"]["texture_data"]["has_diffuse_texture"] = false;
            config["properties"]["texture_data"]["has_specular_texture"] = false;
            config["properties"]["texture_data"]["has_normal_texture"] = false;
            config["properties"]["texture_data"]["has_height_texture"] = false;

            // Write asset data to file
            YAML::Emitter emitter;
            emitter << config;
            File::WriteDataToFile(configFilePath, emitter.c_str(), true, true);

            return config;
        }

    #pragma endregion

    #pragma region Model Importer

        /* --- GETTER METHODS --- */

        SharedPtr<Model> ModelImporter::Import(const AssetID &ID, const bool recursionProtection)
        {
            PROFILE_FUNCTION();
            // Retrieve config
            bool loadingSerializedAsset = ID.GetFilePath().extension() == ".model";
            FilePath configFilePath = loadingSerializedAsset ? Project::GetAssetDirectory() / ID.GetFilePath() : Project::GetAssetDirectory() / (ID.GetFilePath().string() + ".model");
            FilePath dataFilePath = FilePath(configFilePath).replace_extension(".data");
            YAML::Node config = File::FileExists(configFilePath) && File::FileExists(dataFilePath) ? YAML::LoadFile(configFilePath) : GenerateDefaultAssetConfig(ID, configFilePath, dataFilePath);

            // Try reading from file
            try
            {
                // Read offsets from config
                uint64 vertexOffset = config["vertex_offset"].as<uint64>();
                uint64 indexOffset = config["index_offset"].as<uint64>();

                // Load binary data and calculate its size in bytes
                auto binaryModelData = File::ReadBinaryFile(dataFilePath);
                uint64 binaryModelDataSize = binaryModelData.size() * UINT8_SIZE;

                // Allocate needed data to store the binary formatted vertex & index data
                std::vector<Vertex> allVertices((indexOffset - vertexOffset) / sizeof(Vertex));
                std::vector<uint32> allIndices((binaryModelDataSize - indexOffset) / sizeof(uint32));

                // Copy memory accordingly to fill vertex & index data
                memcpy(allVertices.data(), reinterpret_cast<char*>(binaryModelData.data()) + vertexOffset, indexOffset - vertexOffset);
                memcpy(allIndices.data(), reinterpret_cast<char*>(binaryModelData.data()) + indexOffset, binaryModelDataSize - indexOffset);

                // Set up import info
                ModelCreateInfo createInfo = { .allVertices = allVertices, .allIndices = allIndices };

                // Fill in create info
                auto iterator = config["structure"].begin();
                ProcessLoadNode(ID, createInfo.rootNode, config["structure"].begin());

                // Load model
                return Model::Create(createInfo);
            }
            catch (const YAML::BadConversion &exception)
            {
                // Show warning
                ASSERT_WARNING("Importing texture asset [{0}] failed, due to a corrupted {1} file: {2}", ID.GetFilePath().c_str(), configFilePath.extension().c_str(), exception.what());
                if (recursionProtection) return nullptr;

                // Delete corrupted asset config and re-create it
                File::RemoveFile(configFilePath);
                return Import(ID, true);
            }
        }

        /* --- PRIVATE METHODS --- */

        void ModelImporter::ProcessMesh(const aiMesh* mesh, BinaryModelData &modelData, YAML::Node &configNode)
        {
            // Store values prior to inserting new data
            uint32 priorVertexCount = modelData.vertexStorage.size();
            uint32 priorIndexCount = modelData.indexStorage.size();

            // Allocate memory for new data
            std::vector<Vertex> vertices(mesh->mNumVertices);
            std::vector<uint32> indices;

            // Load vertex data
            for (uint32 i = 0; i < mesh->mNumVertices; i++)
            {
                vertices[i].position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
                vertices[i].normal = mesh->HasNormals() ? Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : Vector3(0.5, 0.5, 0.5);
                vertices[i].UV = mesh->HasTextureCoords(0) ? Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : Vector2(0, 0);
            }

            // Iterate over indices through faces and copy across
            for (uint32 i = 0; i < mesh->mNumFaces; i++)
            {
                // Get a face
                aiFace face = mesh->mFaces[i];

                // Go through face's indices and add to list
                for (uint32 j = 0; j < face.mNumIndices; j++)
                {
                    indices.push_back(face.mIndices[j]);
                }
            }

            // Unite old and new vertex data
            std::vector<Vertex> newVertexStorage;
            newVertexStorage.reserve(modelData.vertexStorage.size() + vertices.size());
            newVertexStorage.insert(newVertexStorage.end(), modelData.vertexStorage.begin(), modelData.vertexStorage.end());
            newVertexStorage.insert(newVertexStorage.end(), vertices.begin(), vertices.end());
            modelData.vertexStorage = std::move(newVertexStorage);

            // Unite old and new index data
            std::vector<uint32> newIndexStorage;
            newIndexStorage.reserve(modelData.indexStorage.size() + indices.size());
            newIndexStorage.insert(newIndexStorage.end(), modelData.indexStorage.begin(), modelData.indexStorage.end());
            newIndexStorage.insert(newIndexStorage.end(), indices.begin(), indices.end());
            modelData.indexStorage = std::move(newIndexStorage);

            // Serialize data
            configNode["vertex_offset"] = priorVertexCount;
            configNode["vertex_count"] = modelData.vertexStorage.size() - priorVertexCount;
            configNode["index_offset"] = priorIndexCount;
            configNode["index_count"] = modelData.indexStorage.size() - priorIndexCount;
        }

        void ModelImporter::ProcessMaterial(const AssetID &ID, const aiMaterial* assimpMaterial, const String &materialName, YAML::Node &configNode)
        {
            // Import material and save its location in config
            FilePath materialPath = ID.GetFilePath().parent_path() / (materialName + ".material");
            configNode["material_path"] = materialName + ".material";
            AssetManager::GetMaterialCollection().AddResource(AssetID(materialPath), MaterialImporter().Import(materialPath, assimpMaterial));
        }

        void ModelImporter::ProcessNode(const AssetID &ID, const aiNode* node, const aiScene* scene, YAML::Node &configNode, BinaryModelData &modelData)
        {
            // An ugly check create one node if model contains a single mesh only
            if (scene->mNumMeshes == 1 && node->mNumMeshes == 0)
            {
                for (uint32 i = 0; i < node->mNumChildren; i++)
                {
                    ProcessNode(ID, node->mChildren[i], scene, configNode, modelData);
                }
                return;
            }

            // Get name of current node (use asset's file name if listing root node)
            String nodeName = node->mParent == nullptr ? ID.GetFilePath().filename().c_str() : node->mName.C_Str();

            // If node has a single mesh its data is set
            if (node->mNumMeshes == 1)
            {
                // Create new node
                YAML::Node currentNode = configNode[nodeName];

                // Get current mesh
                const aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];

                // Load mesh data
                currentNode["has_mesh"] = true;
                YAML::Node meshNode = currentNode["mesh_data"];
                ProcessMesh(mesh, modelData, meshNode);

                // Serialize materials
                YAML::Node materialNode = currentNode["material_data"];
                ProcessMaterial(ID, scene->mMaterials[mesh->mMaterialIndex], nodeName, materialNode);
            }
            // Otherwise a dedicated node is created for each mesh and has its data set
            else if (node->mNumMeshes > 1)
            {
                configNode[nodeName]["has_mesh"] = false;
                for (uint32 i = 0; i < node->mNumMeshes; i++)
                {
                    // Create new node
                    String currentNodeName = nodeName + '_' + std::to_string(i);
                    YAML::Node currentNode = configNode[nodeName]["children"][currentNodeName];

                    // Get current mesh
                    const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

                    // Load mesh data
                    currentNode["has_mesh"] = true;
                    YAML::Node meshNode = currentNode["mesh_data"];
                    ProcessMesh(mesh, modelData, meshNode);

                    // Serialize materials
                    YAML::Node materialNode = currentNode["material_data"];
                    ProcessMaterial(ID, scene->mMaterials[mesh->mMaterialIndex], currentNodeName, materialNode);
                }
            }
            else
            {
                configNode[nodeName]["has_mesh"] = false;
            }

            // Recursion continuation
            auto nextConfigNode = configNode[nodeName]["children"];
            for (uint32 i = 0; i < node->mNumChildren; i++)
            {
                ProcessNode(ID, node->mChildren[i], scene, nextConfigNode, modelData);
            }
        }

        YAML::Node ModelImporter::GenerateDefaultAssetConfig(const AssetID &ID, const FilePath &configFilePath, const FilePath &dataFilePath)
        {
            // Import file
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(Project::GetAssetDirectory() / ID.GetFilePath(), aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

            // Check for errors
            ASSERT_ERROR_IF(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode, "Importing 3D model asset [{0}] failed: {1}", ID.GetFilePath().c_str(), importer.GetErrorString());

            // Get base config
            YAML::Node config = GenerateBaseAssetConfig(AssetType::MODEL, ID);

            // Set up the following members
            config["mesh_count"] = scene->mNumMeshes;
            auto structureNode = config["structure"];

            // Retrieve mesh count and vertex + index data in binary
            BinaryModelData modelData{};
            ProcessNode(ID, scene->mRootNode, scene, structureNode, modelData);

            // Write binary data to file
            config["vertex_offset"] = static_cast<uint64>(0);
            config["index_offset"] = static_cast<uint64>(modelData.vertexStorage.size() * sizeof(Vertex));
            File::WriteBinaryDataToFile(dataFilePath, reinterpret_cast<uint8*>(modelData.vertexStorage.data()), modelData.vertexStorage.size() * sizeof(Vertex), true, true);
            File::WriteBinaryDataToFile(dataFilePath, reinterpret_cast<uint8*>(modelData.indexStorage.data()), modelData.indexStorage.size() * sizeof(uint32), false);

            // Write asset data to file
            YAML::Emitter emitter;
            emitter << config;
            File::WriteDataToFile(configFilePath, emitter.c_str(), true, true);

            importer.FreeScene();
            return config;
        }

        void ModelImporter::ProcessLoadNode(const AssetID &ID, ModelCreateInfoNode &currentNode, const YAML::detail::iterator_base<YAML::detail::iterator_value> &loadInfoIterator)
        {
            // Save node name and load mesh data if present
            currentNode.name = loadInfoIterator->first.as<String>();
            if (loadInfoIterator->second["has_mesh"].as<bool>())
            {
                currentNode.meshInfo = ModelCreateInfoMeshNode {
                    .vertexOffset = loadInfoIterator->second["mesh_data"]["vertex_offset"].as<uint32>(),
                    .vertexCount  = loadInfoIterator->second["mesh_data"]["vertex_count"].as<uint32>(),
                    .indexOffset  = loadInfoIterator->second["mesh_data"]["index_offset"].as<uint32>(),
                    .indexCount   = loadInfoIterator->second["mesh_data"]["index_count"].as<uint32>(),
                    .material     = AssetManager::GetMaterialCollection().GetResource(AssetManager::ImportMaterial(ID.GetFilePath().parent_path() / loadInfoIterator->second["material_data"]["material_path"].as<String>()))
                };
            }

            // Load child nodes
            uint32 i = 0;
            currentNode.children.resize(loadInfoIterator->second["children"].size());
            for (auto iterator = loadInfoIterator->second["children"].begin(); iterator != loadInfoIterator->second["children"].end(); iterator++)
            {
                ProcessLoadNode(ID, currentNode.children[i], iterator);
                i++;
            }
        }

    #pragma endregion

}