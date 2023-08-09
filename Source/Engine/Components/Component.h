//
// Created by Nikolay Kanchevski on 19.10.22.
//

#pragma once

#include "../Classes/Entity.h"

namespace Sierra::Engine
{

    class Component
    {
    public:
        /* --- SETTER METHODS --- */
        template <typename T, ENABLE_IF(std::is_base_of_v<Component, T>), typename... Args>
        inline T& AddComponent(Args&&... args) { return World::AddComponent<T>(enttEntity, std::forward<Args>(args)...); }

        template<typename T, ENABLE_IF(std::is_base_of_v<Component, T>), typename... Args>
        inline T& AddOrReplaceComponent(Args&&... args) { return World::AddOrReplaceComponent<T>(enttEntity, std::forward<Args>(args)...); }

        /* --- GETTER METHODS --- */
        inline Entity GetEntity() { return Entity(enttEntity); };

        template<typename T>
        [[nodiscard]] inline T& GetComponent() const { return World::GetComponent<T>(enttEntity); }

        template<typename T>
        [[nodiscard]] inline bool HasComponent() const { return World::HasComponent<T>(enttEntity); }

        template<typename T>
        inline void RemoveComponent() const { World::RemoveComponent<T>(enttEntity); }

        /* --- CALLBACKS --- */
        virtual inline void OnAddComponent() { }
        virtual inline void OnRemoveComponent() { }

        virtual inline void OnDrawUI() { }
        virtual inline void Update() { }
        virtual inline void Destroy() { }

    private:
        // Set from the World.h
        entt::entity enttEntity = entt::null;
        friend class World;

    };
}