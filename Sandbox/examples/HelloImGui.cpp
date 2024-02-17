//
// Created by Nikolay Kanchevski on 8.12.2023.
//

#include <Sierra.h>

//////////////////////////////////////////////////////////////////////////////////////////////
/////                  Simple Cross-Platform ImGui UI Rendering Test                    //////
//////////////////////////////////////////////////////////////////////////////////////////////
class SandboxApplication final : public Application
{
public:
    explicit SandboxApplication(const ApplicationCreateInfo &createInfo)
        : Application(createInfo)
    {
        APP_INFO("Application launched.");
    }

private:
    std::unique_ptr<Window> window = nullptr;
    std::unique_ptr<Swapchain> swapchain = nullptr;

    std::unique_ptr<ImGuiRenderTask> imGuiTask = nullptr;
    std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

    void Start() override
    {
        // Create window
        window = GetWindowManager().CreateWindow({ .title = "Hello, ImGui!" });

        // Create swapchain
        swapchain = GetRenderingContext().CreateSwapchain({ .name = "Test Swapchain", .window = window, .preferredPresentationMode = SwapchainPresentationMode::VSync });

        // Create command buffer to use once to upload ImGui data to GPU (fonts, textures, etc.)
        auto transferCommandBuffer = GetRenderingContext().CreateCommandBuffer({ .name = "Transfer Command Buffer" });
        transferCommandBuffer->Begin();

        // Record ImGui resources
        ImGuiRenderTask::CreateResources(GetRenderingContext(), transferCommandBuffer);
        transferCommandBuffer->End();

        // Submit and wait until resources are live
        GetRenderingContext().GetDevice().SubmitCommandBuffer(transferCommandBuffer);

        // Create ImGui context
        imGuiTask = std::make_unique<ImGuiRenderTask>(GetRenderingContext(), ImGuiRenderTaskCreateInfo {
            .templateImage = swapchain->GetImage(0),
            .scaling = static_cast<float32>(swapchain->GetImage(0)->GetWidth()) / static_cast<float32>(window->GetSize().x)
        });

        // Create a command buffer for every concurrent frame
        commandBuffers.resize(swapchain->GetConcurrentFrameCount());
        for (uint32 i = 0; i < swapchain->GetConcurrentFrameCount(); i++)
        {
            commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = "General Command Buffer " + std::to_string(i) });
        }

        GetRenderingContext().GetDevice().WaitForCommandBuffer(transferCommandBuffer);
    }

    bool Update(const TimeStep &timeStep) override
    {
        // Update ImGui
        imGuiTask->Update(window->GetInputManager(), window->GetCursorManager(), window->GetTouchManager());

        // Draw ImGui
        ImGui::ShowDemoWindow();

        // Get command buffer for current frame
        auto &commandBuffer = commandBuffers[swapchain->GetCurrentFrame()];

        // Wait until it is no longer in use
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);

        // Begin recording commands to GPU
        commandBuffer->Begin();

        // Swap out old swapchain image
        swapchain->AcquireNextImage();

        static bool firstCall = true;
        if (firstCall)
        {
            // Prepare image for writing
            commandBuffer->SynchronizeImageUsage(swapchain->GetCurrentImage(), ImageCommandUsage::None, ImageCommandUsage::ColorWrite);
            firstCall = false;
        }

        // Execute ImGui task, which is to render to window
        imGuiTask->Render(commandBuffer, swapchain->GetCurrentImage());

        // Wait until image is written to before presenting it to screen
        commandBuffer->SynchronizeImageUsage(swapchain->GetCurrentImage(), ImageCommandUsage::ColorWrite, ImageCommandUsage::Present);

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

public:
    ~SandboxApplication() override
    {
        // Before deallocating rendering resources, make sure device is not using them
        for (const auto &commandBuffer : commandBuffers)
        {
            GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);
        }

        ImGuiRenderTask::DestroyResources();
    }

};

int main()
{
    // Create and run application
    std::unique_ptr<SandboxApplication> application = std::make_unique<SandboxApplication>(ApplicationCreateInfo{ .name = "Sandbox" });
    application->Run();
    return 0;
}
