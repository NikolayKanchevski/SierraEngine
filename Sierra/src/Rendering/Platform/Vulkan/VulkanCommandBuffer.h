//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "../../CommandBuffer.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"

namespace Sierra
{

    class SIERRA_API VulkanCommandBuffer final : public CommandBuffer, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanCommandBuffer(const VulkanDevice &device, const CommandBufferCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin() override;
        void End() override;

        void BeginDebugRegion(const std::string &regionName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) const override;
        void InsertDebugMarker(const std::string &markerName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) const override;
        void EndDebugRegion() const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkCommandBuffer GetVulkanCommandBuffer() const { return commandBuffer; }

        /* --- DESTRUCTOR --- */
        ~VulkanCommandBuffer() override;

    private:
        const VulkanDevice &device;

        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    };

}
