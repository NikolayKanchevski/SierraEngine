//
// Created by Nikolay Kanchevski on 12.12.22.
//

#include "RenderPass.h"

#include "../VK.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    RenderPass::RenderPass(const RenderPassCreateInfo &createInfo)
    {
        const std::vector<RenderPassAttachment> &attachments = createInfo.attachments;
        const std::vector<SubpassInfo> &subpassInfos = createInfo.subpassInfos;

        std::vector<VkAttachmentDescription> attachmentDescriptions(attachments.size());

        for (uint32_t i = attachments.size(); i--;)
        {
            attachmentDescriptions[i].format = (VkFormat) attachments[i].imageAttachment->GetFormat();
            attachmentDescriptions[i].samples = (VkSampleCountFlagBits) attachments[i].imageAttachment->GetSampling();
            attachmentDescriptions[i].loadOp = (VkAttachmentLoadOp) attachments[i].loadOp;
            attachmentDescriptions[i].storeOp = (VkAttachmentStoreOp) attachments[i].storeOp;
            attachmentDescriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;      /// NOTE: These lines may need to be changed
            attachmentDescriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescriptions[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            attachmentDescriptions[i].finalLayout = (VkImageLayout) attachments[i].finalLayout;
        }

        std::vector<SubpassDescription> subpassDescriptions{};
        std::vector<VkAttachmentReference> depthReferences;

        for (uint32_t i = subpassInfos.size(); i--;)
        {
            SubpassDescription subpassDescription{};
            subpassDescription.data.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpassDescription.data.colorAttachmentCount = 0;
            subpassDescription.data.inputAttachmentCount = 0;

            bool hasDepth = false;
            bool hasColor = false;
            bool hasInput = false;
            bool hasResolve = false;

            for (auto& attachment : attachments)
            {
                if (attachment.IsDepth())
                {
                    hasDepth = true;
                }
                else if (attachment.isResolve)
                {
                    hasResolve = true;
                }

                if (hasDepth && hasResolve) break;
            }

            for (uint32_t j = subpassInfos[i].renderTargets.size(); j--;)
            {
                uint32_t renderTarget = subpassInfos[i].renderTargets[j];

                if (attachments[renderTarget].IsDepth())
                {
                    subpassDescription.depthReference.attachment = renderTarget;
                    subpassDescription.depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                    hasDepth = true;
                }
                else if (attachments[renderTarget].isResolve)
                {
                    VkAttachmentReference resolveReference{};
                    resolveReference.attachment = renderTarget;
                    resolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    subpassDescription.resolveReferences.push_back(resolveReference);
                }
                else
                {
                    VkAttachmentReference colorReference{};
                    colorReference.attachment = renderTarget;
                    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    subpassDescription.colorReferences.push_back(colorReference);

                    hasColor = true;
                }
            }

            for (uint32_t j = subpassInfos[i].subpassInputs.size(); j--;)
            {
                uint32_t renderTarget = subpassInfos[i].subpassInputs[j];

                if (attachments[renderTarget].IsDepth())
                {
                    subpassDescription.depthReference.attachment = renderTarget;
                    subpassDescription.depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                    hasDepth = true;
                }
                else
                {
                    VkAttachmentReference inputReference{};
                    inputReference.attachment = renderTarget;
                    inputReference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    subpassDescription.inputReferences.push_back(inputReference);

                    hasInput = true;
                }
            }

            if (hasDepth)
            {
                depthReferences.push_back(subpassDescription.depthReference);
                subpassDescription.data.pDepthStencilAttachment = &depthReferences.back();
            }

            if (hasColor)
            {
                subpassDescription.data.pColorAttachments = subpassDescription.colorReferences.data();
                subpassDescription.data.colorAttachmentCount = subpassDescription.colorReferences.size();
            }

            if (hasResolve)
            {
                subpassDescription.data.pResolveAttachments = subpassDescription.resolveReferences.data();
            }

            if (hasInput)
            {
                subpassDescription.data.pInputAttachments = subpassDescription.inputReferences.data();
                subpassDescription.data.inputAttachmentCount = subpassDescription.inputReferences.size();
            }

            subpassDescriptions.push_back(std::move(subpassDescription));
        }

        std::vector<VkSubpassDependency> dependencies;
        if (subpassInfos.size() != 1)
        {
            dependencies.resize(subpassInfos.size() + 1);

            VkSubpassDependency& firstDependency = dependencies[0];
            firstDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            firstDependency.dstSubpass = 0;
            firstDependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            firstDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            firstDependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            firstDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            firstDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            for (size_t i = 1; i < (dependencies.size() - 1); i++)
            {
                dependencies[i].srcSubpass = i-1;
                dependencies[i].dstSubpass = i;
                dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dependencies[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
            }

            VkSubpassDependency& lastDependency = *(dependencies.end() - 1);
            lastDependency.srcSubpass = subpassInfos.size() - 1;
            lastDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
            lastDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            lastDependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            lastDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            lastDependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            lastDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        }
        else
        {
            dependencies.resize(2);
            dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
            dependencies[0].dstSubpass = 0;
            dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependencies[0].srcAccessMask = 0;
            dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            dependencies[1].srcSubpass = 0;
            dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
            dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        }

        std::vector<VkSubpassDescription> vkSubpassDescriptions{};
        for (uint32_t i = subpassDescriptions.size(); i--;)
        {
            vkSubpassDescriptions.push_back(subpassDescriptions[i].data);
        }

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
        renderPassInfo.pAttachments = attachmentDescriptions.data();
        renderPassInfo.subpassCount = static_cast<uint32_t>(vkSubpassDescriptions.size());
        renderPassInfo.pSubpasses = vkSubpassDescriptions.data();
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();

        VK_ASSERT(vkCreateRenderPass(VK::GetLogicalDevice(), &renderPassInfo, nullptr, &renderPass), "Could not create render pass");

        clearValues.resize(attachments.size());
        for (uint32_t i = 0; i < attachments.size(); i++)
        {
            if (attachments[i].IsDepth())
            {
                clearValues[i].depthStencil = {1.0f, 0};
            }
            else
            {
                clearValues[i].color = {0.31f, 0.31f, 0.31f, 1.0f};
            }
        }
    }

    std::unique_ptr<RenderPass> RenderPass::Create(const RenderPassCreateInfo renderPassCreateInfo)
    {
        return std::make_unique<RenderPass>(renderPassCreateInfo);
    }

    /* --- POLLING METHODS --- */

    void RenderPass::NextSubpass(const VkCommandBuffer commandBuffer)
    {
        vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderPass::Begin(const std::unique_ptr<Framebuffer> &framebuffer, const VkCommandBuffer commandBuffer)
    {
        // Set up begin info
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = framebuffer->GetVulkanFramebuffer();
        renderPassBeginInfo.renderArea.extent.width = framebuffer->GetWidth();
        renderPassBeginInfo.renderArea.extent.height = framebuffer->GetHeight();
        renderPassBeginInfo.clearValueCount = clearValues.size();
        renderPassBeginInfo.pClearValues = clearValues.data();

        // Begin render pass
        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Set up viewport info
        VkViewport viewport{};
        viewport.width = framebuffer->GetWidth();
        viewport.height = framebuffer->GetHeight();
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Set viewport
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        // Set up scissor info
        VkRect2D scissor{};
        scissor.extent.width = framebuffer->GetWidth();
        scissor.extent.height = framebuffer->GetHeight();
        scissor.offset.x = 0;
        scissor.offset.y = 0;

        // Set scissor
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void RenderPass::End(const VkCommandBuffer commandBuffer)
    {
        vkCmdEndRenderPass(commandBuffer);
    }

    /* --- DESTRUCTOR --- */

    void RenderPass::Destroy()
    {
        vkDestroyRenderPass(VK::GetLogicalDevice(), renderPass, nullptr);
        clearValues.clear();
    }
}


