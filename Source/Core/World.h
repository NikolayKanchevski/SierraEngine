//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

namespace Sierra::Engine
{

    // NOTE: Forward declared, as Entity class needs World.h, due to template inlining
    class Entity;

    class World
    {
    public:
        /* --- POLLING METHODS --- */

        /// @brief Starts the built-in classes. This means that each of them acquires its needed per-initialization data.
        /// Must be called once at the very beginning of the application's code.
        static void Initialize();

        /// @brief Updates the built-in classes and performs required world-related actions. Should be called before each draw.
        static void Update();

        /// @brief Release all memory allocated for the world and shut it down.
        static void Shutdown();

        /* --- SETTER METHODS --- */
        static void DestroyEntity(const entt::entity enttEntity);

        /* --- GETTER METHODS --- */
        [[nodiscard]] static inline entt::entity RegisterEntity() { return enttRegistry.create(); }
        [[nodiscard]] static inline std::vector<entt::entity>& GetOriginEntities() { return originEntities; }

        /* --- TEMPLATES --- */
        template <typename T, typename... Args>
        inline static T& AddComponent(const entt::entity enttEntity, Args&&... args)
        {
            if (HasComponent<T>(enttEntity))
            {
                ASSERT_WARNING("Component of type [{0}] already present in entity. New components has been dismissed and instead the old one has been returned", Debugger::TypeToString<T>());
                return GetComponent<T>(enttEntity);
            }

            T& component = enttRegistry.emplace<T>(enttEntity, std::forward<Args>(args)...);
            component.enttEntity = enttEntity;
            component.OnAddComponent();
            return component;
        }

        template <typename T, typename... Args>
        inline static T& AddOrReplaceComponent(const entt::entity enttEntity, Args&&... args)
        {
            T& component = enttRegistry.emplace_or_replace<T>(enttEntity, std::forward<Args>(args)...);
            component.enttEntity = enttEntity;
            component.OnAddComponent();
            return component;
        }

        template<typename T>
        inline static T& GetComponent(const entt::entity enttEntity)
        {
            ASSERT_ERROR_IF(!HasComponent<T>(enttEntity), "Component of type [{0}] does not exist within the entity", Debugger::TypeToString<T>());
            return enttRegistry.get<T>(enttEntity);
        }

        template<typename T>
        inline static bool HasComponent(const entt::entity enttEntity)
        {
            return enttRegistry.all_of<T>(enttEntity);
        }

        template<typename T>
        inline static void RemoveComponent(const entt::entity enttEntity)
        {
            if (!HasComponent<T>(enttEntity))
            {
                ASSERT_WARNING("Component of type [{0}] does not exist within entity. No components were removed", Debugger::TypeToString<T>());
                return;
            }

            GetComponent<T>(enttEntity).OnRemoveComponent();
            enttRegistry.remove<T>(enttEntity);
        }

        template<typename T>
        inline static auto GetAllComponentsOfType()
        {
            return enttRegistry.view<T>();
        }

    private:
        inline static entt::registry enttRegistry;
        inline static std::vector<entt::entity> originEntities;

    };
}
