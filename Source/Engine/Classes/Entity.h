//
// Created by Nikolay Kanchevski on 17.10.22.
//

#pragma once

namespace Sierra::Engine
{
    // NOTE: Forward declared, since Component.h needs to store an Entity instance
    class Component;

    /// Represents an entity (game object) inside the world. Holds components and has position, rotation and scale in the 3D world space.
    class Entity
    {
    public:
        /* --- CONSTRUCTORS --- */
        Entity() = default;
        explicit Entity(entt::entity givenEnttEntity);
        explicit Entity(const String &givenName);

        /* --- PROPERTIES --- */
        static const Entity Null;

        /* --- SETTER METHODS --- */
        void SetParent(const Entity &parent) const;
        void Destroy() const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] String& GetTag() const;
        [[nodiscard]] inline uint32 GetID() const { return static_cast<uint32>(enttEntity); }
        [[nodiscard]] inline entt::entity GetEnttEntity() const { return enttEntity; }

        [[nodiscard]] inline bool IsNull() const { return enttEntity == entt::null; }
        [[nodiscard]] inline operator bool() const { return !IsNull(); }


        /* --- TEMPLATES --- */
        template <typename T, ENABLE_IF(std::is_base_of_v<Component, T>), typename... Args>
        inline T& AddComponent(Args&&... args) { return World::AddComponent<T>(enttEntity, std::forward<Args>(args)...); }

        template<typename T, ENABLE_IF(std::is_base_of_v<Component, T>), typename... Args>
        inline T& AddOrReplaceComponent(Args&&... args) { return World::AddOrReplaceComponent<T>(enttEntity, std::forward<Args>(args)...); }

        template<typename T>
        [[nodiscard]] inline T& GetComponent() const { return World::GetComponent<T>(enttEntity); }

        template<typename T>
        [[nodiscard]] inline bool HasComponent() const { return World::HasComponent<T>(enttEntity); }

        template<typename T>
        inline void RemoveComponent() const { World::RemoveComponent<T>(enttEntity); }

        /* --- OPERATORS --- */
        inline bool operator==(const Entity &other) const { return enttEntity == other.enttEntity; }
		inline bool operator!=(const Entity &other) const { return !(*this == other); }

    private:
        entt::entity enttEntity { entt::null };

    };

}
