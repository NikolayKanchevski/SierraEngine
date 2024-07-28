//
// Created by Nikolay Kanchevski on 6.05.24.
//

#pragma once

#include "Component.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API Entity final
    {
    public:
        /* --- CONSTRUCTORS --- */
        Entity() = default;

        /* --- POLLING METHODS --- */
        template<ComponentType Component, typename... Args>
        Component& AddComponent(Args&&... args)
        {
            if (HasComponent<Component>())
            {
                APP_WARNING("Could not add component of type [{0}] to entity [{1}] with a hash of [{2}], as it already contains one of this type, so a reference to it has been returned.", Component::GetName(), GetTag(), GetHash());
                return GetComponent<Component>();
            }
            return registry->emplace<Component>(entity, std::forward<Args>(args)...);
        }

        template<ComponentType Component>
        void RemoveComponent()
        {
            if (!HasComponent<Component>())
            {
                APP_WARNING("Could not remove component of type [{0}] from entity [{1}] with a hash of [{2}], as it does not contain one of this type, so no components were removed.", Component::GetName(), GetTag(), GetHash());
                return;
            }
            registry->remove<Component>(entity);
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetTag() const;
        [[nodiscard]] uint64 GetHash() const;

        [[nodiscard]] bool HasParent() const;
        [[nodiscard]] Entity GetParent() const;

        [[nodiscard]] uint32 GetChildCount() const;
        [[nodiscard]] Entity GetChild(size index) const;

        template<ComponentType Component>
        [[nodiscard]] bool HasComponent() const
        {
            return registry->any_of<Component>(entity);
        }

        template<ComponentType Component>
        [[nodiscard]] Component& GetComponent() const
        {
            APP_ERROR_IF(!HasComponent<Component>(), "Could not get component of type [{0}] from entity [{1}] with a hash of [{2}], as it does not contain one of this type!", Component::GetName(), GetTag(), GetHash());
            return registry->get<Component>(entity);
        }

        /* --- SETTER METHODS --- */
        void SetTag(std::string_view tag) const;

        void SetParent(Entity parent) const;
        void RemoveParent() const;

        void AddChild(Entity child) const;
        void RemoveChild(Entity child) const;

        /* --- OPERATORS --- */
        [[nodiscard]] bool IsNull() const { return entity == entt::null; }
        operator bool() const { return !IsNull(); }

        [[nodiscard]] bool operator==(const Entity &other) const { return registry == other.registry && entity == other.entity; }
        [[nodiscard]] bool operator!=(const Entity &other) const { return !(*this == other); }

        Entity(const Entity&) = default;
        Entity& operator=(const Entity&) = default;

        /* --- DESTRUCTOR --- */
        ~Entity() = default;

    private:
        entt::registry* registry = nullptr;
        entt::entity entity = entt::null;

        friend class Scene;
        explicit Entity(entt::registry* registry, entt::entity entity);

    };


}