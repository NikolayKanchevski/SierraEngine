//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include <entt/entt.hpp>

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

        /* --- GETTER METHODS --- */
        [[nodiscard]] static std::shared_ptr<entt::registry>& GetEnttRegistry() { return enttRegistry; }
        [[nodiscard]] static entt::entity RegisterEntity() { return enttRegistry->create(); }

        /* --- DESTRUCTOR --- */
    private:
        inline static std::shared_ptr<entt::registry> enttRegistry = std::make_shared<entt::registry>();

        static void UpdateObjects();
    };
}
