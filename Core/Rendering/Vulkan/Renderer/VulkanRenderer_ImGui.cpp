//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateImGuiInstance()
    {
        std::vector<VkDescriptorPoolSize> poolSizes =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo imGuiDescriptorPoolCreateInfo{};
        imGuiDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        imGuiDescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        imGuiDescriptorPoolCreateInfo.maxSets = 1000;
        imGuiDescriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
        imGuiDescriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

        VulkanDebugger::CheckResults(
            vkCreateDescriptorPool(this->logicalDevice, &imGuiDescriptorPoolCreateInfo, nullptr, &imGuiDescriptorPool),
            "Could not create ImGui descriptor pool"
        );

        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForVulkan(window.GetCoreWindow(), true);

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = instance;
        initInfo.PhysicalDevice = physicalDevice;
        initInfo.Device = logicalDevice;
        initInfo.Queue = graphicsQueue;
        initInfo.DescriptorPool = imGuiDescriptorPool;
        initInfo.MinImageCount = MAX_CONCURRENT_FRAMES;
        initInfo.ImageCount = MAX_CONCURRENT_FRAMES;
        initInfo.MSAASamples = msaaSampleCount;

        ImGui_ImplVulkan_Init(&initInfo, renderPass->GetVulkanRenderPass());

        VkCommandBuffer commandBuffer = VulkanUtilities::BeginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        VulkanUtilities::EndSingleTimeCommands(commandBuffer);

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    void VulkanRenderer::BeginNewImGuiFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void VulkanRenderer::UpdateImGuiData()
    {

    }

    void VulkanRenderer::RenderImGui()
    {
        ImGui::Render();
    }

}