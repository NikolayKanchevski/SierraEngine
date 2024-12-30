//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "EntityID.h"
#include "Component.h"
#include "../Assets/AssetManager.h"

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
            return registry.get_or_emplace<Component>(static_cast<entt::entity>(entityID.GetValue()), std::forward<Args>(args)...);
        }

        template<ComponentType Component>
        bool RemoveEntityComponent(const EntityID entityID) noexcept
        {
            return registry.remove<Component>(static_cast<entt::entity>(entityID.GetValue())) > 0;
        }

        template<ComponentType Component>
        void ForEachComponent(const ComponentCallback<Component>& Callback)
        {
            registry.view<Component>().each(Callback);
        }

        template<ComponentType Component>
        void ForEachComponent(const ComponentCallback<const Component>& Callback) const
        {
            registry.view<const Component>().each(Callback);
        }

        template<ComponentType... Components>
        void ForEachComponentPair(const ComponentCallback<Components...>& Callback)
        {
            registry.view<Components...>().each(Callback);
        }

        template<ComponentType... Components>
        void ForEachComponentPair(const ComponentCallback<const Components...>& Callback) const
        {
            registry.view<const Components...>().each(Callback);
        }

        /* --- SETTER METHODS --- */
        void SetEntityParent(EntityID entityID, EntityID parentID) noexcept;
        void RemoveEntityParent(EntityID entityID) noexcept;

        void AddEntityChild(EntityID entityID, EntityID childID) noexcept;
        void RemoveEntityChild(EntityID entityID, EntityID childID) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept { return name; }

        [[nodiscard]] bool EntityExists(EntityID entityID) const noexcept;
        [[nodiscard]] uint32 GetEntityCount() const noexcept { return entityCount; }

        template<ComponentType Component>
        [[nodiscard]] bool EntityHasComponent(const EntityID entityID) const noexcept
        {
            return registry.any_of<Component>(static_cast<entt::entity>(entityID.GetValue()));
        }

        template<ComponentType Component>
        [[nodiscard]] Component* GetEntityComponent(const EntityID entityID) noexcept
        {
            return registry.try_get<Component>(static_cast<entt::entity>(entityID.GetValue()));
        }

        template<ComponentType Component>
        [[nodiscard]] const Component* GetEntityComponent(const EntityID entityID) const noexcept
        {
            return registry.try_get<const Component>(static_cast<entt::entity>(entityID.GetValue()));
        }

        [[nodiscard]] bool EntityHasParent(EntityID entityID) const noexcept;
        [[nodiscard]] EntityID GetEntityParent(EntityID entityID) const noexcept;
        [[nodiscard]] std::span<const EntityID> GetEntityChildren(EntityID entityID) const noexcept;

        [[nodiscard]] std::span<const EntityID> GetRootEntities() const noexcept { return rootEntities; }

        /* --- COPY SEMANTICS --- */
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        /* --- MOVE SEMANTICS --- */
        Scene(Scene&&) noexcept = default;
        Scene& operator=(Scene&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Scene() noexcept = default;

    private:
        std::string name = { };

        uint32 entityCount = 0;
        entt::registry registry = { };

        std::vector<EntityID> rootEntities;

    };

}
