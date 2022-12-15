//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateSynchronization()
    {
        // Resize the semaphores and fences arrays
        imageAvailableSemaphores.resize(maxConcurrentFrames);
        renderFinishedSemaphores.resize(maxConcurrentFrames);
        frameBeingRenderedFences.resize(maxConcurrentFrames);

        // Define the semaphores creation info (universal for all semaphores)
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        // Define the fences creation info (universal for all fences)
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        // Create semaphores and fences
        for (uint32_t i = maxConcurrentFrames; i--;)
        {
            vkCreateSemaphore(device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]);
            vkCreateSemaphore(device->GetLogicalDevice(), &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]);
            vkCreateFence(device->GetLogicalDevice(), &fenceCreateInfo, nullptr, &frameBeingRenderedFences[i]);
        }
    }

    void VulkanRenderer::Draw()
    {
        // Wait for the fences to be signalled
        vkWaitForFences(device->GetLogicalDevice(), 1, &frameBeingRenderedFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

        // Get the current swapchain image
        uint32_t imageIndex;
        VkResult imageAcquireResult = vkAcquireNextImageKHR(device->GetLogicalDevice(), this->swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (imageAcquireResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchainObjects();
            return;
        }

        // Reset the fences
        vkResetFences(device->GetLogicalDevice(), 1, &frameBeingRenderedFences[currentFrame]);

        // Update uniform & storage buffers
        UpdateShaderBuffers(imageIndex);

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
        VK_ASSERT(
            vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, frameBeingRenderedFences[currentFrame]),
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
        VkResult queuePresentResult = vkQueuePresentKHR(device->GetPresentationQueue(), &presentInfo);
        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR || window.IsResized())
        {
            RecreateSwapchainObjects();
            return;
        }

        // Increment the current frame whilst capping it to "maxConcurrentFrames"
        currentFrame = (currentFrame + 1) % maxConcurrentFrames;
    }

}