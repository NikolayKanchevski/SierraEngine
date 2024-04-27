//
// Created by Nikolay Kanchevski on 18.02.24.
//

#include "Application.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Application::Application(const Sierra::ApplicationCreateInfo &createInfo)
        : Sierra::Application(createInfo), threadPool({ .threadCount = std::thread::hardware_concurrency() }), assetManager({ .threadPool = threadPool, .fileManager = GetFileManager(), .renderingContext = GetRenderingContext() })
    {
        APP_INFO("Application launched.");
    }

    /* --- POLLING METHODS --- */

    void Application::Start()
    {
        // Create window & swapchain
        window = GetWindowManager().CreateWindow({ .title = "Test Window", .resizable = true, .maximize = true });
        swapchain = GetRenderingContext().CreateSwapchain({ .name = "Test Swapchain", .window = window, .preferredPresentationMode = Sierra::SwapchainPresentationMode::VSync });
        swapchain->OnEvent<Sierra::SwapchainResizeEvent>([this](const auto event) -> bool { imGuiTask->Resize(event.GetWidth(), event.GetHeight(), event.GetScaling()); return false; });

        // Create resource table
        resourceTable = GetRenderingContext().CreateResourceTable({ .name = "Global Resource Table" });

        // Create a command buffer for every concurrent frame
        commandBuffers.resize(swapchain->GetConcurrentFrameCount());
        for (uint32 i = 0; i < swapchain->GetConcurrentFrameCount(); i++)
        {
            commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = "General Command Buffer [" + std::to_string(i) + "]" });
        }
    }

    bool Application::Update(const Sierra::TimeStep&)
    {
        // Get command buffer for current frame
        std::unique_ptr<Sierra::CommandBuffer> &commandBuffer = commandBuffers[swapchain->GetCurrentFrameIndex()];

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
        std::call_once(firstTimeFlag, [this, &commandBuffer]() -> void
        {
            if (std::optional<Sierra::File> fontFile = GetFileManager().OpenFile(GetFileManager().GetResourcesDirectoryPath() / "core/assets/fonts/Lato.ttf"); fontFile.has_value())
            {
                // Create ImGui resources
                imGuiTask = std::make_unique<Sierra::ImGuiRenderTask>(Sierra::ImGuiRenderTaskCreateInfo {
                    .renderingContext = GetRenderingContext(),
                    .commandBuffer = commandBuffer,
                    .scaling = swapchain->GetScaling(),
                    .templateOutputImage = swapchain->GetImage(0),
                    .fontAtlasIndex = 0,
                    .fontCreateInfos = { { .ttfMemory = fontFile.value().Read() } },
                    .fontSamplerIndex = 0,
                    .resourceTable = resourceTable
                });
            }
            else
            {
                APP_ERROR("Failed not load Lato.ttf font file!");
            }
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
    }

}
