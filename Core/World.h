//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

namespace Sierra::Core
{

    class World
    {
    public:
        /* --- POLLING METHODS --- */

        /// @brief Starts the built-in classes. This means that each of them acquires its needed per-initialization data.
        /// Must be called once at the very beginning of the application's code.
        static void Start();

        /// @brief Updates the built-in classes and performs required world-related actions before each draw.
        static void Prepare();

        /// @brief Updates all objects, their properties and the corresponding renderer. Draws to the window.
        static void Update();

        /* --- SETTER METHODS --- */
        static inline void DestroyEntity(const entt::entity enttEntity) { enttRegistry->destroy(enttEntity); }

        /* --- GETTER METHODS --- */
        [[nodiscard]] static inline entt::entity RegisterEntity() { return enttRegistry->create(); }
        [[nodiscard]] static inline std::unordered_map<uint, entt::entity>& GetOriginEntitiesList() { return originEntities; }

        /* --- TEMPLATES --- */
        template <typename T, typename... Args>
        inline static T& AddComponent(const entt::entity enttEntity, Args&&... args)
        {
            if (HasComponent<T>(enttEntity))
            {
                ASSERT_WARNING("Component of type [" + Debugger::TypeToString<T>() + "] already present in entity. New components has been dismissed and instead the old one has been returned");
                return GetComponent<T>(enttEntity);
            }

            T& component = enttRegistry->emplace<T>(enttEntity, std::forward<Args>(args)...);
            component.SetEnttEntity(enttEntity);
            component.OnAddComponent();
            return component;
        }

        template <typename T, typename... Args>
        inline static T& AddOrReplaceComponent(const entt::entity enttEntity, Args&&... args)
        {
            T& component = enttRegistry->emplace_or_replace<T>(enttEntity, std::forward<Args>(args)...);
            component.SetEnttEntity(enttEntity);
            component.OnAddComponent();
            return component;
        }

        template<typename T>
        inline static T& GetComponent(const entt::entity enttEntity)
        {
            ASSERT_ERROR_IF(!HasComponent<T>(enttEntity), "Component of type [" + Debugger::TypeToString<T>() + "] does not exist within the entity");

            return enttRegistry->get<T>(enttEntity);
        }

        template<typename T>
        inline static bool HasComponent(const entt::entity enttEntity)
        {
            return enttRegistry->all_of<T>(enttEntity);
        }

        template<typename T>
        inline static void RemoveComponent(const entt::entity enttEntity)
        {
            if (!HasComponent<T>(enttEntity))
            {
                ASSERT_WARNING("Component of type [" + Debugger::TypeToString<T>() + "] does not exist within entity. No components were removed");
                return;
            }

            enttRegistry->remove<T>(enttEntity);
        }

        template<typename T>
        inline static auto GetAllComponentsOfType()
        {
            return enttRegistry->view<T>();
        }

        /* --- DESTRUCTOR --- */
    private:
        inline static std::unordered_map<uint, entt::entity> originEntities;
        inline static std::shared_ptr<entt::registry> enttRegistry = std::make_shared<entt::registry>();
    };
}
