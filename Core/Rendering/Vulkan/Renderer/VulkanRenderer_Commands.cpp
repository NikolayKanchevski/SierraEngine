//
// Created by Nikolay Kanchevski on 7.10.22.
//

#include "VulkanRenderer.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include "../VulkanCore.h"
#include "../../../../Engine/Classes/Mesh.h"
#include "../../../../Engine/Components/MeshRenderer.h"

using Sierra::Engine::Classes::Mesh;
using Sierra::Engine::Classes::PushConstant;

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateCommandBuffers()
    {
        // Resize the command buffers array
        commandBuffers.resize(maxConcurrentFrames);

        // Set up allocation info
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = device->GetCommandPool();
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = maxConcurrentFrames;

        VK_ASSERT(
            vkAllocateCommandBuffers(device->GetLogicalDevice(), &commandBufferAllocateInfo, commandBuffers.data()),
            "Failed to allocate command buffers"
        );
    }

    void VulkanRenderer::RecordCommandBuffer(const VkCommandBuffer &givenCommandBuffer, const uint32_t imageIndex)
    {
        // Set up buffer begin info
        VkCommandBufferBeginInfo bufferBeginInfo{};
        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bufferBeginInfo.flags = 0;
        bufferBeginInfo.pInheritanceInfo = nullptr;

        // Begin the buffer
        VK_ASSERT(
            vkBeginCommandBuffer(givenCommandBuffer, &bufferBeginInfo),
            "Failed to begin command buffer [" + std::to_string(imageIndex) + "]"
        );

        // Queries must be reset after each individual use.
        vkCmdResetQueryPool(givenCommandBuffer, this->drawTimeQueryPool, imageIndex * 2, 2);

        // Start GPU timer
        vkCmdWriteTimestamp(givenCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, drawTimeQueryPool, imageIndex * 2);

        // Bind the pipeline
        vkCmdBindPipeline(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline);

        // Begin rendering offscreen image
        offscreenRenderer->Begin(givenCommandBuffer, currentFrame);

        VkBuffer vertexBuffers[1];
        VkDescriptorSet descriptorSets[2];
        descriptorSets[0] = bufferDescriptorSets[currentFrame]->GetVulkanDescriptorSet();
        const VkDeviceSize offsets[] = {0 };

        // For each mesh in the world
        auto enttMeshView = World::GetEnttRegistry().view<MeshRenderer>();
        for (auto enttEntity : enttMeshView)
        {
            // Get current mesh
            auto &mesh = enttMeshView.get<MeshRenderer>(enttEntity);
            vertexBuffers[0] = mesh.GetMesh()->GetVertexBuffer()->GetVulkanBuffer();

            // Bind the vertex buffer
            vkCmdBindVertexBuffers(givenCommandBuffer, 0, 1, vertexBuffers, offsets);

            // Bind the index buffer
            vkCmdBindIndexBuffer(givenCommandBuffer, mesh.GetMesh()->GetIndexBuffer()->GetVulkanBuffer(), 0, VK_INDEX_TYPE_UINT32);

            // Get the push constant model of the current mesh and push it to shader
            PushConstant data = mesh.GetPushConstantData();

            // Send push constant data to shader
            vkCmdPushConstants(
                givenCommandBuffer, this->graphicsPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                pushConstantSize, &data
            );

            // Use either bindless or bind*full* descriptor set if not supported
            descriptorSets[1] = VulkanCore::GetDescriptorIndexingSupported() ? globalBindlessDescriptorSet->GetVulkanDescriptorSet() : mesh.GetDescriptorSet();

            vkCmdBindDescriptorSets(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipelineLayout, 0, 2, descriptorSets, 0, nullptr);

            // Draw using the index buffer to prevent vertex re-usage
            vkCmdDrawIndexed(givenCommandBuffer, mesh.GetMesh()->GetIndexCount(), 1, 0, 0, 0);
        }

        // End the offscreen renderer
        offscreenRenderer->End(givenCommandBuffer);

        // Begin the render pass
        swapchainRenderPass->Begin(swapchainFramebuffers[imageIndex], givenCommandBuffer);

        // Set up the viewport
        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = this->swapchainExtent.width;
        viewport.height = this->swapchainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Set up scissor
        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = { this->swapchainExtent.width,  this->swapchainExtent.height };

        // Apply scissoring and viewport update
        vkCmdSetViewport(givenCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(givenCommandBuffer, 0, 1, &scissor);

        // Render ImGui UI
        if (ImGui::GetDrawData() != nullptr) ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), givenCommandBuffer);

        // End the render pass
        swapchainRenderPass->End(givenCommandBuffer);

        // End GPU timer
        vkCmdWriteTimestamp(givenCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, drawTimeQueryPool, imageIndex * 2 + 1);

        // Get rendering time
        FetchRenderTimeResults(imageIndex);

        // End the command buffer and check for errors during command execution
        VK_ASSERT(
            vkEndCommandBuffer(givenCommandBuffer),
            "Failed to end command buffer"
        );
    }

    void VulkanRenderer::FetchRenderTimeResults(const uint32_t swapchainIndex)
    {
        uint64_t buffer[2];
        uint64_t size = sizeof(uint64_t) * 2;

        // Check if draw time query results are available
        VkResult result = vkGetQueryPoolResults(device->GetLogicalDevice(), drawTimeQueryPool, swapchainIndex * 2, 2, size, buffer, sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);
        if (result == VK_NOT_READY)
        {
            return;
        }
        else if (result == VK_SUCCESS)
        {
            // Calculate the difference
            drawTimeQueryResults[swapchainIndex] = ((float)(buffer[1] - buffer[0]) * timestampPeriod);
        }
        else
        {
            ASSERT_ERROR("Failed to receive query results");
        }

        // Calculate final GPU draw time
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            rendererInfo.drawTime += drawTimeQueryResults[i];
        }

        // Get the average of the concurrent draws and convert from nanoseconds to milliseconds
        rendererInfo.drawTime /= (float) maxConcurrentFrames * 1000000.0f;
    }

}