//
// Created by Nikolay Kanchevski on 22.12.22.
//

#include "VulkanRenderer.h"

#include <imgui_impl_vulkan.h>

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    VulkanRenderer::VulkanRenderer(const VulkanRendererCreateInfo &createInfo)
        : window(createInfo.window)
    {
        // Create swapchain
        this->swapchain = Swapchain::Create(window);
        maxConcurrentFrames = swapchain->GetMaxConcurrentFramesCount();

        // Create ImGui instance if requested
        if (createInfo.createImGuiInstance)
        {
            imGuiInstance = ImGuiInstance::Create({
                .window = window,
                .swapchain = swapchain,
                .createImGuizmoLayer = createInfo.createImGuizmoLayer
            });

            hasImGuiInstance = true;
        }

        window->Show();
    }

    std::unique_ptr<VulkanRenderer> VulkanRenderer::Create(VulkanRendererCreateInfo createInfo)
    {
        return std::make_unique<VulkanRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void VulkanRenderer::Prepare()
    {
        prepared = true;

        if (hasImGuiInstance) imGuiInstance->BeginNewImGuiFrame();
    }

    void VulkanRenderer::Update()
    {
        /// Do full update here
    }

    void VulkanRenderer::Render()
    {
        window->Update();

        if (!prepared)
        {
            ASSERT_ERROR("Vulkan renderer is not prepared for rendering properly! Make sure you have called renderer->Prepare() before calling renderer->Render()");
        }

        if (hasImGuiInstance) imGuiInstance->Render();

        /// Do rest of rendering here
    }

    /* --- DESTRUCTOR --- */

    void VulkanRenderer::Destroy()
    {
        /// Destroy the newest objects first here

        if (hasImGuiInstance) imGuiInstance->Destroy();
        swapchain->Destroy();
    }
}