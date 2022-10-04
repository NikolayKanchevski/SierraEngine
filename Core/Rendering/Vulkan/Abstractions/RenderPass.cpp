//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "../VulkanCore.h"
#include "RenderPass.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    // ********************* Subpass ********************* \\

    Subpass::Subpass(const VkPipelineBindPoint givenBindPoint, const Subpass::DepthAttachment givenDepthAttachment, std::vector<ColorAttachment> givenColorAttachments, std::vector<ResolveAttachment> givenResolveAttachments, const uint32_t srcSubpass, const uint32_t dstSubpass)
        : bindPoint(givenBindPoint)
    {
        bool depthAttachmentProvided = givenDepthAttachment.data.finalLayout != VK_IMAGE_LAYOUT_UNDEFINED;

        // Calculate total attachment count
        int attachmentCount = givenColorAttachments.size() + givenResolveAttachments.size() + (depthAttachmentProvided ? 1 : 0);
        this->attachmentDescriptions = std::vector<VkAttachmentDescription>(attachmentCount);

        // Create the references to the color attachments
        this->colorAttachmentReferences = std::vector<VkAttachmentReference>(givenColorAttachments.size());
        for (auto &colorAttachment : givenColorAttachments)
        {
            colorAttachmentReferences.push_back({ colorAttachment.binding, colorAttachment.data.finalLayout });
            attachmentDescriptions.push_back(colorAttachment.data);
        }

        // Create the references to the resolve attachments
        this->resolveAttachmentReferences = std::vector<VkAttachmentReference>(givenResolveAttachments.size());
        for (auto &resolveAttachment : givenResolveAttachments)
        {
            resolveAttachmentReferences.push_back({ resolveAttachment.binding, resolveAttachment.imageLayout });
            attachmentDescriptions.push_back(resolveAttachment.data);
        }

        // If a depth attachment is provided create a reference to it
        if (depthAttachmentProvided)
        {
            attachmentDescriptions.push_back(givenDepthAttachment.data);
            this->depthReference = { givenDepthAttachment.binding, givenDepthAttachment.data.finalLayout };
            this->hasDepthReference = true;
        }

        // Configure the subpass dependency
        vkSubpassDependency.srcSubpass = srcSubpass;
        vkSubpassDependency.dstSubpass = dstSubpass;
        vkSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        vkSubpassDependency.srcAccessMask = 0;
        vkSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        vkSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


        // Create the subpass description
        vkSubpass.pipelineBindPoint = bindPoint;
        vkSubpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
        vkSubpass.pColorAttachments = colorAttachmentReferences.data();
        vkSubpass.pResolveAttachments = resolveAttachmentReferences.data();
        vkSubpass.pDepthStencilAttachment = depthAttachmentProvided ? &depthReference : nullptr;
    }

    Subpass::Builder &Subpass::Builder::SetPipelineBindPoint(const VkPipelineBindPoint givenPipelineBindPoint)
    {
        // Save the given bind point locally
        this->pipelineBindPoint = givenPipelineBindPoint;
        return *this;
    }

    Subpass::Builder &Subpass::Builder::SetDepthAttachment(const uint32_t binding, const Image depthImage, const VkAttachmentLoadOp loadOp, const VkAttachmentStoreOp storeOp, const VkAttachmentLoadOp stencilLoadOp, const VkAttachmentStoreOp stencilStoreOp)
    {
        //  Create the depth attachment and save it together with its binding
        VkAttachmentDescription depthAttachmentDescription{};
        depthAttachmentDescription.format = depthImage.GetFormat();
        depthAttachmentDescription.samples = depthImage.GetSampling();
        depthAttachmentDescription.loadOp = loadOp;
        depthAttachmentDescription.storeOp = storeOp;
        depthAttachmentDescription.stencilLoadOp = stencilLoadOp;
        depthAttachmentDescription.stencilStoreOp = stencilStoreOp;
        depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        this->depthAttachment = { depthAttachmentDescription, binding };

        return *this;
    }

    Subpass::Builder &Subpass::Builder::AddColorAttachment(const uint32_t binding, const Image colorImage, const VkAttachmentLoadOp loadOp, const VkAttachmentStoreOp storeOp, const VkAttachmentLoadOp stencilLoadOp, const VkAttachmentStoreOp stencilStoreOp)
    {
        // Create the color attachment and save it in the local list together with its binding
        VkAttachmentDescription colorAttachmentDescription{};
        colorAttachmentDescription.format = colorImage.GetFormat();
        colorAttachmentDescription.samples = colorImage.GetSampling();
        colorAttachmentDescription.loadOp = loadOp;
        colorAttachmentDescription.storeOp = storeOp;
        colorAttachmentDescription.stencilLoadOp = stencilLoadOp;
        colorAttachmentDescription.stencilStoreOp = stencilStoreOp;
        colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        this->colorAttachments[binding] = { colorAttachmentDescription, binding };

        return *this;
    }

    Subpass::Builder &Subpass::Builder::AddResolveAttachment(const uint32_t binding, const Image image, const VkImageLayout finalLayout, const VkImageLayout referenceLayout, const VkAttachmentLoadOp loadOp, const VkAttachmentStoreOp storeOp, const VkSampleCountFlagBits sampling, const VkAttachmentLoadOp stencilLoadOp, const VkAttachmentStoreOp stencilStoreOp)
    {
        // Create the resolve attachment and save it in the local list together with its binding and reference layout
        VkAttachmentDescription resolveAttachmentDescription{};
        resolveAttachmentDescription.format = image.GetFormat();
        resolveAttachmentDescription.samples = sampling;
        resolveAttachmentDescription.loadOp = loadOp;
        resolveAttachmentDescription.storeOp = storeOp;
        resolveAttachmentDescription.stencilLoadOp = stencilLoadOp;
        resolveAttachmentDescription.stencilStoreOp = stencilStoreOp;
        resolveAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        resolveAttachmentDescription.finalLayout = finalLayout;

        resolveAttachments[binding] = {resolveAttachmentDescription, referenceLayout, binding };

        return *this;
    }

    std::unique_ptr<Subpass> Subpass::Builder::Build(const uint32_t srcSubpass, const uint32_t dstSubpass) const
    {
        return std::make_unique<Subpass>(pipelineBindPoint, depthAttachment, colorAttachments, resolveAttachments, srcSubpass, dstSubpass);
    }

    // ********************* Render Pass ********************* \\

    /* --- CONSTRUCTORS --- */

    RenderPass::RenderPass(Subpass *subpass)
    {
        // Put all subpasses and their dependencies in pointer arrays
        std::vector<VkSubpassDescription> subpasses = { subpass->GetVulkanSubpass() };
        std::vector<VkSubpassDependency> subpassDependencies = { subpass->GetVulkanSubpassDependency() };

        // Set up the render pass creation info
        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(subpass->attachmentDescriptions.size());
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = subpasses.data();
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = subpassDependencies.data();
        renderPassCreateInfo.pAttachments = subpass->attachmentDescriptions.data();

        // Set default values
        clearValues[0].color = { 0.0, 0.0, 0.0 };
        clearValues[1].depthStencil.depth = 1.0;

        // Create the Vulkan render pass
        VulkanDebugger::CheckResults(
            vkCreateRenderPass(VulkanCore::GetLogicalDevice(), &renderPassCreateInfo, nullptr, &vkRenderPass),
            "Could not create render pass with [" + std::to_string(renderPassCreateInfo.subpassCount) + "] subpasses and [" + std::to_string(renderPassCreateInfo.attachmentCount) + "] attachments"
        );
    }

    RenderPass::Builder &RenderPass::Builder::SetSubpass(Subpass *givenSubpass)
    {
        // Save the Vulkan version of provided subpass
        this->subpass = givenSubpass;
        return *this;
    }

    std::unique_ptr<RenderPass> RenderPass::Builder::Build() const
    {
        return std::make_unique<RenderPass>(subpass);
    }

    /* --- SETTER METHODS --- */

    void RenderPass::SetFramebuffer(Framebuffer &givenFramebuffer)
    {
        this->vkFramebuffer = givenFramebuffer.GetVulkanFramebuffer();
    }

    void RenderPass::SetBackgroundColor(const glm::vec3 givenColor)
    {
        clearValues[0].color = { givenColor.x, givenColor.y, givenColor.z };
    }

    void RenderPass::Begin(const VkCommandBuffer givenCommandBuffer)
    {
        if (renderPassBegan)
        {
            VulkanDebugger::ThrowWarning("Cannot begin a render twice. Action automatically suspended");
            return;
        }

        // Configure begin info
        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = this->vkRenderPass;
        beginInfo.framebuffer = vkFramebuffer;

        // Set render area and background color
        beginInfo.renderArea.offset = { 0, 0 };
        beginInfo.renderArea.extent = VulkanCore::GetSwapchainExtent();
        beginInfo.clearValueCount = 2;
        beginInfo.pClearValues = clearValues;

        // Begin the render pass
        vkCmdBeginRenderPass(givenCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
        renderPassBegan = true;
    }

    void RenderPass::End(const VkCommandBuffer givenCommandBuffer)
    {
        if (!renderPassBegan)
        {
            VulkanDebugger::ThrowWarning("Cannot end a non-began render pass. Action automatically suspended");
            return;
        }

        // End the render pass
        vkCmdEndRenderPass(givenCommandBuffer);
        renderPassBegan = false;
    }

    /* --- DESTRUCTOR --- */

    RenderPass::~RenderPass()
    {
        // Destroy the Vulkan render pass
        vkDestroyRenderPass(VulkanCore::GetLogicalDevice(), vkRenderPass, nullptr);
    }
}
