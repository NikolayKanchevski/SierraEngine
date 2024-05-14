//
// Created by Nikolay Kanchevski on 6.05.24.
//

#pragma once

#include <entt/entt.hpp>

#include "Components/Tag.h"
#include "Components/UUID.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API Entity final
    {
    public:
        /* --- POLLING METHODS --- */
        template<ComponentType Component, typename... Args>
        Component& AddComponent(Args&&... args)
        {
            if (HasComponent<Component>())
            {
                APP_WARNING("Could not add component of type [{0}] to entity [{1}], as it already contains one of this type, so a reference to it has been returned.", Component::GetName(), GetName());
                return GetComponent<Component>();
            }
            return registry->emplace<Component>(entity, std::forward<Args>(args)...);
        }

        template<ComponentType Component>
        void RemoveComponent()
        {
            if (!HasComponent<Component>())
            {
                APP_WARNING("Could not remove component of type [{0}] from entity [{1}], as it does not contain one of this type, so no components were removed.", Component::GetName(), GetName());
                return;
            }
            registry->remove<Component>(entity);
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string_view GetName() const { return GetComponent<Tag>().GetName(); }
        [[nodiscard]] inline uint64 GetHash() const { return GetComponent<UUID>().GetHash(); }

        template<ComponentType Component>
        [[nodiscard]] bool HasComponent() const
        {
            return registry->any_of<Component>(entity);
        }

        template<ComponentType Component>
        [[nodiscard]] Component& GetComponent() const
        {
            APP_ERROR_IF(!HasComponent<Component>(), "Could not get component of type [{0}] from entity [{1}], as it does not contain one of this type!", Component::GetName(), GetName());
            return registry->get<Component>(entity);
        }

        /* --- OPERATORS --- */
        [[nodiscard]] inline Entity(const Entity&) = default;
        [[nodiscard]] inline Entity& operator=(const Entity&) = default;

        /* --- DESTRUCTOR --- */
        ~Entity() = default;

    private:
        entt::registry* registry = nullptr;
        entt::entity entity = entt::null;

        friend class Scene;
        explicit Entity(std::string_view name, entt::entity entity, entt::registry* registry);

    };


}