//
// Created by Nikolay Kanchevski on 8.12.2023.
//

#include <Sierra.h>

//////////////////////////////////////////////////////////////////////////////////////////////
/////                   Simple Cross-Platform Multi-API Rendering Test                  //////
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
    std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

    void OnStart() override
    {
        // Create window
        window = GetWindowManager().CreateWindow({ .title = "My Window :)" });

        // Create swapchain
        swapchain = GetRenderingContext().CreateSwapchain({ .name = "Test Swapchain", .window = window, .preferredPresentationMode = SwapchainPresentationMode::VSync });

        // Create a command buffer for every concurrent frame
        commandBuffers.resize(swapchain->GetConcurrentFrameCount());
        for (uint32 i = swapchain->GetConcurrentFrameCount(); i--;)
        {
            commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = "Command Buffer " + std::to_string(i) });
        }
    }

    bool OnUpdate(const TimeStep &timeStep) override
    {
        // Begin recording GPU commands on current command buffer
        auto &commandBuffer = commandBuffers[swapchain->GetCurrentFrame()];
        commandBuffer->Begin();

        // Swap out swapchain images and begin rendering to window
        swapchain->Begin(commandBuffer);

        // End rendering to window
        swapchain->End(commandBuffer);

        // End recording commands
        commandBuffer->End();

        // Draw to window
        swapchain->SubmitCommandBufferAndPresent(commandBuffer);

        // Update window
        window->OnUpdate();

        return window->IsClosed();
    }

public:
    ~SandboxApplication()
    {
        for (const auto &commandBuffer : commandBuffers) commandBuffer->Destroy();
        swapchain->Destroy();
    }

};

int main()
{
    // Create and run application
    std::unique_ptr<SandboxApplication> application = std::make_unique<SandboxApplication>(ApplicationCreateInfo{ .name = "Sandbox" });
    application->Run();
    return 0;
}
