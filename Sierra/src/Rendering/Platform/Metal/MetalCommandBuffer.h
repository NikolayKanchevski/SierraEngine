//
// Created by Nikolay Kanchevski on 12.12.23.
//

#pragma once

#include "../../CommandBuffer.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalCommandBuffer final : public CommandBuffer, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalCommandBuffer(const MetalDevice &device, const CommandBufferCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin() override;
        void End() override;

        void SynchronizeBufferUsage(const std::unique_ptr<Buffer> &buffer, BufferCommandUsage previousUsage, BufferCommandUsage nextUsage, uint64 memorySize = 0, uint64 offset = 0) override;
        void SynchronizeImageUsage(const std::unique_ptr<Image> &image, ImageCommandUsage previousUsage, ImageCommandUsage nextUsage, uint32 baseMipLevel = 0, uint32 mipLevelCount = 0, uint32 baseLayer = 0, uint32 layerCount = 0) override;

        void BeginDebugRegion(const std::string &regionName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) const override;
        void InsertDebugMarker(const std::string &markerName, const Color &color = Color(1.0f, 1.0f, 0.0f, 1.0f)) const override;
        void EndDebugRegion() const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline MTL::CommandBuffer* GetMetalCommandBuffer() const { return commandBuffer; }
        [[nodiscard]] inline dispatch_semaphore_t GetCompletionSemaphore() const { return completionSemaphore; }
        [[nodiscard]] inline MTL::RenderCommandEncoder* GetCurrentRenderEncoder() const { return currentRenderEncoder; }

        [[nodiscard]] inline MTL::Buffer* GetCurrentIndexBuffer() const { return currentIndexBuffer; }
        [[nodiscard]] inline uint64 GetCurrentIndexBufferOffset() const { return currentIndexBufferOffset; }

        /* --- CONVERSIONS --- */
        [[nodiscard]] static MTL::RenderStages BufferCommandUsageToRenderStages(BufferCommandUsage bufferCommandUsage);
        [[nodiscard]] static MTL::RenderStages ImageCommandUsageToRenderStages(ImageCommandUsage imageCommandUsage);

        /* --- DESTRUCTOR --- */
        ~MetalCommandBuffer() override = default;

    private:
        const MetalDevice &device;
        MTL::CommandBuffer* commandBuffer = nullptr;
        dispatch_semaphore_t completionSemaphore = nullptr;

        MTL::RenderCommandEncoder* currentRenderEncoder = nullptr;
        MTL::ComputeCommandEncoder* currentComputeEncoder = nullptr;

        MTL::Buffer* currentIndexBuffer = nullptr;
        uint64 currentIndexBufferOffset = 0;

        friend class MetalRenderPass;
        void PushRenderEncoder(MTL::RenderCommandEncoder* renderCommandEncoder);
        void PopRenderEncoder();

        friend class MetalGraphicsPipeline;
        void BindIndexBuffer(MTL::Buffer* indexBuffer, uint64 offset);

        friend class MetalComputePipeline;
        void PushComputeEncoder(MTL::ComputeCommandEncoder* computeCommandEncoder);
        void PopComputeEncoder();

    };

}
