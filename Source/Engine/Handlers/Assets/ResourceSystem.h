//
// Created by Nikolay Kanchevski on 6.08.23.
//

#pragma once

#include "../../Types.h"
#include "../../Classes/Time.h"
#include "../../Classes/Asset.h"
#include "../../Classes/Model.h"
#include "../../../Core/Rendering/Abstractions/Cubemap.h"
#include "../../../Core/Rendering/Abstractions/Texture.h"
#include "../../Classes/Material.h"

namespace Sierra::Engine
{
    template <typename T>
    class ResourceSystem
    {
    public:
        typedef SharedPtr<T> ResourceHandle;
        struct Resource
        {
            float lastAccessed;
            ResourceHandle resource;
        };

        /* --- POLLING METHODS --- */
        inline ResourceHandle AddResource(const AssetID &ID)
        {
            // Check if resource already exists and return it
            auto iterator = resourceMap.find(ID);
            if (iterator != resourceMap.end())
            {
                return iterator->second.resource;
            }

            // Try to load resource
            ResourceHandle resource;
            if (!OnLoadCallback(ID, resource))
            {
                ASSERT_WARNING("Could not load resource [{0}] of type [{1}]", ID.GetFileName(), Debugger::TypeToString<T>());
                return ResourceHandle(nullptr);
            }

            // Save resource data
            Resource newResource;
            newResource.resource     = resource;
            newResource.lastAccessed = Time::GetUpTime();

            // Insert resource into pool and return
            resourceMap.emplace(ID, newResource);
            return resource;
        }
        inline ResourceHandle AddResource(const AssetID &ID, const ResourceHandle &resource)
        {
            // Check if resource already exists and return it
            auto iterator = resourceMap.find(ID);
            if (iterator != resourceMap.end())
            {
                return iterator->second.resource;
            }

            // Save resource data
            Resource newResource;
            newResource.resource     = resource;
            newResource.lastAccessed = Time::GetUpTime();

            // Insert resource into pool and return
            resourceMap.emplace(ID, newResource);
            return resource;
        }
        inline ResourceHandle ReloadResource(const AssetID &ID)
        {
            // Check if resource already exists and erase it
            auto iterator = resourceMap.find(ID);
            if (iterator != resourceMap.end())
            {
                resourceMap.erase(iterator);
            }

            // Load the resource (quite literally a re-load :D)
            return AddResource(ID);
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline ResourceHandle GetResource(const AssetID &ID)
        {
            // Check if resource already exists and return it
            auto iterator = resourceMap.find(ID);
            if (iterator != resourceMap.end())
            {
                iterator->second.lastAccessed = Time::GetUpTime();
                return iterator->second.resource;
            }

            // Otherwise try to load it
            return AddResource(ID);
        }
        [[nodiscard]] inline bool ResourceExists(const AssetID &ID)
        {
            resourceMap.find(ID) != resourceMap.end();
        }

        /* --- DESTRUCTOR --- */
        inline virtual void Destroy()
        {
            resourceMap.clear();
        }

    protected:
        std::unordered_map<AssetID, Resource> resourceMap;
        std::function<bool(const AssetID&, ResourceHandle&)> OnLoadCallback;

    };

    class TextureCollection : public ResourceSystem<Rendering::Texture>
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline TextureCollection() { OnLoadCallback = Load; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        static bool Load(const AssetID &ID, SharedPtr<Rendering::Texture> &texture);

    };

    class CubemapCollection : public ResourceSystem<Rendering::Cubemap>
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline CubemapCollection() { OnLoadCallback = Load; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        static bool Load(const AssetID &ID, SharedPtr<Rendering::Cubemap> &cubemap);

    };

    class MaterialCollection : public ResourceSystem<Material>
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline MaterialCollection() { OnLoadCallback = Load; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        static bool Load(const AssetID &ID, SharedPtr<Material> &material);

    };

    class ModelCollection : public ResourceSystem<Model>
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline ModelCollection() { OnLoadCallback = Load; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        static bool Load(const AssetID &ID, SharedPtr<Model> &model);

    };

    class DefaultCollection // Does not inherit ResourceManager, because this needs to store multiple types of assets
    {
    public:
        /* --- CONSTRUCTORS --- */
        DefaultCollection();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline SharedPtr<Rendering::Texture>& GetTexture(const TextureType type) { return defaultTextures[type]; }
        [[nodiscard]] inline SharedPtr<Rendering::Cubemap>& GetCubemap(const CubemapType type) { return defaultCubemaps[type]; }
        [[nodiscard]] inline SharedPtr<Material>& GetMaterial() { return defaultMaterial; }
        [[nodiscard]] inline SharedPtr<Model>& GetModel(const ModelType type) { return defaultModels[type]; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(DefaultCollection);

    private:
        std::unordered_map<TextureType, SharedPtr<Rendering::Texture>> defaultTextures;
        std::unordered_map<CubemapType, SharedPtr<Rendering::Cubemap>> defaultCubemaps;
        SharedPtr<Material> defaultMaterial;
        std::unordered_map<ModelType, SharedPtr<Model>> defaultModels;

    };

    class EngineIconCollection
    {
    public:
        /* --- CONSTRUCTORS --- */
        EngineIconCollection();

        /* --- GETTER METHODS --- */
        [[nodiscard]] ImTextureID GetEngineIcon(const EngineIconType iconType, Vector2 &UV0, Vector2 &UV1);

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(EngineIconCollection);

    private:
        SharedPtr<Rendering::Texture> iconAtlas;

    };

}

