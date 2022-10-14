//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateSynchronization()
    {
        // Resize the semaphores and fences arrays
        imageAvailableSemaphores.resize(MAX_CONCURRENT_FRAMES);
        renderFinishedSemaphores.resize(MAX_CONCURRENT_FRAMES);
        frameBeingRenderedFences.resize(MAX_CONCURRENT_FRAMES);

        // Define the semaphores creation info (universal for all semaphores)
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        // Define the fences creation info (universal for all fences)
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // Create semaphores and fences
        for (int i = MAX_CONCURRENT_FRAMES; i--;)
        {
            vkCreateSemaphore(this->logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]);
            vkCreateSemaphore(this->logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]);
            vkCreateFence(this->logicalDevice, &fenceCreateInfo, nullptr, &frameBeingRenderedFences[i]);
        }
    }

    void VulkanRenderer::Draw()
    {
        // Wait for the fences to be signalled
        vkWaitForFences(this->logicalDevice, 1, &frameBeingRenderedFences[currentFrame], VK_TRUE, MAX_UINT64_T);

        // Get the current swapchain image
        uint32_t imageIndex;
        VkResult imageAcquireResult = vkAcquireNextImageKHR(this->logicalDevice, this->swapchain, MAX_UINT64_T, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (imageAcquireResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchainObjects();
            return;
        }

        // Reset the fences
        vkResetFences(this->logicalDevice, 1, &frameBeingRenderedFences[currentFrame]);

        UpdateUniformBuffers(imageIndex);

        // Reset and re-record the command buffer
        vkResetCommandBuffer(this->commandBuffers[currentFrame], 0);
        this->RecordCommandBuffer(this->commandBuffers[currentFrame], imageIndex);

        const std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        // Set up the submitting info
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
        submitInfo.pWaitDstStageMask = waitStages.data();

        // Submit the queue
        VulkanDebugger::CheckResults(
            vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameBeingRenderedFences[currentFrame]),
            "Failed to submit graphics queue"
        );

        // Set up presentation info
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &this->swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        // Present
        VkResult queuePresentResult = vkQueuePresentKHR(this->presentQueue, &presentInfo);
        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || window.IsResized())
        {
            RecreateSwapchainObjects();
        }

        // Increment the current frame whilst capping it to "MAX_CONCURRENT_FRAMES"
        currentFrame = (currentFrame + 1) % MAX_CONCURRENT_FRAMES;
    }

}