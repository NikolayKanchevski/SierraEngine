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

    void OnStart() override
    {
        // Create window
        window = GetWindowManager()->CreateWindow({ .title = "Hello, Triangle!" });

        // Create swapchain
        swapchain = GetRenderingContext()->CreateSwapchain({ .name = "Test Swapchain", .window = window, .preferredPresentationMode = SwapchainPresentationMode::VSync });

        // Handle swapchain resizing
        swapchain->OnEvent<SwapchainResizeEvent>([this](const SwapchainResizeEvent &event) {
            GetRenderingContext()->GetDevice().WaitForCommandBuffer(commandBuffers[swapchain->GetCurrentFrame()]);
            imGuiTask->Resize(event.GetSize().x, event.GetSize().y);
            return false;
        });

        // Create ImGui context
        imGuiTask = GetRenderingContext()->CreateImGuiRenderTask({
            .templateImage = swapchain->GetImage(0),
            .scaling = static_cast<float32>(swapchain->GetImage(0)->GetWidth() / window->GetSize().x)
        });

        // Create a command buffer for every concurrent frame
        commandBuffers.resize(swapchain->GetConcurrentFrameCount());
        for (uint32 i = 0; i < swapchain->GetConcurrentFrameCount(); i++)
        {
            commandBuffers[i] = GetRenderingContext()->CreateCommandBuffer({ .name = "General Command Buffer " + std::to_string(i) });
        }
    }

    bool OnUpdate(const TimeStep &timeStep) override
    {
        // Get command buffer for current frame
        auto &commandBuffer = commandBuffers[swapchain->GetCurrentFrame()];

        // Wait until it is no longer in use
        GetRenderingContext()->GetDevice().WaitForCommandBuffer(commandBuffer);

        // Begin recording commands to GPU
        commandBuffer->Begin();

        // Swap out old swapchain image
        swapchain->AcquireNextImage();

        // Begin ImGui frame
        imGuiTask->BeginFrame( window->GetInputManager(), window->GetCursorManager());

        // End ImGui frame
        imGuiTask->EndFrame();

        // Execute ImGui task, which is to render to window
        imGuiTask->Render(commandBuffer, swapchain->GetCurrentImage());

        // Wait until image is written to before presenting it to screen
        commandBuffer->SynchronizeImageUsage(swapchain->GetCurrentImage(), ImageCommandUsage::AttachmentWrite, ImageCommandUsage::Present);

        // End recording commands
        commandBuffer->End();

        // Submit command buffer to GPU
        GetRenderingContext()->GetDevice().SubmitCommandBuffer(commandBuffer);

        // Draw to window
        swapchain->Present(commandBuffer);

        // Update window
        window->OnUpdate();

        return window->IsClosed();
    }

public:
    ~SandboxApplication() override
    {
        // Before deallocating rendering resources, make sure device is not using them
        for (const auto &commandBuffer : commandBuffers)
        {
            GetRenderingContext()->GetDevice().WaitForCommandBuffer(commandBuffer);
        }
    }

};

int main()
{
    // Create and run application
    std::unique_ptr<SandboxApplication> application = std::make_unique<SandboxApplication>(ApplicationCreateInfo{ .name = "Sandbox" });
    application->Run();
    return 0;
}