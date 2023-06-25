//
// Created by Nikolay Kanchevski on 19.12.22.
//

#pragma once

#include "../../Window.h"
#include "../VulkanTypes.h"
#include "Image.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "CommandBuffer.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    class Swapchain
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Swapchain(UniquePtr<Window> &givenWindow);
        static UniquePtr<Swapchain> Create(UniquePtr<Window> &givenWindow);

        /* --- POLLING METHODS --- */
        void BeginRenderPass(const UniquePtr<CommandBuffer> &givenCommandBuffer);
        void EndRenderPass(const UniquePtr<CommandBuffer> &givenCommandBuffer);

        VkResult AcquireNextImage();
        VkResult SwapImage();

        /* --- SETTER METHODS --- */
        inline void SetResizeCallback(Callback &methodToCall) { resizeCallback = methodToCall; }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetWidth() const { return extent.width; }
        [[nodiscard]] inline uint GetHeight() const { return extent.height; }

        [[nodiscard]] inline UniquePtr<CommandBuffer>& GetCurrentCommandBuffer() { return commandBuffers[currentFrame]; }
        [[nodiscard]] inline UniquePtr<RenderPass>& GetRenderPass() { return renderPass; }

        [[nodiscard]] inline UniquePtr<Image>& GetImage(const uint index) { return swapchainImages[index]; }
        [[nodiscard]] inline UniquePtr<Image>& GetCurrentImage() { return GetImage(currentFrame); }

        [[nodiscard]] inline uint GetCurrentFrameIndex() const { return currentFrame; }

        [[nodiscard]] inline ImageFormat GetBestSwapchainImageFormat() const { return (ImageFormat) bestSurfaceFormat.format; }
        [[nodiscard]] inline VkColorSpaceKHR GetBestColorSpace() const { return bestSurfaceFormat.colorSpace; }
        [[nodiscard]] inline VkPresentModeKHR GetBestPresentationMode() const { return bestPresentMode; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Swapchain);

    private:
        UniquePtr<Window> &window;
        Callback resizeCallback = nullptr;

        uint maxConcurrentFrames;
        VkSwapchainKHR vkSwapchain = VK_NULL_HANDLE;

        VkExtent2D extent;
        VkSurfaceFormatKHR bestSurfaceFormat;
        VkPresentModeKHR bestPresentMode;

        std::vector<UniquePtr<CommandBuffer>> commandBuffers;
        std::vector<UniquePtr<Image>> swapchainImages;
        std::vector<UniquePtr<Framebuffer>> swapchainFramebuffers;
        UniquePtr<RenderPass> renderPass;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        uint imageIndex = 0;
        uint currentFrame = 0;

        void GetSurfaceData();
        void CreateSwapchain();
        void CreateCommandBuffers();
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
        VkSurfaceFormatKHR ChooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &givenFormats);
        VkPresentModeKHR ChooseBestSwapchainPresentMode(const std::vector<VkPresentModeKHR> &givenPresentModes);
        VkExtent2D GetSwapchainExtent(VkSurfaceCapabilitiesKHR givenSurfaceCapabilities);
    };
}
