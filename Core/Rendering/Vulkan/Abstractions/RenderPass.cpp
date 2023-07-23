//
// Created by Nikolay Kanchevski on 12.12.22.
//

#include "RenderPass.h"

#include "../VK.h"

// TODO: Resolve attachment support
namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    RenderPass::RenderPass(const RenderPassCreateInfo &createInfo)
    {
        // Allocate memory for attachments
        std::vector<VkAttachmentDescription> attachmentDescriptions;
        attachmentDescriptions.resize(createInfo.attachments.size());

        // Keep track of how many attachments are loaded
        uint totalColorAttachmentLoadCount = 0;
        bool loadsDepthAttachment = false;
        int depthAttachmentIndex = -1;

        // Set up attachments & clear values
        clearValues.resize(createInfo.attachments.size());
        for (uint i = createInfo.attachments.size(); i--;)
        {
            const auto &attachment = createInfo.attachments[i];

            // Add attachment description
            attachmentDescriptions[i] = {
                .format = static_cast<VkFormat>(attachment.image->GetFormat()),
                .samples = static_cast<VkSampleCountFlagBits>(attachment.image->GetSampling()),
                .loadOp = static_cast<VkAttachmentLoadOp>(attachment.loadOp),
                .storeOp = static_cast<VkAttachmentStoreOp>(attachment.storeOp),
                .stencilLoadOp = static_cast<VkAttachmentLoadOp>(LoadOp::DONT_CARE),
                .stencilStoreOp = static_cast<VkAttachmentStoreOp>(StoreOp::DONT_CARE),
                .initialLayout = createInfo.attachments[i].loadOp == LoadOp::LOAD ? static_cast<VkImageLayout>(createInfo.attachments[i].image->GetLayout()) : static_cast<VkImageLayout>(VK_IMAGE_LAYOUT_UNDEFINED),
            };

            switch (attachment.type)
            {
                case RenderPassAttachmentType::SWAPCHAIN:
                    attachmentDescriptions[i].finalLayout = static_cast<VkImageLayout>(ImageLayout::PRESENT_SRC);
                    break;
                case RenderPassAttachmentType::COLOR:
                    attachmentDescriptions[i].finalLayout = static_cast<VkImageLayout>(ImageLayout::SHADER_READ_ONLY_OPTIMAL);
                case RenderPassAttachmentType::DEPTH:
                    attachmentDescriptions[i].finalLayout = static_cast<VkImageLayout>(ImageLayout::SHADER_READ_ONLY_OPTIMAL);
                    depthAttachmentIndex = i;
                    break;
            }

            // Set clear values
            if (attachment.type == RenderPassAttachmentType::DEPTH)
            {
                clearValues[i].depthStencil = { 1.0f, 0 };
                if (attachment.loadOp == LoadOp::LOAD) loadsDepthAttachment = true;
            }
            else
            {
                clearValues[i].color = { attachment.clearColor.x, attachment.clearColor.y, attachment.clearColor.z, attachment.clearColor.w };
                if (attachment.loadOp == LoadOp::LOAD) totalColorAttachmentLoadCount++;
            }
        }

        // Allocate memory to hold subpass dependencies (we need both, as the VkSubpassDescription vector points to data inside the SubpassDescription vector)
        std::vector<SubpassDescription> subpassDescriptions;
        subpassDescriptions.resize(createInfo.subpassInfos.size());

        std::vector<VkSubpassDescription> vkSubpassDescriptions;
        vkSubpassDescriptions.resize(createInfo.subpassInfos.size());

        // Set up subpasses & transitioning info
        subpasses.resize(createInfo.subpassInfos.size());

        // Allocate a dependency for each subpass plus one extra and another extra one for depth
        std::vector<VkSubpassDependency> subpassDependencies;
        subpassDependencies.reserve(createInfo.subpassInfos.size() + 1 + loadsDepthAttachment);

        // Populate rest of subpass info
        for (uint i = createInfo.subpassInfos.size(); i--;)
        {
            // Set default values
            const auto &subpassInfo = createInfo.subpassInfos[i];
            SubpassDescription &subpassDescription = subpassDescriptions[i];

            // Allocate transitioning info data
            auto &transitionInfo = subpasses[i].attachmentTransitionInfo;
            transitionInfo.resize(subpassInfo.renderTargets.size());

            // Check if subpass loads depth attachment from another subpass
            if (subpassInfo.loadDepthAttachment && depthAttachmentIndex != -1)
            {
                subpassDescription.SetDepthReference({
                    .attachment = static_cast<uint>(depthAttachmentIndex),
                    .layout = static_cast<VkImageLayout>(ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                });
                subpasses[i].hasDepthAttachment = true;
            }

            // Assign attachment references
            for (uint j = 0; j < subpassInfo.renderTargets.size(); j++)
            {
                uint renderTargetIndex = subpassInfo.renderTargets[j];
                if (createInfo.attachments[renderTargetIndex].type == RenderPassAttachmentType::DEPTH)
                {
                    // Set depth reference
                    subpassDescription.SetDepthReference({
                        .attachment = renderTargetIndex,
                        .layout = static_cast<VkImageLayout>(ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                    });

                    // Save subpass index
                    subpasses[i].hasDepthAttachment = true;
                    subpasses[i].writesToDepthAttachment = true;
                }
                else
                {
                    // Set color references
                    subpassDescription.AddColorReference({
                        .attachment = renderTargetIndex,
                        .layout = static_cast<VkImageLayout>(ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
                    });
                    subpasses[i].colorAttachmentCount++;
                }

                // Save transition info
                transitionInfo[j] = { .image = createInfo.attachments[renderTargetIndex].image.get(), .finalLayout = static_cast<ImageLayout>(attachmentDescriptions[renderTargetIndex].finalLayout) };
            }

            for (uint j = 0; j < subpassInfo.subpassInputs.size(); j++)
            {
                uint subpassInputIndex = subpassInfo.subpassInputs[j];
                if (createInfo.attachments[subpassInputIndex].type == RenderPassAttachmentType::DEPTH)
                {
                    subpasses[i].hasDepthAttachment = true;
                }

                // Add input attachment references
                subpassDescription.AddInputReference({
                    .attachment = subpassInputIndex,
                    .layout = static_cast<VkImageLayout>(ImageLayout::SHADER_READ_ONLY_OPTIMAL)
                });
            }

            // Create and store VkSubpassDescription
            vkSubpassDescriptions[i] = subpassDescription.CreateVulkanSubpassDescription();

            // Add dependency between current and previous subpass
            if (i != 0)
            {
                subpassDependencies.push_back({
                    .srcSubpass = i - 1,
                    .dstSubpass = i,
                    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT,
                    .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
                });
            }

            // Add special dependency between current and previous subpass for depth attachment
            if (subpasses[i].writesToDepthAttachment)
            {
                subpassDependencies.push_back({
                    .srcSubpass = i == 0 ? VK_SUBPASS_EXTERNAL : i - 1,
                    .dstSubpass = i,
                    .srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                    .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
                });
            }
        }

        // Set first dependency
        subpassDependencies.push_back({
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = static_cast<VkPipelineStageFlags>(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | (totalColorAttachmentLoadCount > 0 ? VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT : 0)),
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            .srcAccessMask = static_cast<VkAccessFlags>(VK_ACCESS_SHADER_READ_BIT | (totalColorAttachmentLoadCount > 0 ? VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT : 0) | (loadsDepthAttachment ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT : 0)),
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
        });

        // Set last dependency
        subpassDependencies.push_back({
            .srcSubpass = static_cast<uint>(createInfo.subpassInfos.size() - 1),
            .dstSubpass = VK_SUBPASS_EXTERNAL,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        });

        // Create render pass
        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint>(attachmentDescriptions.size());
        renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
        renderPassCreateInfo.subpassCount = static_cast<uint>(vkSubpassDescriptions.size());
        renderPassCreateInfo.pSubpasses = vkSubpassDescriptions.data();
        renderPassCreateInfo.dependencyCount = static_cast<uint>(subpassDependencies.size());
        renderPassCreateInfo.pDependencies = subpassDependencies.data();
        VK_ASSERT(vkCreateRenderPass(VK::GetLogicalDevice(), &renderPassCreateInfo, nullptr, &renderPass), "Could not create render pass");
    }

    UniquePtr<RenderPass> RenderPass::Create(const RenderPassCreateInfo &createInfo)
    {
        return std::make_unique<RenderPass>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void RenderPass::NextSubpass(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        // Trigger next subpass
        vkCmdNextSubpass(commandBuffer->GetVulkanCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE);
        currentSubpass++;

        // Apply layout transitions
        for (const auto &transition : subpasses[currentSubpass].attachmentTransitionInfo)
        {
            commandBuffer->TransitionImageLayoutFromRenderPass(transition.image, transition.finalLayout);
        }
    }

    void RenderPass::Begin(const UniquePtr<Framebuffer> &framebuffer, const UniquePtr<CommandBuffer> &commandBuffer)
    {
        // Reset subpass index
        currentSubpass = 0;

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
        vkCmdBeginRenderPass(commandBuffer->GetVulkanCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Set viewport and apply scissoring
        commandBuffer->SetViewportAndScissor(framebuffer->GetWidth(), framebuffer->GetHeight());

        // Apply layout transitions
        for (const auto &transition : subpasses[currentSubpass].attachmentTransitionInfo)
        {
            commandBuffer->TransitionImageLayoutFromRenderPass(transition.image, transition.finalLayout);
        }
    }

    void RenderPass::End(const UniquePtr<CommandBuffer> &commandBuffer)
    {
        vkCmdEndRenderPass(commandBuffer->GetVulkanCommandBuffer());
    }

    /* --- SETTER METHODS  --- */

    void RenderPass::SetClearColor(const uint colorAttachmentIndex, const Vector4& color)
    {
        clearValues[colorAttachmentIndex].color = { color.x, color.y, color.z, color.w };
    }

    /* --- DESTRUCTOR --- */

    void RenderPass::Destroy()
    {
        vkDestroyRenderPass(VK::GetLogicalDevice(), renderPass, nullptr);
    }
}


