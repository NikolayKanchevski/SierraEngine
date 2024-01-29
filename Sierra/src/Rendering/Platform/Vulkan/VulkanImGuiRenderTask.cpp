//
// Created by Nikolay Kanchevski on 29.01.24.
//

#include "VulkanImGuiRenderTask.h"

#include <backends/imgui_impl_vulkan.h>

#include "VulkanRenderPass.h"
#include "VulkanImage.h"
#include "VulkanCommandBuffer.h"

namespace Sierra
{
    /* --- CONSTRUCTORS --- */

    VulkanImGuiRenderTask::VulkanImGuiRenderTask(const VulkanInstance &instance, const VulkanDevice &device, const ImGuiRenderTaskCreateInfo &createInfo)
        : ImGuiRenderTask(createInfo)
    {
        SR_ERROR_IF(createInfo.templateImage->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Template image passed to Metal-implemented ImGui render task has a graphics API, which differs from [GraphicsAPI::Vulkan]!");

        // Create render pass
        renderPass = std::make_unique<VulkanRenderPass>(device, RenderPassCreateInfo {
            .name = "Render pass of ImGui task [" + std::to_string(GetActiveTaskCount() - 1) + "]",
            .attachments = { { .templateImage = createInfo.templateImage } },
            .subpassDescriptions = {
                { .renderTargets = { 0 } }
            }
        });

        if (GetActiveTaskCount() > 1) return;

        // Create shared descriptor pool
        sharedDescriptorPool = std::make_unique<VulkanDescriptorPool>(device, VulkanDescriptorPoolCreateInfo {
            .name = "ImGui render task's descriptor pool",
            .initialImageCount = 0,
            .initialTextureCount = 512,
            .initialUniformBufferCount = 0,
            .initialStorageBufferCount = 0
        });

        // Set up init info
        ImGui_ImplVulkan_InitInfo initInfo = { };
        initInfo.Instance = instance.GetVulkanInstance();
        initInfo.PhysicalDevice = device.GetPhysicalDevice();
        initInfo.Device = device.GetLogicalDevice();
        initInfo.QueueFamily = device.GetGeneralQueueFamily();
        initInfo.Queue = device.GetGeneralQueue();
        initInfo.PipelineCache = VK_NULL_HANDLE;
        initInfo.DescriptorPool = sharedDescriptorPool->GetVulkanDescriptorPool();
        initInfo.Subpass = 0;
        initInfo.MinImageCount = 2;
        initInfo.ImageCount = 2;
        initInfo.MSAASamples = VulkanImage::ImageSamplingToVkSampleCountFlags(createInfo.templateImage->GetSampling());
        initInfo.CheckVkResultFn = [](const VkResult result)
        {
            switch (result)
            {
                case VK_ERROR_FRAGMENTED_POOL:
                case VK_ERROR_OUT_OF_POOL_MEMORY:
                {
                    sharedDescriptorPool->Reallocate();
                    reinterpret_cast<ImGui_ImplVulkan_InitInfo*>(ImGui::GetIO().BackendRendererUserData)->DescriptorPool = sharedDescriptorPool->GetVulkanDescriptorPool();
                    break;
                }
                default:
                {
                    break;
                }
            }
        };
        initInfo.MinAllocationSize = 1024 * 1024;

        // Initialize ImGui Vulkan backend
        SR_ERROR_IF(!ImGui_ImplVulkan_Init(&initInfo, static_cast<VulkanRenderPass&>(*renderPass).GetVulkanRenderPass()), "[Vulkan]: Could not create Vulkan ImGui backend!");
    }

    /* --- POLLING METHODS --- */

    void VulkanImGuiRenderTask::BeginFrame(const InputManager &inputManager, const CursorManager &cursorManager)
    {
        ImGuiRenderTask::BeginFrame(inputManager, cursorManager);

        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();
    }

    void VulkanImGuiRenderTask::Resize(const uint32 width, const uint32 height)
    {
        ImGuiRenderTask::Resize(width, height);
    }

    void VulkanImGuiRenderTask::Render(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Image> &image)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot execute a Vulkan-implemented ImGui render task using command buffer [{0}], whose graphics API differs from [GraphicsAPI::Vulkan]!!", commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<VulkanCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot render a Vulkan-implemented ImGui render task using image [{0}], whose graphics API differs from [GraphicsAPI::Vulkan]!", image->GetName());
        if (ImGui::GetDrawData() == nullptr) return;

        // Draw ImGui image
        commandBuffer->BeginRenderPass(renderPass, { { .image = image } });
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vulkanCommandBuffer.GetVulkanCommandBuffer());
        commandBuffer->EndRenderPass(renderPass);
    }

    /* --- DESTRUCTOR --- */

    VulkanImGuiRenderTask::~VulkanImGuiRenderTask()
    {
        if (GetActiveTaskCount() == 1)
        {
            ImGui_ImplVulkan_Shutdown();
            ImGui::Shutdown();

            sharedDescriptorPool = nullptr;
        }
    }

}