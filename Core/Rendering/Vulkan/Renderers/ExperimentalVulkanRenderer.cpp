//
// Created by Nikolay Kanchevski on 1.03.23.
//

#include "ExperimentalVulkanRenderer.h"

#include "../VK.h"
#include "../../../Engine/Classes/File.h"

namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    ExperimentalVulkanRenderer::ExperimentalVulkanRenderer(const VulkanRendererCreateInfo &createInfo)
        : VulkanRenderer(createInfo)
    {
        // Create shaders to use in pipeline
        auto vertexShader = Shader::Create({ .filePath = Engine::File::OUTPUT_FOLDER_PATH + "Shaders/Experimental/Experimental.vert", .shaderType = ShaderType::VERTEX });
        auto fragmentShader = Shader::Create({ .filePath = Engine::File::OUTPUT_FOLDER_PATH + "Shaders/Experimental/Experimental.frag", .shaderType = ShaderType::FRAGMENT });

        // Create pipeline
        graphicsPipeline = GraphicsPipeline::Create({
            .shaders = { vertexShader, fragmentShader },
            .renderPassInfo = GraphicsPipelineRenderPassInfo {
                .renderPass = swapchain->GetRenderPass()
            }
        });
    }

    UniquePtr<ExperimentalVulkanRenderer> ExperimentalVulkanRenderer::Create(VulkanRendererCreateInfo createInfo)
    {
        return std::make_unique<ExperimentalVulkanRenderer>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void ExperimentalVulkanRenderer::Render()
    {
        VulkanRenderer::Render();

        // Get next swapchain image
        swapchain->AcquireNextImage();

        // Get the command buffer of current frame
        auto &commandBuffer = swapchain->GetCurrentCommandBuffer();

        // Begin recording commands to send to GPU
        commandBuffer->Begin(CommandBufferUsage::ONE_TIME_SUBMIT);

        // Begin rendering
        swapchain->BeginRenderPass(commandBuffer);

        // Bind pipeline
        graphicsPipeline->Bind(commandBuffer);

        // Draw 3 vertices
        graphicsPipeline->Draw(commandBuffer, 3);

        // End pipeline
        graphicsPipeline->End(commandBuffer);

        // End rendering
        swapchain->EndRenderPass(commandBuffer);

        // End command recording
        commandBuffer->End();

        // Submit commands & present to swapchain
        swapchain->SwapImage();
    }

    void ExperimentalVulkanRenderer::Destroy()
    {
        VK::GetDevice()->WaitUntilIdle();

        graphicsPipeline->Destroy();

        VulkanRenderer::Destroy();
    }

}







