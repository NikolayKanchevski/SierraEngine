//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include <entt/entt.hpp>

#include "Component.h"

namespace SierraEngine
{

    struct SceneCreateInfo
    {

    };

    class Entity;
    class Scene final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Scene(const SceneCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] Entity CreateEntity(std::string_view name = "Entity");
        void DestroyEntity(Entity entity);

        /* --- GETTER METHODS --- */
        template<typename T> requires (IsComponent<T>)
        [[nodiscard]] inline auto GetAllComponents()
        {
            return registry.view<T>();
        }

        /* --- OPERATORS --- */
        Scene(const Scene&) = delete;
        Scene &operator=(const Scene&) = delete;

        /* --- DESTRUCTOR --- */
        ~Scene() = default;

    private:
        friend class Entity;
        friend class SceneSerializer;
        entt::registry registry = { };

    };

    class Entity final
    {
    public:
        /* --- POLLING METHODS --- */
        template<typename T, typename... Args> requires (IsComponent<T>)
        T& AddComponent(Args&&... args)
        {
            if (HasComponent<T>())
            {
                APP_WARNING("Could not add component of type [{0}] to entity [{1}], as it already contains one of this type, so a reference to it has been returned.", GetComponentName<T>(), GetName());
                return GetComponent<T>();
            }
            return scene->registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename T> requires (IsComponent<T>)
        void RemoveComponent()
        {
            if (!HasComponent<T>())
            {
                APP_WARNING("Could not remove component of type [{0}] from entity [{1}], as it does not contain one of this type, so no components were removed.", GetComponentName<T>(), GetName());
                return;
            }
            scene->registry.remove<T>(entity);
        }

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const;
        [[nodiscard]] uint64 GetHash() const;

        template<typename T> requires (IsComponent<T>)
        [[nodiscard]] inline bool HasComponent() const
        {
            return scene->registry.all_of<T>(entity);
        }

        template<typename T> requires (IsComponent<T>)
        [[nodiscard]] T& GetComponent() const
        {
            APP_ERROR_IF(!HasComponent<T>(), "Could not get component of type [{0}] from entity [{1}], as it does not contain one of this type!", GetComponentName<T>(), GetName());
            return scene->registry.get<T>(entity);
        }

        /* --- OPERATORS --- */
        [[nodiscard]] inline Entity(const Entity&) = default;
        [[nodiscard]] inline Entity &operator=(const Entity&) = default;

        /* --- DESTRUCTOR --- */
        ~Entity() = default;

    private:
        Scene* scene = nullptr;
        entt::entity entity = entt::null;

        friend class Scene;
        explicit Entity(std::string_view name, entt::entity entity, Scene* scene);

    };

}
