//
// Created by Nikolay Kanchevski on 10.10.22.
//

#pragma once

#include <cstdint>

#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Classes/Input.h"
#include "../Engine/Classes/File.h"
#include "../Engine/Components/Camera.h"
#include "Rendering/Vulkan/Renderer/VulkanRenderer.h"

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

        /* --- DESTRUCTOR --- */
    private:
        static void UpdateObjects(VulkanRenderer &renderer);
        static void UpdateRenderer(VulkanRenderer &renderer);

    };
}
