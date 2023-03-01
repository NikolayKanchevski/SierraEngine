//
// Created by Nikolay Kanchevski on 19.10.22.
//

#pragma once

namespace Sierra::Engine::Components
{
    using Sierra::Core::World;

    class Component
    {
    public:
        inline void SetEnttEntity(entt::entity givenEntity) { this->enttEntity = givenEntity; }
        inline entt::entity GetEnttEntity() { return enttEntity; };

        template <typename T, ENABLE_IF(std::is_base_of_v<Component, T>), typename... Args>
        inline T& AddComponent(Args&&... args) { return World::AddComponent<T>(enttEntity, std::forward<Args>(args)...); }

        template<typename T, ENABLE_IF(std::is_base_of_v<Component, T>), typename... Args>
        inline T& AddOrReplaceComponent(Args&&... args) { return World::AddOrReplaceComponent<T>(enttEntity, std::forward<Args>(args)...); }

        template<typename T>
        inline T& GetComponent() const { return World::GetComponent<T>(enttEntity); }

        template<typename T>
        inline bool HasComponent() const { return World::HasComponent<T>(enttEntity); }

        template<typename T>
        inline void RemoveComponent() const { World::RemoveComponent<T>(enttEntity); }

        virtual inline void OnAddComponent() { }
        virtual inline void OnRemoveComponent() { }

        virtual inline void OnDrawUI() { }
        virtual inline void Update() { }
        virtual inline void Destroy() const { }

    protected:
        entt::entity enttEntity = entt::null;

    };
}