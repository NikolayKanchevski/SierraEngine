//
// Created by Nikolay Kanchevski on 18.02.24.
//

#include "Application.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Application::Application(const Sierra::ApplicationCreateInfo &createInfo)
        : Sierra::Application(createInfo), threadPool({ .threadCount = std::thread::hardware_concurrency() }), assetManager(threadPool, GetRenderingContext(), { .version = Sierra::Version({ 1, 0, 0 }) })
    {
        APP_INFO("Application launched.");
    }

    /* --- POLLING METHODS --- */

    void Application::Start()
    {
        // Create window & swapchain
        window = GetWindowManager().CreateWindow({ .title = "Test Window" });
        swapchain = GetRenderingContext().CreateSwapchain({ .name = "Test Swapchain", .window = window, .preferredPresentationMode = Sierra::SwapchainPresentationMode::VSync });

        // Create resource table
        resourceTable = GetRenderingContext().CreateResourceTable({ .name = "Global Resource Table" });

        // Create a command buffer for every concurrent frame
        commandBuffers.resize(swapchain->GetConcurrentFrameCount());
        for (uint32 i = 0; i < swapchain->GetConcurrentFrameCount(); i++)
        {
            commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = "General Command Buffer " + std::to_string(i) });
        }
    }

    bool Application::Update(const Sierra::TimeStep &timeStep)
    {
        // Get command buffer for current frame
        auto &commandBuffer = commandBuffers[swapchain->GetCurrentFrameIndex()];

        // Wait until it is no longer in use
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);

        // Swap out old swapchain image
        swapchain->AcquireNextImage();

        // Begin recording commands to GPU
        commandBuffer->Begin();

        // Record asset manager's resources
        assetManager.Update(commandBuffer);

        // Bind bindless resource table
        commandBuffer->BindResourceTable(resourceTable);

        static std::once_flag firstTimeFlag;
        std::call_once(firstTimeFlag, [this, &commandBuffer]
        {
            // Create ImGui resources
            Sierra::ImGuiRenderTask::CreateResources(GetRenderingContext(), resourceTable, 0, 0, commandBuffer);
            imGuiTask = std::make_unique<Sierra::ImGuiRenderTask>(GetRenderingContext(), Sierra::ImGuiRenderTaskCreateInfo {
                .templateImage = swapchain->GetImage(0),
                .scaling = swapchain->GetScaling()
            });
        });

        // Prepare swapchain image for writing
        commandBuffer->SynchronizeImageUsage(swapchain->GetCurrentImage(), Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::ColorWrite);

        // Update & draw ImGui demo window
        imGuiTask->Update(window->GetInputManager(), window->GetCursorManager(), window->GetTouchManager());
        ImGui::ShowDemoWindow();

        // Render ImGui
        imGuiTask->Render(commandBuffer, swapchain->GetCurrentImage());

        // Wait until image is written to before presenting it to screen
        commandBuffer->SynchronizeImageUsage(swapchain->GetCurrentImage(), Sierra::ImageCommandUsage::ColorWrite, Sierra::ImageCommandUsage::Present);

        // End recording commands
        commandBuffer->End();

        // Submit command buffer to GPU
        GetRenderingContext().GetDevice().SubmitCommandBuffer(commandBuffer);

        // Draw to window
        swapchain->Present(commandBuffer);

        // Update window
        window->Update();

        return window->IsClosed();
    }

    /* --- DESTRUCTOR --- */

    Application::~Application()
    {
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffers[swapchain->GetCurrentImageIndex()]);
        Sierra::ImGuiRenderTask::DestroyResources();
    }

}
