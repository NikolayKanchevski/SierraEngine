//
// Created by Nikolay Kanchevski on 19.12.22.
//

#pragma once

#include "../Window.h"
#include "../Types.h"
#include "Image.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "CommandBuffer.h"

namespace Sierra::Rendering
{

    struct SwapchainCreateInfo
    {
        const UniquePtr<Window> &window;
    };

    class Swapchain
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Swapchain(const SwapchainCreateInfo &createInfo);
        static UniquePtr<Swapchain> Create(const SwapchainCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void BeginRenderPass(const UniquePtr<CommandBuffer> &commandBuffer);
        void EndRenderPass(const UniquePtr<CommandBuffer> &commandBuffer);

        VkResult AcquireNextImage();
        VkResult SwapImage(const UniquePtr<CommandBuffer> &commandBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetWidth() const { return extent.width; }
        [[nodiscard]] inline uint32 GetHeight() const { return extent.height; }

        [[nodiscard]] inline uint32 GetCurrentFrameIndex() const { return currentFrame; }
        [[nodiscard]] inline UniquePtr<RenderPass>& GetRenderPass() { return renderPass; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Swapchain);

    private:
        const UniquePtr<Window> &window;

        const uint32 MAX_CONCURRENT_FRAMES;
        VkSwapchainKHR vkSwapchain = VK_NULL_HANDLE;

        VkExtent2D extent;
        VkSurfaceFormatKHR bestSurfaceFormat;
        VkPresentModeKHR bestPresentMode;

        std::vector<UniquePtr<Image>> swapchainImages;
        std::vector<UniquePtr<Framebuffer>> framebuffers;
        UniquePtr<RenderPass> renderPass;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        uint32 imageIndex = 0;
        uint32 currentFrame = 0;

        void GetSurfaceData();
        void CreateSwapchain();
        void CreateImages();
        void CreateRenderPass();
        void CreateFramebuffers();
        void CreateSynchronization();

        void RecreateSwapchain();
        void DestroyTemporaryObjects();

        struct SwapchainSupportDetails
        {
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        SwapchainSupportDetails GetSwapchainSupportDetails();
        VkExtent2D GetSwapchainExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities);
        VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &surfaceFormats);
        VkPresentModeKHR ChooseBestSwapchainPresentMode(const std::vector<VkPresentModeKHR> &presentModes);
    };
}
