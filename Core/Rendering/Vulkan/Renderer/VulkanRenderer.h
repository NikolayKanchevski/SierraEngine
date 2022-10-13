//
// Created by Nikolay Kanchevski on 4.10.22.
//

#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include <vulkan/vk_enum_string_helper.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
#include "../../../../Engine/Components/Mesh.h"

#define MAX_UINT_32T 4294967295

using namespace Sierra::Core::Rendering::Vulkan::Abstractions;
using namespace Sierra::Engine::Components;

namespace Sierra::Core::Rendering::Vulkan
{
    class VulkanRenderer
    {
    private:
        // Declared here to keep code consistency
        struct alignas(16) UniformData
        {
            /* Vertex Uniform Data */
            glm::mat4x4 view;
            glm::mat4x4 projection;

            /* Fragment Uniform Data */
//            std::vector<DirectionalLight::UniformDirectionalLight> directionalLights;
//            std::vector<PointLight::UniformPointLight> pointLights;
//            std::vector<Spotlight::UniformSpotLight> spotLights;
//
//            int directionalLightsCount;
//            int pointLightsCount;
//            int spotLightsCount;
        };

        UniformData uniformData;
    public:
        /* --- CONSTRUCTORS --- */

        /// @brief Creates a new renderer and a window without the need of setting its size. It will automatically be 800x600 or,
        /// if setMaximized, as big as it can be on your display.
        /// @param givenTitle What the givenTitle / name of the window should be.
        /// @param setMaximized A bool indicating whether the window should use all the space on your screen and start setMaximized.
        /// @param setResizable Whether the window is going to be setResizable or not.
        /// @param setFocusRequirement Whether the window requires to be focused in order to draw and handle events.
        VulkanRenderer(std::string givenTitle, const bool setMaximized, const bool setResizable = true, const bool setFocusRequirement = true);

        /* --- POLLING METHODS --- */

        // Should only really be used by the World class if you are not certain you know what you are doing!
        void Prepare();
        void Render();
        void UpdateWindow();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsActive() const { return !window.IsClosed(); };
        [[nodiscard]] inline Window& GetWindow()  { return window; }
        [[nodiscard]] inline UniformData* GetUniformDataReference() { return &uniformData; };
        [[nodiscard]] inline auto GetRendererInfo() const { return rendererInfo; };

        /* --- DESTRUCTOR --- */
        ~VulkanRenderer();
        VulkanRenderer(const VulkanRenderer &) = delete;
        VulkanRenderer &operator=(const VulkanRenderer &) = delete;

    private:
        /* --- TESTER --- */
        std::vector<Vertex> vertices
        {
                { { -1.0, -1.0, -1.0 },{ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
                { {  1.0, -1.0, -1.0 },{ 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
                { {  1.0,  1.0, -1.0 },{ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
                { { -1.0,  1.0, -1.0 },{ 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
                { { -1.0, -1.0,  1.0 },{ 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
                { {  1.0, -1.0,  1.0 },{ 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
                { {  1.0,  1.0,  1.0 },{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
                { { -1.0,  1.0,  1.0 },{ 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } }
        };

        std::vector<uint32_t> meshIndices
        {
                0, 1, 3, 3, 1, 2,
                1, 5, 2, 2, 5, 6,
                5, 4, 6, 6, 4, 7,
                4, 0, 7, 7, 0, 3,
                3, 2, 7, 7, 2, 6,
                4, 5, 0, 0, 5, 1
        };

        /* --- GENERAL --- */
        Window window;
        void Start();
        void UpdateRendererInfo();

        bool prepared = false;
        const bool msaaSamplingEnabled = true;
        VkSampleCountFlagBits msaaSampleCount = msaaSamplingEnabled ? VK_SAMPLE_COUNT_64_BIT : VK_SAMPLE_COUNT_1_BIT;

        enum RenderingMode { Fill, Wireframe };
        RenderingMode renderingMode = Wireframe;

        struct
        {
            float drawTime = 0;
            int verticesDrawn = 0;
            int meshesDrawn = 0;
            int objectsInScene = 0;
        } rendererInfo;

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
        std::vector<std::unique_ptr<DescriptorSet>> uniformDescriptorSets;
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
        const uint32_t MAX_TEXTURES = VulkanCore::MAX_TEXTURES;
        void CreateTextureSampler();
        void CreateNullTextures();

        /* --- UNIFORM BUFFERS --- */
        const uint64_t uniformDataSize = sizeof(UniformData);
        std::vector<std::unique_ptr<Buffer>> uniformBuffers;
        void CreateUniformBuffers();
        void UpdateUniformBuffers(uint32_t imageIndex);

        /* --- QUERYING --- */
        VkQueryPool drawTimeQueryPool;
        float timestampPeriod;
        std::vector<float> drawTimeQueryResults;
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
        void SetImGuiStyle();
        void BeginNewImGuiFrame();
        void UpdateImGuiData();
        void RenderImGui();
    };

}