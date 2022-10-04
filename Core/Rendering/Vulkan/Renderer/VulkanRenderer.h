//
// Created by Nikolay Kanchevski on 4.10.22.
//

#pragma once

#include <vector>
#include "../VulkanDebugger.h"

namespace Sierra::Core::Rendering::Vulkan
{
    class VulkanRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanRenderer() = default;
        void Start();
        void Update();

        /* --- DESTRUCTOR --- */
        ~VulkanRenderer();
        VulkanRenderer(const VulkanRenderer &) = delete;
        VulkanRenderer &operator=(const VulkanRenderer &) = delete;
    private:
        #if DEBUG
            bool VALIDATION_ENABLED = true;
        #else
            bool VALIDATION_ENABLED = false;
        #endif

        /* --- Instance --- */
        std::vector<const char*> requiredInstanceExtensions
        {
            #if DEBUG
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            #endif
        };

        VkInstance instance;
        void CreateInstance();
    };

}