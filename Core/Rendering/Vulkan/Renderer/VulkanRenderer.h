//
// Created by Nikolay Kanchevski on 4.10.22.
//

#pragma once

#if _WIN32
    #define NOMINMAX
#endif

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

#include "../../Window.h"
#include "../Abstractions/Device.h"
#include "../Abstractions/Buffer.h"
#include "../Abstractions/Descriptors.h"
#include "../Abstractions/RenderPass.h"
#include "../Abstractions/Image.h"
#include "../Abstractions/Framebuffer.h"
#include "../Abstractions/OffscreenRenderer.h"
#include "../VulkanTypes.h"

using namespace Sierra::Core::Rendering::Vulkan::Abstractions;

namespace Sierra::Core::Rendering::Vulkan
{
    class VulkanRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */

        /// @brief Creates a new renderer and a window without the need of setting its size. It will automatically be 800x600 or,
        /// if setMaximized, as big as it can be on your display.
        /// @param givenTitle What the givenTitle / tag of the window should be.
        /// @param setMaximized A bool indicating whether the window should use all the space on your screen and start setMaximized.
        /// @param setResizable Whether the window is going to be setResizable or not.
        /// @param setFocusRequirement Whether the window requires to be focused in order to draw and handle events.
        VulkanRenderer(const std::string &givenTitle, const bool setMaximized, const bool setResizable = true, const bool setFocusRequirement = true);

        /* --- POLLING METHODS --- */

        // Should only really be used by the World class if you are not certain you know what you are doing!
        void Prepare();
        void Render();
        void UpdateWindow();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkDescriptorSet GetRenderedTextureDescriptorSet() const { return offscreenImageDescriptorSets[currentFrame]; }
        [[nodiscard]] inline bool IsActive() const { return !window.IsClosed(); };
        [[nodiscard]] inline Window& GetWindow()  { return window; }
        [[nodiscard]] inline auto GetRendererInfo() const { return rendererInfo; };

        /* --- DESTRUCTOR --- */
        ~VulkanRenderer();
        VulkanRenderer(const VulkanRenderer &) = delete;
        VulkanRenderer &operator=(const VulkanRenderer &) = delete;

    private:
        /* --- SETTINGS --- */
        bool msaaSamplingEnabled = true;
        Multisampling msaaSampleCount = msaaSamplingEnabled ? Multisampling::MSAAx64 : Multisampling::MSAAx1;

        enum RenderingMode { Fill, Wireframe };
        RenderingMode renderingMode = Fill;

        /* --- GENERAL --- */
        Window window;
        std::shared_ptr<Device> device;

        void Start();
        void CreateDevice();
        void UpdateRendererInfo();

        bool prepared = false;

        struct
        {
            float drawTime = 0;
            int verticesDrawn = 0;
            int meshesDrawn = 0;
            int objectsInScene = 0;
        } rendererInfo;

        /* --- SWAPCHAIN --- */
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkExtent2D swapchainExtent;

        VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR &givenCapabilities);

        void CreateSwapchain();
        void RecreateSwapchainObjects();
        void DestroySwapchainObjects();

        /* --- OFFSCREEN --- */
        std::unique_ptr<OffscreenRenderer> offscreenRenderer;
        std::vector<VkDescriptorSet> offscreenImageDescriptorSets;

        void CreateOffscreenRenderer();
        void CreateOffscreenImageDescriptorSets();

        /* --- COMMANDS --- */
        std::vector<VkCommandBuffer> commandBuffers;
        void CreateCommandBuffers();
        void RecordCommandBuffer(const VkCommandBuffer &givenCommandBuffer, uint32_t imageIndex);
        void FetchRenderTimeResults(uint32_t swapchainIndex);

        /* --- RENDER PASS --- */
        std::unique_ptr<RenderPass> swapchainRenderPass;
        void CreateRenderPass();

        /* --- PUSH CONSTANTS --- */
        VkPushConstantRange pushConstantRange;
        uint64_t pushConstantSize;
        void CreatePushConstants();

        /* --- DESCRIPTORS --- */
        #define UNIFORM_BUFFER_BINDING 0
        #define STORAGE_BUFFER_BINDING 1

        #define GET_UNIFORM_BUFFER(index)(shaderBuffers[UNIFORM_BUFFER_BINDING * bufferDescriptorSets.size() + index])
        #define GET_STORAGE_BUFFER(index)(shaderBuffers[STORAGE_BUFFER_BINDING * bufferDescriptorSets.size() + index])

        std::unique_ptr<DescriptorSetLayout> globalDescriptorSetLayout;
        std::shared_ptr<BindlessDescriptorSet> globalBindlessDescriptorSet;

        std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
        void CreateDescriptorSetLayout();
        std::shared_ptr<DescriptorPool> descriptorPool;
        void CreateDescriptorPool();
        std::vector<std::unique_ptr<DescriptorSet>> bufferDescriptorSets;
        void CreateShaderBuffersDescriptorSets();

        /* --- GRAPHICS PIPELINE --- */
        VkPipelineLayout graphicsPipelineLayout;
        VkPipeline graphicsPipeline;
        void CreateGraphicsPipeline();

        /* --- FRAMEBUFFERS --- */
        std::vector<std::unique_ptr<Image>> swapchainImages;
        std::vector<std::unique_ptr<Framebuffer>> swapchainFramebuffers;
        void CreateFramebuffers();

        /* --- TEXTURES --- */
        void CreateNullTextures();

        /* --- SHADER BUFFERS --- */
        #define SHADER_BUFFERS_COUNT 2
        std::vector<std::unique_ptr<Buffer>> shaderBuffers;

        void CreateShaderBuffers();
        void UpdateShaderBuffers(uint32_t imageIndex);

        /* --- QUERYING --- */
        VkQueryPool drawTimeQueryPool;
        float timestampPeriod;
        std::vector<float> drawTimeQueryResults;
        void CreateQueryPool();

        /* --- DRAWING --- */
        uint32_t currentFrame = 0;
        uint32_t maxConcurrentFrames = 3;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> frameBeingRenderedFences;

        void CreateSynchronization();
        void Draw();

        /* --- ImGui --- */
        bool imGuiFrameBegan = false;
        VkDescriptorPool imGuiDescriptorPool;

        void CreateImGuiInstance();
        void SetImGuiStyle();
        void BeginNewImGuiFrame();
        void UpdateImGuiData();
        void RenderImGui();
    };

}