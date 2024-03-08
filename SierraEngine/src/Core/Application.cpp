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

        // Create render pass
        renderPass = GetRenderingContext().CreateRenderPass({
            .name = "Swapchain Render Pass",
            .attachments = {
                { .templateImage = swapchain->GetImage(0), .type = Sierra::RenderPassAttachmentType::Color }
            },
            .subpassDescriptions = {
                { .renderTargets = { 0 } }
            }
        });

        // Load shaders
        vertexShader = GetRenderingContext().CreateShader({ .name = "Triangle Vertex Shader", .shaderBundlePath = Sierra::File::GetResourcesDirectoryPath() / "shaders/TriangleShader.vert.shader", .shaderType = Sierra::ShaderType::Vertex });
        fragmentShader = GetRenderingContext().CreateShader({ .name = "Triangle Fragment Shader", .shaderBundlePath = Sierra::File::GetResourcesDirectoryPath() / "shaders/TriangleShader.frag.shader", .shaderType = Sierra::ShaderType::Fragment });

        // Create graphics pipeline
        pipelineLayout = GetRenderingContext().CreatePipelineLayout({ .name = "Triangle Graphics Pipeline Layout" });
        graphicsPipeline = GetRenderingContext().CreateGraphicsPipeline({
            .name = "Triangle Graphics Pipeline",
            .vertexShader = vertexShader,
            .fragmentShader = fragmentShader,
            .layout = pipelineLayout,
            .templateRenderPass = renderPass,
            .cullMode = Sierra::CullMode::Back
        });

        // Create a command buffer for every concurrent frame
        commandBuffers.resize(swapchain->GetConcurrentFrameCount());
        for (uint32 i = 0; i < swapchain->GetConcurrentFrameCount(); i++)
        {
            commandBuffers[i] = GetRenderingContext().CreateCommandBuffer({ .name = "General Command Buffer " + std::to_string(i) });
        }

        // Import test texture
        assetManager.ImportTexture(Sierra::File::GetResourcesDirectoryPath() / "assets/Mario.png", [](const AssetID assetID) { APP_INFO("Asset {0} was imported successfully!", assetID.GetHash()); });
    }

    bool Application::Update(const Sierra::TimeStep &timeStep)
    {
        // Get command buffer for current frame
        auto &commandBuffer = commandBuffers[swapchain->GetCurrentFrame()];

        // Wait until it is no longer in use
        GetRenderingContext().GetDevice().WaitForCommandBuffer(commandBuffer);

        // Begin recording commands to GPU
        commandBuffer->Begin();

        // Record asset manager's resources
        assetManager.Update(commandBuffer);

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

}
