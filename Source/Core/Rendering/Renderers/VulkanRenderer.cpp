//
// Created by Nikolay Kanchevski on 22.12.22.
//

#include "VulkanRenderer.h"

#include "../Bases/VK.h"
#include "../../../Engine/Handlers/Project.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    VulkanRenderer::VulkanRenderer(const VulkanRendererCreateInfo &createInfo)
        : window(createInfo.window), MAX_CONCURRENT_FRAMES(VK::GetDevice()->GetMaxConcurrentFramesCount())
    {
        // Create swapchain
        swapchain = Swapchain::Create({ .window = window });

        // Create ImGui instance if requested
        if (createInfo.createImGuiInstance)
        {
            imGuiInstance = ImGuiInstance::Create({
                .window = window,
                .renderPass = swapchain->GetRenderPass(),
                .fontCreateInfos = {
                    { .fontFilePath = Engine::Project::GetAssetDirectory() / "Fonts/PTSans.ttf", .fontSize = 18.0f },
                    { .fontFilePath = Engine::Project::GetAssetDirectory() / "Fonts/OpenSans-Bold.ttf", .fontSize = 18.0f },
                },
                .createImGuizmoLayer = createInfo.createImGuizmoLayer
            });
        }

        window->Show();
    }

    UniquePtr<VulkanRenderer> VulkanRenderer::Create(const VulkanRendererCreateInfo &createInfo)
    {
        return std::make_unique<VulkanRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void VulkanRenderer::Prepare()
    {
        prepared = true;
        if (imGuiInstance != nullptr) imGuiInstance->BeginNewImGuiFrame();
    }

    void VulkanRenderer::Update()
    {
        // Do full update here
    }

    void VulkanRenderer::Render()
    {
        window->Update();

        Update();

        DrawUI();

        if (!prepared)
        {
            ASSERT_ERROR("Vulkan renderer is not prepared for rendering properly! Make sure you have called renderer->Update() before calling renderer->Render()");
        }

        if (imGuiInstance != nullptr) imGuiInstance->Render();

        // Do rest of rendering here
    }

    /* --- DESTRUCTOR --- */

    void VulkanRenderer::Destroy()
    {
        // Destroy the newest objects first here

        if (imGuiInstance != nullptr) imGuiInstance->Destroy();
        swapchain->Destroy();
    }
}