//
// Created by Nikolay Kanchevski on 1.03.23.
//

#include "ExperimentalVulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan::Renderers
{
    /* --- CONSTRUCTORS --- */

    ExperimentalVulkanRenderer::ExperimentalVulkanRenderer(const VulkanRendererCreateInfo &createInfo)
        : VulkanRenderer(createInfo)
    {
        CreateObjects();
    }

    UniquePtr<ExperimentalVulkanRenderer> ExperimentalVulkanRenderer::Create(const VulkanRendererCreateInfo createInfo)
    {
        return std::make_unique<ExperimentalVulkanRenderer>(createInfo);
    }

    /* --- PRIVATE METHODS --- */

    void ExperimentalVulkanRenderer::CreateObjects()
    {
        // Create shaders to use in pipeline
        auto vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Experimental/Experimental.vert", .shaderType = ShaderType::VERTEX });
        auto fragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Experimental/Experimental.frag", .shaderType = ShaderType::FRAGMENT });

        // Create pipeline
        graphicsPipeline = GraphicsPipeline<>::Create({
            .maxConcurrentFrames = swapchain->GetMaxConcurrentFramesCount(),
            .shaders = { vertexShader, fragmentShader },
            .renderPassInfo = GraphicsPipelineRenderPassInfo {
                .renderPass = &swapchain->GetRenderPass()
            }
        });
    }

    /* --- POLLING METHODS --- */

    void ExperimentalVulkanRenderer::Render()
    {
        VulkanRenderer::Render();

        // Get next swapchain iamge
        swapchain->AcquireNextImage();

        // Get needed data
        auto &commandBuffer = swapchain->GetCurrentCommandBuffer();

        // Begin recording commands
        commandBuffer->Begin();

        // Begin rendering
        swapchain->BeginRenderPass(commandBuffer);

        // Bind pipeline
        graphicsPipeline->Bind(commandBuffer);

        // Draw 3 vertices
        commandBuffer->Draw(3);

        // End rendering
        swapchain->EndRenderPass(commandBuffer);

        // End command recording
        commandBuffer->End();

        // Present to swapchain
        swapchain->SubmitCommandBuffers();
    }

    void ExperimentalVulkanRenderer::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        graphicsPipeline->Destroy();

        VulkanRenderer::Destroy();
    }

}