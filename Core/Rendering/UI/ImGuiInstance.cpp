//
// Created by Nikolay Kanchevski on 23.12.22.
//

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <ImGuizmo.h>

#include "ImGuiInstance.h"

#include "ImGuiCore.h"
#include "../../../Engine/Classes/Cursor.h"

#define MAX_IMGUI_DESCRIPTOR_COUNT 2000

using Sierra::Engine::Classes::Cursor;

namespace Sierra::Core::Rendering::UI
{

    /* --- CONSTRUCTORS --- */

    ImGuiInstance::ImGuiInstance(const ImGuiInstanceCreateInfo &createInfo)
        : window(createInfo.window), hasImGuizmoLayer(createInfo.createImGuizmoLayer)
    {
        // Set up example pool sizes
        std::vector<VkDescriptorPoolSize> poolSizes =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER,                MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, MAX_IMGUI_DESCRIPTOR_COUNT },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       MAX_IMGUI_DESCRIPTOR_COUNT }
        };

        // Set up descriptor pool creation info
        VkDescriptorPoolCreateInfo imGuiDescriptorPoolCreateInfo{};
        imGuiDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        imGuiDescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        imGuiDescriptorPoolCreateInfo.maxSets = MAX_IMGUI_DESCRIPTOR_COUNT;
        imGuiDescriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
        imGuiDescriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

        // Create descriptor pool
        VK_ASSERT(
            vkCreateDescriptorPool(VK::GetDevice()->GetLogicalDevice(), &imGuiDescriptorPoolCreateInfo, nullptr, &descriptorPool),
            "Could not create ImGui descriptor pool"
        );

        // Get style
        if (createInfo.givenImGuiStyle == nullptr) imGuiStyle = ImGuiCore::GetDefaultStyle();
        else imGuiStyle = std::move(*createInfo.givenImGuiStyle);

        // Create ImGui context
        imGuiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(imGuiContext);

        // Create GLFW backend for context
        ImGui_ImplGlfw_InitForVulkan(createInfo.window->GetCoreWindow(), true);

        // Set up Vulkan backend creation info
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = VK::GetInstance();
        initInfo.PhysicalDevice = VK::GetDevice()->GetPhysicalDevice();
        initInfo.Device = VK::GetDevice()->GetLogicalDevice();
        initInfo.Queue = VK::GetDevice()->GetGraphicsQueue();
        initInfo.DescriptorPool = descriptorPool;
        initInfo.MinImageCount = createInfo.swapchain->GetMaxConcurrentFramesCount();
        initInfo.ImageCount = createInfo.swapchain->GetMaxConcurrentFramesCount();
        initInfo.MSAASamples = (VkSampleCountFlagBits) createInfo.sampling;

        // Create the Vulkan backend
        ImGui_ImplVulkan_Init(&initInfo, createInfo.swapchain->GetRenderPass()->GetVulkanRenderPass());

        // Set settings
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags = createInfo.imGuiConfigFlags;
        if (VK::GetDevice()->GetBestColorImageFormat() == Vulkan::FORMAT_R8G8B8A8_SRGB)
        {
            io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
        }

        if (createInfo.fontFilePath != nullptr)
        {
            // Load font file
            io.Fonts->AddFontFromFileTTF(createInfo.fontFilePath, createInfo.fontSize);

            // Upload font file to shader
            VkCommandBuffer commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();
            ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
            VK::GetDevice()->EndSingleTimeCommands(commandBuffer);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    }

    std::unique_ptr<ImGuiInstance> ImGuiInstance::Create(ImGuiInstanceCreateInfo createInfo)
    {
        return std::make_unique<ImGuiInstance>(createInfo);
    }

    /* --- POLLING METHODS --- */

    void ImGuiInstance::RenderDrawData(VkCommandBuffer commandBuffer)
    {
        // NOTE: Might have to have a separate draw list per instance
        if (ImGui::GetDrawData() != nullptr) ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    }

    void ImGuiInstance::Render()
    {
        if (!imGuiFrameBegan) return;

        if (window->IsFocusRequired() && !window->IsFocused())
        {
            EndImGuiFrame();
            return;
        }

        ImGui::Render();
        imGuiFrameBegan = false;
    }

    void ImGuiInstance::BeginNewImGuiFrame()
    {
        if (imGuiFrameBegan) return;

        ImGui::SetCurrentContext(imGuiContext);
        if (hasImGuizmoLayer) ImGuizmo::SetImGuiContext(imGuiContext);

        ImGui::GetStyle() = imGuiStyle;

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        if (!Cursor::IsCursorShown()) ImGui::GetIO().MousePos = { -696969, -696969 };

        if (hasImGuizmoLayer) ImGuizmo::BeginFrame();

        imGuiFrameBegan = true;
    }

    void ImGuiInstance::EndImGuiFrame()
    {
        ImGui::EndFrame();
        imGuiFrameBegan = false;
    }

    /* --- DESTRUCTOR --- */

    void ImGuiInstance::Destroy()
    {
        vkDestroyDescriptorPool(VK::GetLogicalDevice(), descriptorPool, nullptr);
    }

}