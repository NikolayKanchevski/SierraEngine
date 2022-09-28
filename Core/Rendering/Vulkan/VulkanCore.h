//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include "../Window.h"

namespace Sierra::Core::Rendering::Vulkan
{
    class VulkanCore
    {
    public:
        static Window* window;
        static GLFWwindow* glfwWindow;
    };
}
