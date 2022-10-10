//
// Created by Nikolay Kanchevski on 7.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateCommandPool()
    {
        // Set up the command pool creation info
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

        // Create the command pool
        VulkanDebugger::CheckResults(
            vkCreateCommandPool(this->logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool),
            "Failed to create command pool"
        );

        // Assign the EngineCore's command pool
        VulkanCore::SetCommandPool(commandPool);
    }

    void VulkanRenderer::CreateCommandBuffers()
    {
        // Resize the command buffers array
        commandBuffers.resize(MAX_CONCURRENT_FRAMES);

        // Set up allocation info
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = commandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = MAX_CONCURRENT_FRAMES;

        VulkanDebugger::CheckResults(
            vkAllocateCommandBuffers(this->logicalDevice, &commandBufferAllocateInfo, commandBuffers.data()),
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
        VulkanDebugger::CheckResults(
            vkBeginCommandBuffer(givenCommandBuffer, &bufferBeginInfo),
            "Failed to begin command buffer [" + std::to_string(imageIndex) + "]"
        );

        // Queries must be reset after each individual use.
        vkCmdResetQueryPool(givenCommandBuffer, this->drawTimeQueryPool, imageIndex * 2, 2);

        // Start GPU timer
        vkCmdWriteTimestamp(givenCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, drawTimeQueryPool, imageIndex * 2);

        // Begin the render pass
        renderPass->SetFramebuffer(swapchainFramebuffers[imageIndex]);
        renderPass->Begin(givenCommandBuffer);

        // Bind the pipeline
        vkCmdBindPipeline(givenCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline);

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
        scissor.extent = this->swapchainExtent;

        vkCmdSetViewport(givenCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(givenCommandBuffer, 0, 1, &scissor);

//        ulong* offsets = stackalloc ulong[] { 0 };
//        VkBuffer* vertexBuffers = stackalloc VkBuffer[1];
//        VkDescriptorSet* descriptorSetsPtr = stackalloc VkDescriptorSet[3];
//
//        foreach (var mesh in World.meshes)
//        {
//            // Define a pointer to the vertex buffer
//            vertexBuffers[0] = mesh.GetVertexBuffer();
//
//            // Bind the vertex buffer
//            VulkanNative.vkCmdBindVertexBuffers(givenCommandBuffer, 0, 1, vertexBuffers, offsets);
//
//            // Bind the index buffer
//            VulkanNative.vkCmdBindIndexBuffer(givenCommandBuffer, mesh.GetIndexBuffer(), 0, VkIndexType.VK_INDEX_TYPE_UINT32);
//
//            // Get the push constant model of the current mesh and push it to shader
//            PushConstant pushConstantData = mesh.GetPushConstantData();
//            // FragmentPushConstant fragmentPushConstantData = mesh.GetFragmentPushConstantData();
//
//            VulkanNative.vkCmdPushConstants(
//                    givenCommandBuffer, this.graphicsPipelineLayout,
//                    VkShaderStageFlags.VK_SHADER_STAGE_VERTEX_BIT | VkShaderStageFlags.VK_SHADER_STAGE_FRAGMENT_BIT, 0,
//                    pushConstantSize, &pushConstantData
//            );
//
//            descriptorSetsPtr[0] = uniformDescriptorSets[currentFrame];
//            descriptorSetsPtr[1] = diffuseTextures[mesh.diffuseTextureID].descriptorSet;
//            descriptorSetsPtr[2] = specularTextures[mesh.specularTextureID].descriptorSet;
//
//            VulkanNative.vkCmdBindDescriptorSets(givenCommandBuffer, VkPipelineBindPoint.VK_PIPELINE_BIND_POINT_GRAPHICS, this.graphicsPipelineLayout, 0, 3, descriptorSetsPtr, 0, null);
//
//            // Draw using the index buffer to prevent vertex re-usage
//            VulkanNative.vkCmdDrawIndexed(givenCommandBuffer, mesh.indexCount, 1, 0, 0, 0);
//        }

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), givenCommandBuffer);

        // End the render pass
        renderPass->End(givenCommandBuffer);

        // End GPU timer
        vkCmdWriteTimestamp(givenCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, drawTimeQueryPool, imageIndex * 2 + 1);

        FetchRenderTimeResults(imageIndex);

        // End the command buffer and check for errors during command execution
        VulkanDebugger::CheckResults(
            vkEndCommandBuffer(givenCommandBuffer),
            "Failed to end command buffer"
        );
    }

    void VulkanRenderer::FetchRenderTimeResults(const uint32_t swapchainIndex)
    {
        std::vector<uint64_t> buffer(2);

        uint64_t size = sizeof(uint64_t) * 2;

        // Check if draw time query results are available
        VkResult result = vkGetQueryPoolResults(this->logicalDevice, drawTimeQueryPool, swapchainIndex * 2, 2, size, buffer.data(), sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);
        if (result == VK_NOT_READY)
        {
            return;
        }
        else if (result == VK_SUCCESS)
        {
            // Calculate the difference
            drawTimeQueryResults[swapchainIndex] = (buffer[1] - buffer[0]) * timestampPeriod;
        }
        else
        {
            VulkanDebugger::ThrowError("Failed to receive query results");
        }

        // Calculate final GPU draw time
        for (int i = MAX_CONCURRENT_FRAMES; i--;)
        {
            this->drawTime += drawTimeQueryResults[i];
        }
        this->drawTime /= MAX_CONCURRENT_FRAMES * 1000000.0f;
    }

}