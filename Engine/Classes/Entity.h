//
// Created by Nikolay Kanchevski on 17.10.22.
//

#pragma once

#include <string>
#include <vector>
#include <entt/entt.hpp>
#include "../../Core/Debugger.h"
#include "../Components/InternalComponents.h"
#include "../../Core/World.h"

using Sierra::Core::World;
using Sierra::Core::Debugger;
using namespace Sierra::Engine::Components;

namespace Sierra::Engine::Classes
{
    /// Represents an entity (game object) inside the world. Holds components and has position, rotation and scale in the 3D world space.
    class Entity
    {
    public:

        /* --- CONSTRUCTORS --- */
        Entity();
        Entity(const std::string &givenName);
        Entity(Entity &givenParent);
        Entity(const std::string &givenName, Entity &givenParent);

        /* --- SETTER METHODS --- */
        void SetParent(Entity &givenParent);
        void Destroy();

        /* --- GETTER METHODS --- */
        [[nodiscard]] Transform& GetTransform() const { return GetComponent<Transform>(); }
        [[nodiscard]] std::string& GetTag() const { return GetComponent<Tag>().tag; }
        [[nodiscard]] entt::entity& GetEnttEntity() { return enttEntity; }

        /* --- TEMPLATES --- */
        template <typename T, std::enable_if_t<std::is_base_of_v<Component, T>, bool> = true, typename... Args>
        T& AddComponent(Args&&... args)
        {
            if (HasComponent<T>())
            {
                Debugger::ThrowWarning("Component of type [" + Debugger::TypeToString<T>() + "] already present in entity [" + GetTag() + "]. New components has been dismissed and instead the old one has been returned");
                return GetComponent<T>();
            }

            T& component = World::GetEnttRegistry().emplace<T>(enttEntity, std::forward<Args>(args)...);
            component.SetEnttEntity(enttEntity);
            return component;
        }

        template<typename T, std::enable_if_t<std::is_base_of_v<Component, T>, bool> = true, typename... Args>
        T& AddOrReplaceComponent(Args&&... args)
        {
            T& component = World::GetEnttRegistry().emplace_or_replace<T>(enttEntity, std::forward<Args>(args)...);
            component.SetEnttEntity(enttEntity);
            return component;
        }

        template<typename T>
        T& GetComponent() const
        {
            if (!HasComponent<T>())
            {
                Debugger::ThrowError("Component of type [" + Debugger::TypeToString<T>() + "] does not exist within the entity [" + GetTag() + "]");
            }

            return World::GetEnttRegistry().get<T>(enttEntity);
        }

        template<typename T>
        bool HasComponent() const
        {
            return World::GetEnttRegistry().all_of<T>(enttEntity);
        }

        template<typename T>
        void RemoveComponent() const
        {
            if (!HasComponent<T>())
            {
                Debugger::ThrowWarning("Component of type [" + Debugger::TypeToString<T>() + "] is not exist within entity [" + GetTag() + "]. No components were removed");
                return;
            }

            World::GetEnttRegistry().remove<T>(enttEntity);
        }

        /* --- DESTRUCTOR --- */
        ~Entity();
        Entity(const Entity &) = delete;
        Entity &operator=(const Entity &) = delete;

    private:
        entt::entity enttEntity;

    };

}
