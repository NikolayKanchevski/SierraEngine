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

    class SIERRA_API VulkanSwapchain : public Swapchain, public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanSwapchain(const VulkanInstance &instance, const VulkanDevice &device, const SwapchainCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin(const std::unique_ptr<CommandBuffer> &commandBuffer) override;
        void End(const std::unique_ptr<CommandBuffer> &commandBuffer) override;
        void SubmitCommandBufferAndPresent(const std::unique_ptr<CommandBuffer> &commandBuffer) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetCurrentFrame() const override { return currentFrame; }
        [[nodiscard]] inline uint32 GetConcurrentFrameCount() const override { return concurrentFrameCount; }

        [[nodiscard]] inline uint32 GetWidth() const override { return swapchainCreateInfo.imageExtent.width; }
        [[nodiscard]] inline uint32 GetHeight() const override { return swapchainCreateInfo.imageExtent.height; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        const VulkanInstance &instance;
        const VulkanDevice &device;
        const std::unique_ptr<Window> &window;

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkQueue presentationQueue = VK_NULL_HANDLE;

        VkSwapchainCreateInfoKHR swapchainCreateInfo = { };
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;

        VkRenderPass renderPass = VK_NULL_HANDLE;
        std::vector<VkFramebuffer> framebuffers;

        std::vector<VkSemaphore> isImageFreeSemaphores;
        std::vector<VkSemaphore> isImageRenderedSemaphores;
        std::vector<VkFence> isImageUnderWorkFences;

        uint32 concurrentFrameCount = 0;
        uint32 currentFrame = 0; // On the CPU
        uint32 currentImage = 0; // On the GPU

        void Recreate();
        void CreateTemporaryObjects();
        void DestroyTemporaryObjects();

    };

}
