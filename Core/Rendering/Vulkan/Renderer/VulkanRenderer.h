//
// Created by Nikolay Kanchevski on 4.10.22.
//

#pragma once

#if _WIN32
    #define NOMINMAX
#endif

#include <vector>
#include <algorithm>
#include <functional>
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include <vulkan/vk_enum_string_helper.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "../../../Debugger.h"
#include "../VulkanUtilities.h"

#include "../../Window.h"

#include "../Abstractions/Image.h"
#include "../Abstractions/RenderPass.h"
#include "../Abstractions/Descriptors.h"
#include "../Abstractions/Sampler.h"
#include "../Abstractions/Buffer.h"
#include "../Abstractions/Texture.h"
#include "../../../../Engine/Structures/Vertex.h"

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
        [[nodiscard]] inline VkDescriptorSet GetRenderedTextureDescriptorSet() const { return offscrenImageDescriptorSets[currentFrame]; }
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
        VkSampleCountFlagBits msaaSampleCount = msaaSamplingEnabled ? VK_SAMPLE_COUNT_64_BIT : VK_SAMPLE_COUNT_1_BIT;

        enum RenderingMode { Fill, Wireframe };
        RenderingMode renderingMode = Fill;

        /* --- GENERAL --- */
        Window window;

        void Start();
        void UpdateRendererInfo();

        bool prepared = false;

        struct
        {
            float drawTime = 0;
            int verticesDrawn = 0;
            int meshesDrawn = 0;
            int objectsInScene = 0;
        } rendererInfo;

        /* --- INSTANCE --- */
        VkInstance instance;

        const std::vector<const char*> requiredInstanceExtensions
        {
            #if DEBUG
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            #endif
        };

        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

        void CreateInstance();
        void CheckExtensionsSupport(std::vector<const char *> &givenExtensions);
        bool ValidationLayersSupported();

        /* --- VALIDATION --- */
        #if DEBUG
            bool VALIDATION_ENABLED = true;
        #else
            bool VALIDATION_ENABLED = false;
        #endif

        VkDebugUtilsMessengerEXT validationMessenger;

        void CreateValidationMessenger();
        void GetValidationMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance givenInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance givenInstance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        /* --- SURFACE --- */
        VkSurfaceKHR surface;
        void CreateWindowSurface();

        /* --- PHYSICAL DEVICE --- */
        VkPhysicalDevice physicalDevice;

        struct QueueFamilyIndices
        {
            int graphicsFamily = -1;
            int presentFamily = -1;

            bool IsValid()
            {
                return graphicsFamily >= 0 && presentFamily >= 0;
            }
        };

        std::vector<const char*> requiredDeviceExtensions
        {
            "VK_KHR_swapchain"
        };

        QueueFamilyIndices queueFamilyIndices{};

        void GetPhysicalDevice();
        bool PhysicalDeviceSuitable(VkPhysicalDevice &givenPhysicalDevice);
        bool DeviceExtensionsSupported(VkPhysicalDevice &givenPhysicalDevice);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice &givenPhysicalDevice);

        /* --- LOGICAL DEVICE --- */
        VkDevice logicalDevice;
        VkQueue graphicsQueue;
        VkQueue presentQueue;

        void CreateLogicalDevice();

        /* --- SWAPCHAIN --- */
        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkFormat swapchainImageFormat;
        VkExtent2D swapchainExtent;

        SwapchainSupportDetails GetSwapchainSupportDetails(VkPhysicalDevice &givenPhysicalDevice);
        VkSurfaceFormatKHR ChooseSwapchainFormat(std::vector<VkSurfaceFormatKHR> &givenFormats);
        VkPresentModeKHR ChooseSwapchainPresentMode(std::vector<VkPresentModeKHR> &givenPresentModes);
        VkExtent2D ChooseSwapchainExtent(VkSurfaceCapabilitiesKHR &givenCapabilities);
        void CreateSwapchain();

        void RecreateSwapchainObjects();
        void DestroySwapchainObjects();

        /* --- COMMANDS --- */
        VkCommandPool commandPool;
        void CreateCommandPool();
        std::vector<VkCommandBuffer> commandBuffers;
        void CreateCommandBuffers();
        void RecordCommandBuffer(const VkCommandBuffer &givenCommandBuffer, uint32_t imageIndex);
        void FetchRenderTimeResults(uint32_t swapchainIndex);

        /* --- DEPTH BUFFER --- */
        std::unique_ptr<Image> depthImage;
        VkFormat depthImageFormat;

        VkFormat GetBestDepthBufferFormat(std::vector<VkFormat> &givenFormats, VkImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags);
        void CreateDepthBufferImage();

        /* --- RENDER PASS --- */
        std::unique_ptr<Subpass> offscreenSubpass;
        std::unique_ptr<RenderPass> offscreenRenderPass;

        std::unique_ptr<Subpass> swapchainSubpass;
        std::unique_ptr<RenderPass> swapchainRenderPass;
        void CreateRenderPasses();

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

        /* --- COLOR BUFFER --- */
        std::unique_ptr<Image> colorImage;
        VkSampleCountFlagBits GetHighestSupportedMsaaCount();
        void CreateColorBufferImage();

        /* --- FRAMEBUFFERS --- */
        std::vector<std::unique_ptr<Image>> offscreenImages;
        std::vector<std::unique_ptr<Image>> swapchainImages;

        std::vector<std::unique_ptr<Framebuffer>> offscreenFramebuffers;
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
        std::vector<VkDescriptorSet> offscrenImageDescriptorSets;

        void CreateImGuiInstance();
        void CreateOffscreenImageDescriptorSets();
        void SetImGuiStyle();
        void BeginNewImGuiFrame();
        void UpdateImGuiData();
        void RenderImGui();
    };

}