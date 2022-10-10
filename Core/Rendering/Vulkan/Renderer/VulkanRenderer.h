//
// Created by Nikolay Kanchevski on 4.10.22.
//

#pragma once

#include <vector>
#include <vulkan/vk_enum_string_helper.h>
#include <glm/mat4x4.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "../VulkanCore.h"
#include "../VulkanDebugger.h"
#include "../VulkanUtilities.h"

#include "../../Window.h"

#include "../Abstractions/Image.h"
#include "../Abstractions/RenderPass.h"
#include "../Abstractions/Descriptors.h"
#include "../Abstractions/Sampler.h"
#include "../Abstractions/Buffer.h"

#include "../../../../Engine/Components/Lighting/DirectionalLight.h"
#include "../../../../Engine/Components/Lighting/PointLight.h"
#include "../../../../Engine/Components/Lighting/Spotlight.h"
#include "../../../World.h"

using namespace Sierra::Core::Rendering::Vulkan::Abstractions;

namespace Sierra::Core::Rendering::Vulkan
{
    class VulkanRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */

        /// @brief Creates a new renderer and a window without the need of setting its size. It will automatically be 800x600 or,
        /// if setMaximized, as big as it can be on your display.
        /// @param givenTitle What the givenTitle / name of the window should be.
        /// @param setMaximized A bool indicating whether the window should use all the space on your screen and start setMaximized.
        /// @param setResizable Whether the window is going to be setResizable or not.
        /// @param setFocusRequirement Whether the window requires to be focused in order to draw and handle events.
        VulkanRenderer(std::string givenTitle, const bool setMaximized, const bool setResizable, const bool setFocusRequirement);

        /* --- POLLING METHODS --- */
        void Start();
        void Update();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsActive() const { return window.IsClosed(); };
        [[nodiscard]] inline Window& GetWindow()  { return window; }
        [[nodiscard]] inline float GetDrawTime() const { return drawTime; };

        /* --- DESTRUCTOR --- */
        ~VulkanRenderer();
        VulkanRenderer(const VulkanRenderer &) = delete;
        VulkanRenderer &operator=(const VulkanRenderer &) = delete;

    private:
        /* --- GENERAL --- */
        Window window;

        const bool msaaSamplingEnabled = true;
        VkSampleCountFlagBits msaaSampleCount = msaaSamplingEnabled ? VK_SAMPLE_COUNT_64_BIT : VK_SAMPLE_COUNT_1_BIT;

        enum RenderingMode { Fill, Wireframe, Point };
        RenderingMode renderingMode = Fill;

        /* --- INSTANCE --- */
        const std::vector<const char*> requiredInstanceExtensions
        {
            #if DEBUG
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            #endif
        };

        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

        VkInstance instance;

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
            uint32_t graphicsFamily = -1;
            uint32_t presentFamily = -1;

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

        #if __APPLE__
            bool krhPortabilityRequired = false;
        #endif

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

        std::vector<std::unique_ptr<Image>> swapchainImages;

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
        std::unique_ptr<Subpass> subpass;
        std::unique_ptr<RenderPass> renderPass;
        void CreateRenderPass();

        /* --- PUSH CONSTANTS --- */
        VkPushConstantRange pushConstantRange;
        uint64_t pushConstantSize;
        void CreatePushConstants();

        /* --- DESCRIPTORS --- */
        std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
        void CreateDescriptorSetLayout();
        std::unique_ptr<DescriptorPool> descriptorPool;
        void CreateDescriptorPool();
        std::vector<VkDescriptorSet> uniformDescriptorSets;
        void CreateUniformDescriptorSets();

        /* --- GRAPHICS PIPELINE --- */
        VkPipelineLayout graphicsPipelineLayout;
        VkPipeline graphicsPipeline;
        void CreateGraphicsPipeline();

        /* --- COLOR BUFFER --- */
        std::unique_ptr<Image> colorImage;
        VkSampleCountFlagBits GetHighestSupportedMsaaCount();
        void CreateColorBufferImage();

        /* --- FRAMEBUFFERS --- */
        std::vector<std::unique_ptr<Framebuffer>> swapchainFramebuffers;
        void CreateFramebuffers();

        /* --- TEXTURES --- */
        std::unique_ptr<Sampler> textureSampler;
        const uint32_t MAX_TEXTURES = World::MAX_TEXTURES;
        void CreateTextureSampler();

        /* --- UNIFORM BUFFERS --- */
        struct alignas(16) UniformData
        {
            /* Vertex Uniform Data */
            glm::mat4x4 view;
            glm::mat4x4 projection;

            /* Fragment Uniform Data */
            std::vector<DirectionalLight::UniformDirectionalLight> directionalLights;
            std::vector<PointLight::UniformPointLight> pointLights;
            std::vector<Spotlight::UniformSpotLight> spotLights;

            int directionalLightsCount;
            int pointLightsCount;
            int spotLightsCount;
        };

        UniformData uniformData;
        const uint64_t uniformDataSize = sizeof(UniformData);
        std::vector<std::unique_ptr<Buffer>> uniformBuffers;
        void CreateUniformBuffers();
        void UpdateUniformBuffers(uint32_t imageIndex);

        /* --- QUERYING --- */
        VkQueryPool drawTimeQueryPool;
        float timestampPeriod;
        std::vector<float> drawTimeQueryResults;
        float drawTime;
        void CreateQueryPool();

        /* --- DRAWING --- */
        const uint32_t MAX_CONCURRENT_FRAMES = 3;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> frameBeingRenderedFences;

        uint32_t currentFrame = 0;

        void CreateSynchronization();
        void Draw();

        /* --- ImGui --- */
        VkDescriptorPool imGuiDescriptorPool;
        void CreateImGuiInstance();
        void BeginNewImGuiFrame();
        void UpdateImGuiData();
        void RenderImGui();
    };

}