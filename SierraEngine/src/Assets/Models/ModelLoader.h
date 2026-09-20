//
// Created by Nikolay Kanchevski on 8.11.24.
//

#pragma once

#include "Vertex.h"
#include "../Textures/ImageLoader.h"
#include "../Materials/MaterialSerializer.h"

namespace SierraEngine
{

    struct ModelLoadInfo
    {
        std::span<const uint8> memory = { };
    };

    struct ModelNode
    {
        std::string name = { };
        std::vector<uint32> meshIndices = { };

        std::vector<ModelNode> children = { };
    };

    struct ModelMesh
    {
        uint32 vertexOffset = 0;
        uint32 vertexCount = 0;

        uint32 indexOffset = 0;
        uint32 indexCount = 0;

        std::optional<uint32> materialIndex = std::nullopt;
    };

    struct ModelTexture
    {
        std::string name = { };

        uint32 imageIndex = 0;
        Sierra::SamplerFilter filter = Sierra::SamplerFilter::Linear;
    };

    struct ModelMaterial
    {
        std::string name = { };

        MaterialDiffuse diffuse = { };
        std::optional<size> diffuseTextureIndex = { };

        MaterialSpecular specular = { };
        std::optional<size> specularTextureIndex = { };

        MaterialNormal normal = { };
        std::optional<size> normalTextureIndex = { };

        AlphaMode alphaMode = AlphaMode::Opaque;
        CullMode cullMode = CullMode::SingleSided;
    };

    struct LoadedModel
    {
        std::vector<ModelNode> nodes = { };

        std::vector<Vertex> vertices = { };
        std::vector<uint32> indices = { };
        std::vector<ModelMesh> meshes = { };

        std::vector<LoadedImage> images = { };
        std::vector<ModelTexture> textures = { };
        std::vector<ModelMaterial> materials = { };
    };

    class SIERRA_ENGINE_API ModelLoader
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<LoadedModel> Load(const ModelLoadInfo& loadInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        ModelLoader(const ModelLoader&) = delete;
        ModelLoader& operator=(const ModelLoader&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ModelLoader() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ModelLoader() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        ModelLoader(ModelLoader&&) noexcept = default;
        ModelLoader& operator=(ModelLoader&&) noexcept = default;

    };

}