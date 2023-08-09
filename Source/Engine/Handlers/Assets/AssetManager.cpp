//
// Created by Nikolay Kanchevski on 26.07.23.
//

#include "AssetManager.h"

#include "../Project.h"
#include "ArenaAllocator.h"
#include "../../../Core/Rendering/Bases/VK.h"

namespace Sierra::Engine
{

    // Objects
    UniquePtr<ArenaAllocator> arenaAllocator;
    UniquePtr<Rendering::CommandBuffer> commandBuffer;

    // Collections
    TextureCollection textureCollection;
    CubemapCollection cubemapCollection;
    MaterialCollection materialCollection;
    ModelCollection modelCollection;

    DefaultCollection* defaultCollection;
    EngineIconCollection* engineIconCollection;

    /* --- CONSTRUCTORS --- */

    void AssetManager::Initialize()
    {
        PROFILE_FUNCTION();

        // Create objects
        arenaAllocator = ArenaAllocator::Create();
        commandBuffer = Rendering::CommandBuffer::Create({ });
        commandBuffer->Begin();

        engineIconCollection = new EngineIconCollection();
        defaultCollection = new DefaultCollection();
    }

    /* --- POLLING METHODS --- */

    AssetID AssetManager::ImportModel(const FilePath &filePath)
    {
        // Import model
        AssetID ID = AssetID(filePath);
        modelCollection.AddResource(ID);
        return ID;
    }

    AssetID AssetManager::ImportTexture(const FilePath &filePath)
    {
        // Import texture
        AssetID ID = AssetID(filePath);
        textureCollection.AddResource(ID);
        return ID;
    }

    AssetID AssetManager::ImportCubemap(const FilePath &filePath)
    {
        // Import texture
        AssetID ID = AssetID(filePath);
        cubemapCollection.AddResource(ID);
        return ID;
    }

    AssetID AssetManager::ImportMaterial(const FilePath &filePath)
    {
        // Import material
        AssetID ID = AssetID(filePath);
        materialCollection.AddResource(ID);
        return ID;
    }

    /* --- POLLING METHODS --- */

    void AssetManager::RegisterMesh(const std::vector<Vertex> &vertices, const std::vector<uint32> &indices, uint64 &vertexDataOffset, uint64 &indexDataOffset)
    {
        arenaAllocator->RegisterMesh(commandBuffer, vertices, indices, vertexDataOffset, indexDataOffset);
    }

    void AssetManager::BindMeshes(const UniquePtr<Rendering::CommandBuffer> &commandBuffer)
    {
        arenaAllocator->Bind(commandBuffer);
    }

    void AssetManager::SubmitCommands()
    {
        // Check if asset manager has any commands to submit and if so, submit and wait until they are executed
        if (commandBuffer->IsDirty())
        {
            commandBuffer->End();
            Rendering::VK::GetDevice()->SubmitAndWait(Rendering::QueueType::TRANSFER, { commandBuffer });
            commandBuffer->Begin();
        }
    }

    /* --- GETTER METHODS --- */

    TextureCollection& AssetManager::GetTextureCollection()                     { return textureCollection; }
    CubemapCollection& AssetManager::GetCubemapCollection()                     { return cubemapCollection; }
    MaterialCollection& AssetManager::GetMaterialCollection()                   { return materialCollection; }
    ModelCollection& AssetManager::GetModelCollection()                         { return modelCollection; }

    DefaultCollection& AssetManager::GetDefaultCollection()                     { return *defaultCollection; }
    EngineIconCollection& AssetManager::GetEngineIconCollection()               { return *engineIconCollection; }

    UniquePtr<Rendering::CommandBuffer>& AssetManager::GetCommandBuffer()       { return commandBuffer; }

    /* --- DESTRUCTOR --- */

    void AssetManager::Destroy()
    {
        Rendering::VK::GetDevice()->WaitUntilIdle();

        arenaAllocator->Destroy();

        engineIconCollection->Destroy();
        delete(engineIconCollection);

        defaultCollection->Destroy();
        delete(defaultCollection);

        cubemapCollection.Destroy();
        textureCollection.Destroy();
        modelCollection.Destroy();
    }

}