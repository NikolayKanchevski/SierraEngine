//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "../../CommandBuffer.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"
#include "VulkanDescriptors.h"

#include "VulkanGraphicsPipeline.h"
#include "VulkanComputePipeline.h"

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

        void SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, uint64 memorySize = 0, uint64 byteOffset = 0) override;
        void SynchronizeImageUsage(const std::unique_ptr<Image> &image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseMipLevel = 0, uint32 mipLevelCount = 0, uint32 baseLayer = 0, uint32 layerCount = 0) override;

        void CopyBufferToBuffer(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Buffer> &destinationBuffer, uint64 memoryRange = 0, uint64 sourceByteOffset = 0, uint64 destinationByteOffset = 0) override;
        void CopyBufferToImage(const std::unique_ptr<Buffer> &sourceBuffer, const std::unique_ptr<Image> &destinationImage, uint32 mipLevel, const Vector2UInt &pixelRange = { 0, 0 }, uint32 layer = 0, uint64 sourceByteOffset = 0, const Vector2UInt &destinationPixelOffset = { 0, 0 }) override;
        void GenerateMipMapsForImage(const std::unique_ptr<Image> &image) override;

        void BeginRenderPass(const std::unique_ptr<RenderPass> &renderPass, const std::initializer_list<RenderPassBeginAttachment> &attachments) override;
        void BeginNextSubpass(const std::unique_ptr<RenderPass> &renderPass) override;
        void EndRenderPass(const std::unique_ptr<RenderPass> &renderPass) override;

        void BeginGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline) override;
        void EndGraphicsPipeline(const std::unique_ptr<GraphicsPipeline> &graphicsPipeline) override;

        void BindVertexBuffer(const std::unique_ptr<Buffer> &vertexBuffer, uint64 byteOffset = 0) override;
        void BindIndexBuffer(const std::unique_ptr<Buffer> &indexBuffer, uint64 byteOffset = 0) override;

        void SetScissor(const Vector4UInt &scissor) override;
        void Draw(uint32 vertexCount, uint32 vertexOffset = 0) override;
        void DrawIndexed(uint32 indexCount, uint32 indexOffset = 0, uint32 vertexOffset = 0) override;

        void BeginComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline) override;
        void EndComputePipeline(const std::unique_ptr<ComputePipeline> &computePipeline) override;

        void Dispatch(uint32 xWorkGroupCount, uint32 yWorkGroupCount, uint32 zWorkGroupCount) override;

        void PushConstants(const void* data, uint16 memoryRange, uint16 byteOffset = 0) override;
        void BindBuffer(uint32 binding, const std::unique_ptr<Buffer> &buffer, uint32 arrayIndex = 0, uint64 memoryRange = 0, uint64 byteOffset = 0) override;
        void BindImage(uint32 binding, const std::unique_ptr<Image> &image, uint32 arrayIndex = 0) override;
        void BindImage(uint32 binding, const std::unique_ptr<Image> &image, const std::unique_ptr<Sampler> &sampler, uint32 arrayIndex = 0) override;

        void BeginDebugRegion(const std::string &regionName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) override;
        void InsertDebugMarker(const std::string &markerName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) override;
        void EndDebugRegion() override;

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

        const VulkanGraphicsPipeline* currentGraphicsPipeline = nullptr;
        const VulkanComputePipeline* currentComputePipeline = nullptr;

        VulkanPushDescriptorSet pushDescriptorSet;
        bool resourcesBound = false;
        void BindResources();

    };

}
