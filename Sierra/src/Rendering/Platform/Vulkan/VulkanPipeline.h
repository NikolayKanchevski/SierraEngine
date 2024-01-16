//
// Created by Nikolay Kanchevski on 8.01.24.
//

#pragma once

#include "../../Pipeline.h"
#include "VulkanResource.h"

#include "../../PipelineLayout.h"
#include "../../CommandBuffer.h"

#include "VulkanDevice.h"
#include "VulkanPipelineLayout.h"
#include "VulkanDescriptorSets.h"

namespace Sierra
{

    struct VulkanPipelineCreateInfo
    {
        const std::string &name = "Vulkan Pipeline";
        uint32 concurrentFrameCount = 1;
        const std::unique_ptr<PipelineLayout> &layout;
        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
    };

    class SIERRA_API VulkanPipeline : public virtual Pipeline, public VulkanResource
    {
    public:
        /* --- POLLING METHODS --- */
        void Begin(std::unique_ptr<CommandBuffer> &commandBuffer) const override;
        void End(std::unique_ptr<CommandBuffer> &commandBuffer) const override;

        void PushConstants(std::unique_ptr<CommandBuffer> &commandBuffer, const void* data, uint16 memoryRange, uint16 offset = 0) const override;
        void BindBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 binding, const std::unique_ptr<Buffer> &buffer, uint32 arrayIndex = 0, uint64 memoryRange = 0, uint64 offset = 0) const override;
        void BindImage(std::unique_ptr<CommandBuffer> &commandBuffer, uint32 binding, const std::unique_ptr<Image> &image, uint32 arrayIndex = 0) const override;

        /* --- DESTRUCTOR --- */
        ~VulkanPipeline() override = default;

    protected:
        VulkanPipeline(const VulkanDevice &device, const VulkanPipelineCreateInfo &createInfo);
        const VulkanDevice &device;
        const VulkanPipelineLayout &pipelineLayout;

        VkPipeline pipeline = VK_NULL_HANDLE;
        void BindResources(const VulkanCommandBuffer &commandBuffer) const;

    private:
        VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
        std::vector<std::unique_ptr<VulkanPushDescriptorSet>> descriptorSets;

        uint32 concurrentFrameCount = 0;
        mutable uint32 currentFrame = 0;
        mutable bool resourcesBound = false;

    };

}
