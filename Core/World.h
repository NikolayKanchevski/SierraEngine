//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include <entt/entt.hpp>
#include "../Core/Rendering/Vulkan/Renderer/VulkanRenderer.h"

using Sierra::Core::Rendering::Vulkan::VulkanRenderer;

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
        static void Prepare(VulkanRenderer &renderer);

        /// @brief Updates all objects, their properties and the corresponding renderer. Draws to the window.
        static void Update(VulkanRenderer &renderer);

        /// @brief Shuts the world down, deallocating all previously allocated memory. Must be called once at the very end of the program.
        static void Shutdown();

        /* --- SETTER METHODS --- */

        /* --- GETTER METHODS --- */
        [[nodiscard]] static entt::registry& GetEnttRegistry() { return enttRegistry; }
        [[nodiscard]] static entt::entity RegisterEntity() { return enttRegistry.create(); }

        /* --- DESTRUCTOR --- */
    private:
        static entt::registry enttRegistry;

        static void UpdateObjects(VulkanRenderer &renderer);
        static void UpdateRenderer(VulkanRenderer &renderer);
    };
}
