//
// Created by Nikolay Kanchevski on 17.10.22.
//

#pragma once

#include "../Components/UUID.h"
#include "../Components/Tag.h"

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
        Entity() = default;
        Entity(const String &givenName);
        Entity(Entity &givenParent);
        Entity(const String &givenName, Entity &givenParent);
        inline Entity(entt::entity givenEnttEntity) : enttEntity(givenEnttEntity) {  }

        /* --- PROPERTIES --- */
        const static Entity Null;

        /* --- SETTER METHODS --- */
        void SetParent(Entity &givenParent);
        void Destroy();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsNull() const { return enttEntity == entt::null; }
        [[nodiscard]] inline bool IsEqual(Entity &right) { return enttEntity == right.enttEntity; }

        [[nodiscard]] inline String& GetTag() const { return GetComponent<Tag>().tag; }
        [[nodiscard]] inline entt::entity& GetEnttEntity() { return enttEntity; }

        /* --- TEMPLATES --- */
        template <typename T, std::enable_if_t<std::is_base_of_v<Component, T>, bool> = true, typename... Args>
        inline T& AddComponent(Args&&... args) { return World::AddComponent<T>(enttEntity, std::forward<Args>(args)...); }

        template<typename T, std::enable_if_t<std::is_base_of_v<Component, T>, bool> = true, typename... Args>
        inline T& AddOrReplaceComponent(Args&&... args) { return World::AddOrReplaceComponent<T>(enttEntity, std::forward<Args>(args)...); }

        template<typename T>
        inline T& GetComponent() const { return World::GetComponent<T>(enttEntity); }

        template<typename T>
        inline bool HasComponent() const { return World::HasComponent<T>(enttEntity); }

        template<typename T>
        inline void RemoveComponent() const { World::RemoveComponent<T>(enttEntity); }

        /* --- DESTRUCTOR --- */
        ~Entity();
        Entity(const Entity&) = default;
        bool operator==(Entity &right);
        operator entt::entity() const noexcept;
        operator UUID() const noexcept;

    private:
        entt::entity enttEntity = entt::null;

    };

}
