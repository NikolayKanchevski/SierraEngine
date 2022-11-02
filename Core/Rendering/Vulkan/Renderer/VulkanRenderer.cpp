//
// Created by Nikolay Kanchevski on 4.10.22.
//

#include <functional>
#include "VulkanRenderer.h"
#include "../../../../Engine/Classes/Stopwatch.h"
#include "../../../../Engine/Classes/MeshObject.h"
#include "../../../World.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan
{
    /* --- CONSTRUCTORS --- */
    VulkanRenderer::VulkanRenderer(std::string givenTitle, const bool setMaximized, const bool setResizable, const bool setFocusRequirement)
     : window(Window(givenTitle, setMaximized, setResizable, setFocusRequirement))
    {
        window.SetResizeCallback([this]{
            Draw();
        });

        Start();
    }

    /* --- POLLING METHODS --- */

    void VulkanRenderer::Start()
    {
        Stopwatch stopwatch;

        CreateInstance();
        if (VALIDATION_ENABLED) CreateValidationMessenger();
        CreateWindowSurface();
        GetPhysicalDevice();
        CreateLogicalDevice();

        CreateCommandPool();
        CreateSwapchain();
        CreateDepthBufferImage();

        CreateRenderPass();
        CreatePushConstants();
        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();

        CreateColorBufferImage();

        CreateFramebuffers();
        CreateCommandBuffers();
        CreateUniformBuffers();

        CreateQueryPool();
        CreateDescriptorPool();
        CreateUniformDescriptorSets();
        CreateSynchronization();

        CreateNullTextures();
        CreateImGuiInstance();

        Entity cubeEntity = Entity("Cube");
        auto &cubeMesh = cubeEntity.AddComponent<Mesh>(vertices, meshIndices);

        window.Show();
        Debugger::DisplaySuccess("Successfully started Vulkan! Initialization took: " + std::to_string(stopwatch.GetElapsedMilliseconds()) + "ms");
    }

    void VulkanRenderer::Prepare()
    {
        prepared = true;

        if (!imGuiFrameBegan)
        {
            BeginNewImGuiFrame();
            imGuiFrameBegan = true;
        }

        if (window.IsFocusRequired() && !window.IsFocused()) return;

        UpdateImGuiData();
    }

    void VulkanRenderer::UpdateWindow()
    {
        window.Update();
    }

    void VulkanRenderer::Render()
    {
        if (!prepared)
        {
            Debugger::ThrowError("Vulkan renderer is not prepared for rendering properly! Make sure you have called World::Prepare() before calling World::Update()");
        }

        if (window.IsFocusRequired() && !window.IsFocused()) return;

        if (imGuiFrameBegan)
        {
            RenderImGui();
            imGuiFrameBegan = false;
        }

        Draw();

        UpdateRendererInfo();
    }

    void VulkanRenderer::UpdateRendererInfo()
    {
        rendererInfo.verticesDrawn = Mesh::totalMeshVertices + ImGui::GetDrawData()->TotalVtxCount;
        rendererInfo.meshesDrawn = Mesh::totalMeshCount;
    }

    /* --- DESTRUCTOR --- */

    VulkanRenderer::~VulkanRenderer()
    {
        vkDeviceWaitIdle(logicalDevice);

        vkDestroyDescriptorPool(logicalDevice, imGuiDescriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();

        DestroySwapchainObjects();
        vkDestroySurfaceKHR(instance, surface, nullptr);

        vkDestroyQueryPool(this->logicalDevice, drawTimeQueryPool, nullptr);

        Texture::DestroyDefaultTextures();

        vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice, graphicsPipelineLayout, nullptr);

        for (const auto &uniformBuffer : uniformBuffers)
        {
            uniformBuffer->Destroy();
        }

        descriptorPool->Destroy();
        descriptorSetLayout->Destroy();

        auto enttMeshView = World::GetEnttRegistry().view<Mesh>();
        for (const auto &entity : enttMeshView)
        {
            enttMeshView.get<Mesh>(entity).Destroy();
        }

        for (int i = MAX_CONCURRENT_FRAMES; i--;)
        {
            vkDestroyFence(logicalDevice, frameBeingRenderedFences[i], nullptr);
            vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
        }

        vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

        vkDestroyDevice(logicalDevice, nullptr);

        if (VALIDATION_ENABLED)
        {
            DestroyDebugUtilsMessengerEXT(instance, validationMessenger, nullptr);
        }

        vkDestroyInstance(instance, nullptr);
    }
}