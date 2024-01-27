//
// Created by Nikolay Kanchevski on 8.12.2023.
//

#include <Sierra.h>

//////////////////////////////////////////////////////////////////////////////////////////////
/////                Simple Cross-Platform Multi-API 3D Rendering Test                  //////
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

    std::unique_ptr<Image> depthBuffer = nullptr;
    std::unique_ptr<RenderPass> renderPass = nullptr;

    std::unique_ptr<Shader> vertexShader = nullptr;
    std::unique_ptr<Shader> fragmentShader = nullptr;

    std::unique_ptr<PipelineLayout> pipelineLayout = nullptr;
    std::unique_ptr<GraphicsPipeline> graphicsPipeline = nullptr;

    std::unique_ptr<Buffer> uniformBuffer = nullptr;
    struct UniformData { Matrix4x4 view; Matrix4x4 projection; };
    struct PushConstantData { Matrix4x4 model; };

    std::unique_ptr<Buffer> vertexBuffer = nullptr;
    constexpr static std::array<Vector3, 8> CUBE_VERTICES = { Vector3(-1, -1, -1), Vector3(1, -1, -1), Vector3(1, 1, -1), Vector3(-1, 1, -1), Vector3(-1, -1, 1), Vector3(1, -1, 1), Vector3(1, 1, 1), Vector3(-1, 1, 1) };

    std::unique_ptr<Buffer> indexBuffer = nullptr;
    constexpr static std::array<uint32, 36> CUBE_INDICES = { 1, 5, 0, 0, 5, 4, 6, 2, 7, 7, 2, 3, 3, 0, 7, 7, 0, 4, 7, 4, 6, 6, 4, 5, 6, 5, 2, 2, 5, 1, 2, 1, 3, 3, 1, 0 };

    std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;
    const TimePoint startTime = TimePoint::Now();

    const Vector3 CAMERA_POSITION = { 4.0f, 4.0f, 4.0f };
    const float32 CAMERA_FOV = 45.0f;
    const float32 ROTATION_SPEED = 0.0003f;

    void OnStart() override
    {
        // Create window and swapchain for it
        window = GetWindowManager().CreateWindow({ .title = "Hello, Cube!", .resizable = false });
        swapchain = GetRenderingContext().CreateSwapchain({ .name = "Cube Swapchain", .window = window, .preferredPresentationMode = SwapchainPresentationMode::VSync });

        // Create depth image to properly order 3D meshes
        depthBuffer = GetRenderingContext().CreateImage({
            .name = "Depth Buffer Image",
            .width = swapchain->GetWidth(),
            .height = swapchain->GetHeight(),
            .format = GetRenderingContext().GetDevice().GetSupportedImageFormat(ImageChannels::D, ImageMemoryType::UNorm16, ImageUsage::DepthAttachment),
            .usage = ImageUsage::DepthAttachment,
            .memoryLocation = ImageMemoryLocation::Device
        });

        // Create render pass
        renderPass = GetRenderingContext().CreateRenderPass({
            .name = "Swapchain Render Pass",
            .attachments = {
                { .templateImage = swapchain->GetImage(0), .type = RenderPassAttachmentType::Color },
                { .templateImage = depthBuffer, .type = RenderPassAttachmentType::Depth }
            },
            .subpassDescriptions = {
                { .renderTargets = { 0, 1 } }
            }
        });

        // Load shaders
        vertexShader = GetRenderingContext().CreateShader({ .name = "Cube Vertex Shader", .shaderBundlePath = GetResourcesDirectoryPath() / "shaders/CubeShader.vert.shader", .shaderType = ShaderType::Vertex });
        fragmentShader = GetRenderingContext().CreateShader({ .name = "Cube Fragment Shader", .shaderBundlePath = GetResourcesDirectoryPath() / "shaders/CubeShader.frag.shader", .shaderType = ShaderType::Fragment });

        // Define pipeline layout
        pipelineLayout = GetRenderingContext().CreatePipelineLayout({
            .name = "Cube Graphics Pipeline Layout",
            .bindings = { { .type = PipelineBindingType::UniformBuffer } },
            .pushConstantSize = sizeof(PushConstantData)
        });

        // Create graphics pipeline
        graphicsPipeline = GetRenderingContext().CreateGraphicsPipeline({
            .name = "Cube Graphics Pipeline",
            .vertexInputs = { VertexInput::Position3D },
            .vertexShader = vertexShader,
            .fragmentShader = fragmentShader,
            .layout = pipelineLayout,
            .renderPass = renderPass,
            .shadeMode = ShadeMode::Wireframe
        });

        // Create a command buffer for every concurrent frame
        commandBuffers.resize(swapchain->GetConcurrentFrameCount());
        for (uint32 i = 0; i < swapchain->GetConcurrentFrameCount(); i++)
        {
            commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = "General Command Buffer " + std::to_string(i) });
        }

        // Create uniform buffer to hold view data
        uniformBuffer = GetRenderingContext().CreateBuffer({
            .name = "Uniform Buffer",
            .memorySize = sizeof(UniformData),
            .usage = BufferUsage::Uniform,
            .memoryLocation = BufferMemoryLocation::CPU
        });

        // Create a temporary command buffer to use to upload resources to GPU
        auto transferCommandBuffer = GetRenderingContext().CreateCommandBuffer({ .name = "Transfer Command Buffer" });
        transferCommandBuffer->Begin();

        // Create staging buffer on CPU side to write data to, to then copy its memory to the actual GPU-side vertex buffer
        auto vertexStagingBuffer = GetRenderingContext().CreateBuffer({
            .name = "Cube Vertex Staging Buffer",
            .memorySize = sizeof(CUBE_VERTICES),
            .usage = BufferUsage::Vertex | BufferUsage::SourceTransfer,
            .memoryLocation = BufferMemoryLocation::CPU
        });

        // Create final vertex buffer on the GPU
        vertexBuffer = GetRenderingContext().CreateBuffer({
            .name = "Cube Vertex Buffer",
            .memorySize = sizeof(CUBE_VERTICES),
            .usage = BufferUsage::Vertex | BufferUsage::DestinationTransfer,
            .memoryLocation = BufferMemoryLocation::GPU
        });

        // Copy vertex data to staging buffer and record a buffer copy operation
        vertexStagingBuffer->CopyFromMemory(CUBE_VERTICES.data(), CUBE_VERTICES.size() * sizeof(Vector3));
        transferCommandBuffer->CopyBufferToBuffer(vertexStagingBuffer, vertexBuffer);

        // Create staging buffer on CPU side to write data to, to then copy its memory to the actual GPU-side index buffer
        auto indexStagingBuffer = GetRenderingContext().CreateBuffer({
            .name = "Cube Index Staging Buffer",
            .memorySize = sizeof(CUBE_INDICES),
            .usage = BufferUsage::Index | BufferUsage::SourceTransfer,
            .memoryLocation = BufferMemoryLocation::CPU
        });

        // Create final index buffer on the GPU
        indexBuffer = GetRenderingContext().CreateBuffer({
            .name = "Cube Index Buffer",
            .memorySize = sizeof(CUBE_INDICES),
            .usage = BufferUsage::Index | BufferUsage::DestinationTransfer,
            .memoryLocation = BufferMemoryLocation::GPU
        });

        // Copy index data to staging buffer and record a buffer copy operation
        indexStagingBuffer->CopyFromMemory(CUBE_INDICES.data(), CUBE_INDICES.size() * sizeof(uint32));
        transferCommandBuffer->CopyBufferToBuffer(indexStagingBuffer, indexBuffer);

        // Submit command buffer
        transferCommandBuffer->End();
        GetRenderingContext().GetDevice().SubmitCommandBuffer(transferCommandBuffer);

        // Wait on CPU until it has finished execution, so staging resources are not released early by destructors
        GetRenderingContext().GetDevice().WaitForCommandBuffer(transferCommandBuffer);
    }

    bool OnUpdate(const TimeStep &timeStep) override
    {
        // Update uniform buffer's data
        UniformData uniformData = { };
        uniformData.view = glm::lookAt(CAMERA_POSITION, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        uniformData.projection = glm::perspective(glm::radians(CAMERA_FOV), static_cast<float32>(swapchain->GetWidth()) / static_cast<float32>(swapchain->GetHeight()), 0.1f, 10.0f);
        uniformBuffer->CopyFromMemory(&uniformData, sizeof(UniformData));

        // Calculate cube's rotated model matrix
        PushConstantData pushConstantData = { };
        pushConstantData.model = glm::rotate(Matrix4x4(1.0f), static_cast<float32>(std::fmod((TimePoint::Now() - startTime) * ROTATION_SPEED, 360.0f)), { 0.0f, 1.0f, 0.0f });

        // Get command buffer for current frame
        auto &commandBuffer = commandBuffers[swapchain->GetCurrentFrame()];

        // Wait until it is no longer in use
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);

        // Begin recording commands to GPU
        commandBuffer->Begin();

        // Swap out old swapchain image
        swapchain->AcquireNextImage();

        // Make sure we have finished copying data to buffers before reading from them (only perform this the first time)
        static std::once_flag onceFlag;
        std::call_once(onceFlag, [this, &commandBuffer] {
            commandBuffer->SynchronizeBufferUsage(vertexBuffer, BufferCommandUsage::MemoryWrite, BufferCommandUsage::VertexRead);
            commandBuffer->SynchronizeBufferUsage(indexBuffer, BufferCommandUsage::MemoryWrite, BufferCommandUsage::IndexRead);
        });

        // Make sure we have finished writing to depth buffer before writing again
        commandBuffer->SynchronizeImageUsage(depthBuffer, ImageCommandUsage::DepthWrite, ImageCommandUsage::DepthWrite);

        // Begin rendering to current swapchain image
        commandBuffer->BeginRenderPass(renderPass, { { .image = swapchain->GetCurrentImage() }, { .image = depthBuffer } });

        // Start graphics pipeline
        commandBuffer->BeginGraphicsPipeline(graphicsPipeline);

        // Bind mesh data
        commandBuffer->BindVertexBuffer(vertexBuffer);
        commandBuffer->BindIndexBuffer(indexBuffer);

        // Bind view settings
        commandBuffer->BindBuffer(0, uniformBuffer);
        commandBuffer->PushConstants(&pushConstantData, sizeof(PushConstantData));

        // Draw the vertices of the cube
        commandBuffer->DrawIndexed(CUBE_INDICES.size());

        // End pipeline
        commandBuffer->EndGraphicsPipeline(graphicsPipeline);

        // End render pass
        commandBuffer->EndRenderPass(renderPass);

        // Wait until image is written to before presenting it to screen
        commandBuffer->SynchronizeImageUsage(swapchain->GetCurrentImage(), ImageCommandUsage::AttachmentWrite, ImageCommandUsage::Present);

        // End recording commands
        commandBuffer->End();

        // Submit command buffer to GPU
        GetRenderingContext().GetDevice().SubmitCommandBuffer(commandBuffer);

        // Draw to window
        swapchain->Present(commandBuffer);

        // Flush window changes
        window->OnUpdate();

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