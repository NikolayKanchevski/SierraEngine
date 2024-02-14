//
// Created by Nikolay Kanchevski on 8.12.2023.
//

#include <Sierra.h>

//////////////////////////////////////////////////////////////////////////////////////////////
/////                Simple Cross-Platform Multi-API 2D Rendering Test                  //////
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
    std::unique_ptr<RenderPass> renderPass = nullptr;

    std::unique_ptr<Shader> vertexShader = nullptr;
    std::unique_ptr<Shader> fragmentShader = nullptr;

    std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
    std::unique_ptr<GraphicsPipeline> graphicsPipeline = nullptr;
    std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

    void Start() override
    {
        // Create window
        window = GetWindowManager().CreateWindow({ .title = "Hello, Triangle!" });

        // Create swapchain
        swapchain = GetRenderingContext().CreateSwapchain({ .name = "Test Swapchain", .window = window, .preferredPresentationMode = SwapchainPresentationMode::VSync });

        // Create render pass
        renderPass = GetRenderingContext().CreateRenderPass({
            .name = "Swapchain Render Pass",
            .attachments = {
                { .templateImage = swapchain->GetImage(0), .type = RenderPassAttachmentType::Color }
            },
            .subpassDescriptions = {
                { .renderTargets = { 0 } }
            }
        });

        // Load shaders
        vertexShader = GetRenderingContext().CreateShader({ .name = "Triangle Vertex Shader", .shaderBundlePath = File::GetResourcesDirectoryPath() / "shaders/TriangleShader.vert.shader", .shaderType = ShaderType::Vertex });
        fragmentShader = GetRenderingContext().CreateShader({ .name = "Triangle Fragment Shader", .shaderBundlePath = File::GetResourcesDirectoryPath() / "shaders/TriangleShader.frag.shader", .shaderType = ShaderType::Fragment });

        // Create graphics pipeline
        pipelineLayout = GetRenderingContext().CreatePipelineLayout({ .name = "Triangle Graphics Pipeline Layout" });
        graphicsPipeline = GetRenderingContext().CreateGraphicsPipeline({
            .name = "Triangle Graphics Pipeline",
            .vertexShader = vertexShader,
            .fragmentShader = fragmentShader,
            .layout = pipelineLayout,
            .templateRenderPass = renderPass,
            .cullMode = CullMode::Back
        });

        // Create a command buffer for every concurrent frame
        commandBuffers.resize(swapchain->GetConcurrentFrameCount());
        for (uint32 i = 0; i < swapchain->GetConcurrentFrameCount(); i++)
        {
            commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = "General Command Buffer " + std::to_string(i) });
        }
    }

    bool Update(const TimeStep &timeStep) override
    {
        // Get command buffer for current frame
        auto &commandBuffer = commandBuffers[swapchain->GetCurrentFrame()];

        // Wait until it is no longer in use
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);

        // Begin recording commands to GPU
        commandBuffer->Begin();

        // Swap out old swapchain image
        swapchain->AcquireNextImage();

        // Begin rendering to current swapchain image
        commandBuffer->BeginRenderPass(renderPass, { { .image = swapchain->GetCurrentImage() } });

        // Start graphics pipeline
        commandBuffer->BeginGraphicsPipeline(graphicsPipeline);

        // Draw the 3 vertices of the triangle
        commandBuffer->Draw(3);

        // End pipeline
        commandBuffer->EndGraphicsPipeline(graphicsPipeline);

        // End render pass
        commandBuffer->EndRenderPass(renderPass);

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
    }

};

int main()
{
    // Create and run application
    std::unique_ptr<SandboxApplication> application = std::make_unique<SandboxApplication>(ApplicationCreateInfo{ .name = "Sandbox" });
    application->Run();
    return 0;
}