//
// Created by Nikolay Kanchevski on 22.12.22.
//

#include "VulkanRenderer.h"

#include "../VK.h"
#include "../../../Engine/Classes/File.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    VulkanRenderer::VulkanRenderer(const VulkanRendererCreateInfo &createInfo)
        : window(createInfo.window)
    {
        // Create swapchain
        swapchain = Swapchain::Create(window);
        maxConcurrentFrames = VK::GetDevice()->GetMaxConcurrentFramesCount();

        // Create ImGui instance if requested
        if (createInfo.createImGuiInstance)
        {
            imGuiInstance = ImGuiInstance::Create({
                .window = window,
                .swapchain = swapchain,
                .fontCreateInfos = {
                    { .fontFilePath = Engine::File::OUTPUT_FOLDER_PATH + "Fonts/PTSans.ttf", .fontSize = 18.0f },
                    { .fontFilePath = Engine::File::OUTPUT_FOLDER_PATH + "Fonts/OpenSans-Bold.ttf", .fontSize = 18.0f },
                },
                .createImGuizmoLayer = createInfo.createImGuizmoLayer
            });

            hasImGuiInstance = true;
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

        if (hasImGuiInstance) imGuiInstance->BeginNewImGuiFrame();
    }

    void VulkanRenderer::Update()
    {
        /// Do full update here
    }

    void VulkanRenderer::Render()
    {
        window->Update();

        Update();

        DrawUI();

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