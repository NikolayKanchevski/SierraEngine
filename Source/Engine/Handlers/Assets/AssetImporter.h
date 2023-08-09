//
// Created by Nikolay Kanchevski on 8.08.23.
//

#pragma once

#include "../../Classes/Asset.h"
#include "../../Classes/Model.h"
#include "../../../Core/Rendering/Abstractions/Texture.h"
#include "../../Classes/Material.h"
#include "../../../Core/Rendering/Abstractions/Cubemap.h"

namespace Sierra::Engine
{

    class AssetImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        AssetImporter() = default;

    protected:
        YAML::Node GenerateBaseAssetConfig(const AssetType assetType, const AssetID &ID = AssetID::Null);
        virtual inline YAML::Node GenerateDefaultAssetConfig(const AssetID &ID, const FilePath &configFilePath, const FilePath &dataFilePath) { return YAML::Node(); };

    };

    class TextureImporter : public AssetImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        TextureImporter() = default;

        /* --- GETTER METHODS --- */
        SharedPtr<Rendering::Texture> Import(const AssetID &ID, bool recursionProtection = false);

    private:
        YAML::Node GenerateDefaultAssetConfig(const AssetID &ID, const FilePath &configFilePath, const FilePath &dataFilePath = "") override;

    };

    class CubemapImporter : public AssetImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        CubemapImporter() = default;

        /* --- GETTER METHODS --- */
        SharedPtr<Rendering::Cubemap> Import(const AssetID &ID, bool recursionProtection = false);

    private:
        YAML::Node GenerateDefaultAssetConfig(const AssetID &ID, const FilePath &configFilePath, const FilePath &dataFilePath = "") override;

    };

    class MaterialImporter : public AssetImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        MaterialImporter() = default;

        /* --- GETTER METHODS --- */
        SharedPtr<Material> Import(const AssetID &ID, bool recursionProtection = false);
        SharedPtr<Material> Import(const FilePath &materialPath, const aiMaterial* assimpMaterial);

    private:
        YAML::Node GenerateDefaultAssetConfig(const AssetID &ID, const FilePath &configFilePath, const FilePath &dataFilePath = "") override;

    };

    class ModelImporter : public AssetImporter
    {
    public:
        /* --- CONSTRUCTORS --- */
        ModelImporter() = default;

        /* --- GETTER METHODS --- */
        SharedPtr<Model> Import(const AssetID &ID, bool recursionProtection = false);

    private:
        YAML::Node GenerateDefaultAssetConfig(const AssetID &ID, const FilePath &configFilePath, const FilePath &dataFilePath) override;

        struct BinaryModelData
        {
            // Vectors to store all meshes' data (it will be written in binary to file for meshes to access it through offsets)
            std::vector<Vertex> vertexStorage;
            std::vector<uint32> indexStorage;
        };

        void ProcessNode(const AssetID &ID, const aiNode* node, const aiScene* scene, YAML::Node &configNode, BinaryModelData &modelData);
        void ProcessMesh(const aiMesh* mesh, BinaryModelData &modelData, YAML::Node &configNode);
        void ProcessMaterial(const AssetID &ID, const aiMaterial* assimpMaterial, const String &materialName, YAML::Node &configNode);
        void ProcessLoadNode(const AssetID &ID, ModelCreateInfoNode &currentNode, const YAML::detail::iterator_base<YAML::detail::iterator_value> &loadInfoIterator);

    };

}
