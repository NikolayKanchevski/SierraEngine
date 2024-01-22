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

        void SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, uint64 memorySize = 0, uint64 offset = 0) override;
        void SynchronizeImageUsage(const std::unique_ptr<Image> &image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseMipLevel = 0, uint32 mipLevelCount = 0, uint32 baseLayer = 0, uint32 layerCount = 0) override;

        void BeginDebugRegion(const std::string &regionName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) const override;
        void InsertDebugMarker(const std::string &markerName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) const override;
        void EndDebugRegion() const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkCommandBuffer GetVulkanCommandBuffer() const { return commandBuffer; }
        [[nodiscard]] inline uint64 GetSignalValue() const { return signalValue; }

        /* --- DESTRUCTOR --- */
        ~VulkanCommandBuffer() override;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static VkAccessFlags BufferCommandUsageToVkAccessFlags(BufferCommandUsage bufferCommandUsage);
        [[nodiscard]] static VkPipelineStageFlags BufferCommandUsageToVkPipelineStageFlags(BufferCommandUsage bufferCommandUsage);
        [[nodiscard]] static VkImageLayout ImageCommandUsageToVkLayout(ImageCommandUsage imageCommandUsage);
        [[nodiscard]] static VkAccessFlags ImageCommandUsageToVkAccessFlags(ImageCommandUsage imageCommandUsage);
        [[nodiscard]] static VkPipelineStageFlags ImageCommandUsageToVkPipelineStageFlags(ImageCommandUsage imageCommandUsage);

    private:
        const VulkanDevice &device;

        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        uint64 signalValue = 0;

        friend class VulkanRenderPass;
        static inline std::unordered_map<VkImage, VkImageLayout> imageLayouts;

    };

}
