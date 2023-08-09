//
// Created by Nikolay Kanchevski on 26.07.23.
//

#pragma once

#include "ResourceSystem.h"
#include "../../Classes/Asset.h"
#include "../../Classes/Vertex.h"
#include "../../../Core/Rendering/Abstractions/CommandBuffer.h"

namespace Sierra::Engine::AssetManager
{
    /* --- CONSTRUCTOR --- */
    void Initialize();

    /* --- POLLING METHODS --- */
    void RegisterMesh(const std::vector<Vertex> &vertices, const std::vector<uint32> &indices, uint64 &vertexDataOffset, uint64 &indexDataOffset);
    void BindMeshes(const UniquePtr<Rendering::CommandBuffer> &commandBuffer);
    void SubmitCommands();

    /* --- GETTER METHODS --- */
    AssetID ImportTexture(const FilePath &filePath);
    AssetID ImportCubemap(const FilePath &filePath);
    AssetID ImportMaterial(const FilePath &filePath);
    AssetID ImportModel(const FilePath &filePath);

    [[nodiscard]] TextureCollection& GetTextureCollection();
    [[nodiscard]] CubemapCollection& GetCubemapCollection();
    [[nodiscard]] MaterialCollection& GetMaterialCollection();
    [[nodiscard]] ModelCollection& GetModelCollection();
    [[nodiscard]] DefaultCollection& GetDefaultCollection();
    [[nodiscard]] EngineIconCollection& GetEngineIconCollection();

    [[nodiscard]] UniquePtr<Rendering::CommandBuffer>& GetCommandBuffer();

    /* --- DESTRUCTOR --- */
    void Destroy();
}