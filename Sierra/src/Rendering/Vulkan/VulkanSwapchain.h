//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "../Swapchain.h"
#include "VulkanResource.h"

#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

namespace Sierra
{

    class SIERRA_API VulkanSwapchain final : public Swapchain, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanSwapchain(const VulkanContext& context, const VulkanDevice& device, const SwapchainCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void AcquireNextImage() override;
        void Present(CommandBuffer& commandBuffer) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] uint32 GetCurrentFrameIndex() const noexcept override { return currentFrame; }
        [[nodiscard]] uint32 GetCurrentImageIndex() const noexcept override { return currentImage; }
        [[nodiscard]] uint32 GetConcurrentFrameCount() const noexcept override { return concurrentFrameCount; }

        [[nodiscard]] float32 GetScaling() const noexcept override { return glm::max(1.0f, static_cast<float32>(swapchainImages[currentImage]->GetWidth()) / static_cast<float32>(window->GetWidth())); }
        [[nodiscard]] const Image& GetImage(const uint32 frameIndex) const override { SR_THROW_IF(frameIndex >= concurrentFrameCount, ValueOutOfRangeError(SR_FORMAT("Cannot get image [{0}] of swapchain [{1}]! Use Swapchain::GetConcurrentFrameCount() to query count", frameIndex, GetName()), frameIndex, uint32(0), GetConcurrentFrameCount() - 1)); return *swapchainImages[frameIndex]; }

        /* --- COPY SEMANTICS --- */
        VulkanSwapchain(const VulkanSwapchain&) = delete;
        VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

        /* --- MOVE SEMANTICS --- */
        VulkanSwapchain(VulkanSwapchain&&) noexcept = default;
        VulkanSwapchain& operator=(VulkanSwapchain&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~VulkanSwapchain() noexcept override;

    private:
        const VulkanContext* context = nullptr;
        const VulkanDevice* device = nullptr;

        std::string name = { };
        Window* window = nullptr;

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkQueue presentationQueue = VK_NULL_HANDLE;
        uint32 presentationQueueFamily = std::numeric_limits<uint32>::max();

        SwapchainPresentationMode preferredPresentationMode = SwapchainPresentationMode::VSync;
        SwapchainBuffering preferredBuffering = SwapchainBuffering::DoubleBuffering;
        SwapchainImageMemoryType preferredImageMemoryType = SwapchainImageMemoryType::UNorm8;

        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<std::unique_ptr<VulkanImage>> swapchainImages = { };

        std::vector<VkSemaphore> isImageAcquiredSemaphores = { };
        std::vector<VkSemaphore> isPresentationCommandBufferFreeSemaphores = { };

        uint32 concurrentFrameCount = 0;
        uint32 currentFrame = 0; // On the CPU
        uint32 currentImage = 0; // On the GPU

        void CreateSwapchain();
        void CreateSynchronization();
        void Recreate();

    };

}
