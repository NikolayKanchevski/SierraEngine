//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Mesh.h"

#include "../../Core/Rendering/Bases/VK.h"
#include "../Handlers/Assets/AssetManager.h"

namespace Sierra::Engine
{

    /* --- CONSTRUCTORS --- */

    Mesh::Mesh(const MeshCreateInfo &createInfo)
        : vertexCount(createInfo.vertices.size()), indexCount(createInfo.indices.size())
    {
        // Register mesh and load returned offsets
        AssetManager::RegisterMesh(createInfo.vertices, createInfo.indices, vertexDataOffset, indexDataOffset);
    }

    UniquePtr<Mesh> Mesh::Create(const MeshCreateInfo &createInfo)
    {
        return std::make_unique<Mesh>(createInfo);
    }

    /* --- DESTRUCTOR --- */

    void Mesh::Destroy()
    {

    }
}