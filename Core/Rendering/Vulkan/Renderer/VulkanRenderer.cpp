//
// Created by Nikolay Kanchevski on 4.10.22.
//

#include "VulkanRenderer.h"

#include <imgui_impl_vulkan.h>

#include "../../../World.h"
#include "../../../../Engine/Classes/Mesh.h"
#include "../../../../Engine/Classes/Stopwatch.h"
#include "../../../../Engine/Components/MeshRenderer.h"

using namespace Sierra::Engine::Components;
using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan
{
    /* --- CONSTRUCTORS --- */
    VulkanRenderer::VulkanRenderer(const std::string &givenTitle, const bool setMaximized, const bool setResizable, const bool setFocusRequirement)
     : window(Window(givenTitle, setMaximized, setResizable, setFocusRequirement))
    {
        Start();
    }

    /* --- POLLING METHODS --- */

    void VulkanRenderer::Start()
    {
        Stopwatch stopwatch;

        CreateDevice();
        CreateSwapchain();

        CreateOffscreenRenderer();
        CreateRenderPass();
        CreatePushConstants();
        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();

        CreateFramebuffers();
        CreateCommandBuffers();
        CreateShaderBuffers();

        CreateQueryPool();
        CreateDescriptorPool();
        CreateShaderBuffersDescriptorSets();
        CreateSynchronization();

        CreateNullTextures();

        CreateImGuiInstance();
        CreateOffscreenImageDescriptorSets();

        window.Show();
        ASSERT_SUCCESS("Successfully started Vulkan! Initialization took: " + std::to_string(stopwatch.GetElapsedMilliseconds()) + "ms");
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
            ASSERT_ERROR("Vulkan renderer is not prepared for rendering properly! Make sure you have called World::Prepare() before calling World::Update()");
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
        rendererInfo.verticesDrawn = Mesh::GetTotalVertexCount() + ImGui::GetDrawData()->TotalVtxCount;
        rendererInfo.meshesDrawn = Mesh::GetTotalMeshCount();
    }

    /* --- DESTRUCTOR --- */

    VulkanRenderer::~VulkanRenderer()
    {
        vkDeviceWaitIdle(device->GetLogicalDevice());

        vkDestroyDescriptorPool(device->GetLogicalDevice(), imGuiDescriptorPool, nullptr);

        ImGui_ImplVulkan_Shutdown();

        DestroySwapchainObjects();

        swapchainRenderPass->Destroy();

        vkDestroyQueryPool(device->GetLogicalDevice(), drawTimeQueryPool, nullptr);

        Texture::DestroyDefaultTextures();

        offscreenRenderer->Destroy();

        vkDestroyPipeline(device->GetLogicalDevice(), graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device->GetLogicalDevice(), graphicsPipelineLayout, nullptr);

        for (const auto &buffer : shaderBuffers)
        {
            buffer->Destroy();
        }

        descriptorPool->Destroy();
        descriptorSetLayout->Destroy();

        auto enttMeshView = World::GetEnttRegistry().view<MeshRenderer>();
        for (const auto &entity : enttMeshView)
        {
            enttMeshView.get<MeshRenderer>(entity).Destroy();
        }

        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            vkDestroyFence(device->GetLogicalDevice(), frameBeingRenderedFences[i], nullptr);
            vkDestroySemaphore(device->GetLogicalDevice(), renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device->GetLogicalDevice(), imageAvailableSemaphores[i], nullptr);
        }

        device->Destroy();
    }
}