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
        // Create shaders to use in pipeline
        auto vertexShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Experimental/Experimental.vert", .shaderType = ShaderType::VERTEX });
        auto fragmentShader = Shader::Create({ .filePath = File::OUTPUT_FOLDER_PATH + "Shaders/Experimental/Experimental.frag", .shaderType = ShaderType::FRAGMENT });

        // Create pipeline
        graphicsPipeline = GraphicsPipeline<>::Create({
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

        // Get the command buffer of current frame
        auto &commandBuffer = swapchain->GetCurrentCommandBuffer();

        // Begin recording commands to send to GPU
        commandBuffer->Begin();

        // Begin rendering
        swapchain->BeginRenderPass(commandBuffer);

        // Bind pipeline
        graphicsPipeline->Bind(commandBuffer);

        // Draw 3 vertices
        graphicsPipeline->Draw(commandBuffer, 3);

        // End rendering
        swapchain->EndRenderPass(commandBuffer);

        // End command recording
        commandBuffer->End();

        // Submit commands & present to swapchain
        swapchain->SubmitCommandBuffers();
    }

    void ExperimentalVulkanRenderer::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        graphicsPipeline->Destroy();

        VulkanRenderer::Destroy();
    }

}







