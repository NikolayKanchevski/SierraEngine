//
// Created by Nikolay Kanchevski on 19.12.22.
//

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <vulkan/vulkan.h>

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
        Swapchain(std::unique_ptr<Window> &givenWindow);
        static std::unique_ptr<Swapchain> Create(std::unique_ptr<Window> &givenWindow);

        /* --- POLLING METHODS --- */
        void BeginRenderPass(VkCommandBuffer givenCommandBuffer);
        void EndRenderPass(VkCommandBuffer givenCommandBuffer);

        VkResult AcquireNextImage();
        VkResult SubmitCommandBuffers();

        /* --- SETTER METHODS --- */
        inline void SetResizeCallback(std::function<void()> &methodToCall) { resizeCallback = methodToCall; }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32_t GetWidth() const { return extent.width; }
        [[nodiscard]] inline uint32_t GetHeight() const { return extent.height; }

        [[nodiscard]] inline std::unique_ptr<CommandBuffer>& GetCurrentCommandBuffer() { return commandBuffers[currentFrame]; }
        [[nodiscard]] inline std::unique_ptr<RenderPass>& GetRenderPass() { return renderPass; }

        [[nodiscard]] inline uint32_t GetCurrentFrameIndex() const { return currentFrame; }
        [[nodiscard]] inline uint32_t GetMaxConcurrentFramesCount() const { return maxConcurrentFrames; }

        [[nodiscard]] inline ImageFormat GetBestSwapchainImageFormat() const { return (ImageFormat) bestSurfaceFormat.format; }
        [[nodiscard]] inline VkColorSpaceKHR GetBestColorSpace() const { return bestSurfaceFormat.colorSpace; }
        [[nodiscard]] inline VkPresentModeKHR GetBestPresentationMode() const { return bestPresentMode; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        Swapchain(const Swapchain &) = delete;
        Swapchain &operator=(const Swapchain &) = delete;

    private:
        std::unique_ptr<Window> &window;
        std::function<void()> resizeCallback = nullptr;

        uint32_t maxConcurrentFrames;
        VkSwapchainKHR vkSwapchain;

        VkExtent2D extent;
        VkSurfaceFormatKHR bestSurfaceFormat;
        VkPresentModeKHR bestPresentMode;

        std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;
        std::vector<std::unique_ptr<Image>> swapchainImages;
        std::vector<std::unique_ptr<Framebuffer>> swapchainFramebuffers;
        std::unique_ptr<RenderPass> renderPass;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        uint32_t imageIndex = 0;
        uint32_t currentFrame = 0;

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
