//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "../Assets/AssetManager.h"
#include "Component.h"

#include "Components/Relationship.h"

namespace SierraEngine
{

    struct SceneCreateInfo
    {
        std::string_view name = "Scene";
    };

    class SIERRA_ENGINE_API Scene final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<ComponentType... Components>
        using ComponentCallback = std::function<void(Components&...)>;

        /* --- CONSTRUCTORS --- */
        explicit Scene(const SceneCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] EntityID CreateEntity(std::string_view tag = "Entity");
        void DestroyEntity(EntityID entityID);

        template<ComponentType Component, typename... Args>
        Component& AddEntityComponent(const EntityID entityID, Args&&... args)
        {
            return registry.get_or_emplace<Component>(static_cast<entt::entity>(entityID.GetHash()), std::forward<Args>(args)...);
        }

        template<ComponentType Component>
        bool RemoveEntityComponent(const EntityID entityID)
        {
            return registry.remove<Component>(static_cast<entt::entity>(entityID.GetHash())) > 0;
        }

        template<ComponentType Component>
        void ForEachComponent(ComponentCallback<Component>& Callback) const { registry.view<Component>().each(Callback); }

        template<ComponentType... Components>
        void ForEachComponentPair(ComponentCallback<Components...>& Callback) const { registry.view<Component>().each(Callback); }

        /* --- SETTER METHODS --- */
        void SetEntityParent(EntityID entityID, EntityID parentID);
        void RemoveEntityParent(EntityID entityID);

        void AddEntityChild(EntityID entityID, EntityID childID);
        void RemoveEntityChild(EntityID entityID, EntityID childID);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const { return name; }

        [[nodiscard]] bool EntityExists(EntityID entityID) const;
        [[nodiscard]] uint32 GetEntityCount() const { return entityCount; }

        template<ComponentType Component>
        [[nodiscard]] bool EntityHasComponent(const EntityID entityID) const
        {
            return registry.any_of<Component>(static_cast<entt::entity>(entityID.GetHash()));
        }

        template<ComponentType Component>
        [[nodiscard]] Component* GetEntityComponent(const EntityID entityID)
        {
            return registry.try_get<Component>(static_cast<entt::entity>(entityID.GetHash()));
        }

        template<ComponentType Component>
        [[nodiscard]] const Component* GetEntityComponent(const EntityID entityID) const
        {
            return registry.try_get<Component>(static_cast<entt::entity>(entityID.GetHash()));
        }

        [[nodiscard]] bool EntityHasParent(EntityID entityID) const;
        [[nodiscard]] EntityID GetEntityParent(EntityID entityID) const;
        [[nodiscard]] std::span<const EntityID> GetEntityChildren(EntityID entityID) const;

        [[nodiscard]] std::span<const EntityID> GetRootEntities() const { return rootEntities; }

        /* --- COPY SEMANTICS --- */
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        /* --- MOVE SEMANTICS --- */
        Scene(Scene&&) = default;
        Scene& operator=(Scene&&) = default;

        /* --- DESTRUCTOR --- */
        ~Scene() = default;

    private:
        std::string name;

        uint32 entityCount = 0;
        entt::registry registry = { };

        std::vector<EntityID> rootEntities;

    };

}
