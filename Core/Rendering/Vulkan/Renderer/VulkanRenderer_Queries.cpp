//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateQueryPool()
    {
        // Set up draw time query creation info
        VkQueryPoolCreateInfo queryPoolCreateInfo{};
        queryPoolCreateInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        queryPoolCreateInfo.queryCount = static_cast<uint32_t>(this->commandBuffers.size() * 2);
        queryPoolCreateInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;

        // Create the draw time query pool
        VulkanDebugger::CheckResults(
            vkCreateQueryPool(this->logicalDevice, &queryPoolCreateInfo, nullptr, &drawTimeQueryPool),
            "Failed to create query pool"
        );

        // Get the timestamp period of the physical device
        timestampPeriod = VulkanCore::GetPhysicalDeviceProperties().limits.timestampPeriod;

        // Resize query results vector
        drawTimeQueryResults.resize(MAX_CONCURRENT_FRAMES);
    }

}