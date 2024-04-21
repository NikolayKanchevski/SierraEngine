//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "../../Swapchain.h"
#include "VulkanResource.h"

#include "VulkanDevice.h"
#include "VulkanImage.h"

namespace Sierra
{

    class SIERRA_API VulkanSwapchain final : public Swapchain, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanSwapchain(const VulkanInstance &instance, const VulkanDevice &device, const SwapchainCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void AcquireNextImage() override;
        void Present(std::unique_ptr<CommandBuffer> &commandBuffer) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetCurrentFrameIndex() const override { return currentFrame; }
        [[nodiscard]] inline uint32 GetCurrentImageIndex() const override { return currentImage; }
        [[nodiscard]] inline uint32 GetConcurrentFrameCount() const override { return concurrentFrameCount; }

        [[nodiscard]] inline float32 GetScaling() const override { return static_cast<float32>(swapchainImages[currentImage]->GetWidth()) / static_cast<float32>(window->GetWidth()); }
        [[nodiscard]] inline const std::unique_ptr<Image>& GetImage(const uint32 frameIndex) const override { SR_ERROR_IF(frameIndex >= concurrentFrameCount, "[Vulkan]: Cannot return image with an index [{0}] of swapchain [{1}], as index is out of bounds! Use Swapchain::GetConcurrentFrameCount() to query image count.", frameIndex, GetName()); return swapchainImages[frameIndex]; }

        /* --- DESTRUCTOR --- */
        ~VulkanSwapchain() override;

    private:
        const VulkanInstance &instance;
        const VulkanDevice &device;
        const std::unique_ptr<Window> &window;

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkQueue presentationQueue = VK_NULL_HANDLE;
        uint32 presentationQueueFamily = std::numeric_limits<uint32>::max();

        SwapchainPresentationMode preferredPresentationMode = SwapchainPresentationMode::Immediate;
        SwapchainImageMemoryType preferredImageMemoryType = SwapchainImageMemoryType::UNorm8;

        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<std::unique_ptr<Image>> swapchainImages;

        std::vector<VkSemaphore> isImageAcquiredSemaphores;
        std::vector<VkSemaphore> isPresentationCommandBufferFreeSemaphores;

        uint32 concurrentFrameCount = 0;
        uint32 currentFrame = 0; // On the CPU
        uint32 currentImage = 0; // On the GPU

        void CreateSwapchain();
        void CreateSynchronization();
        void Recreate();

    };

}
