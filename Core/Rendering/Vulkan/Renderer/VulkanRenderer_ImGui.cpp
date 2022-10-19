//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"
#include "../../../../Engine/Classes/Cursor.h"

using namespace Sierra::Engine::Classes;

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

        Debugger::CheckResults(
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

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        SetImGuiStyle();
    }

    void VulkanRenderer::SetImGuiStyle()
    {
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Fonts/PTSans.ttf", 18.0f);

        VkCommandBuffer commandBuffer = VulkanUtilities::BeginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        VulkanUtilities::EndSingleTimeCommands(commandBuffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.FrameRounding = 4.0f;
        style.ScrollbarRounding = 4.0f;
        style.GrabRounding = 4.0f;

        style.Colors[ImGuiCol_Text]                   = { 0.95f, 0.96f, 0.98f, 1.00f };
        style.Colors[ImGuiCol_TextDisabled]           = { 0.36f, 0.42f, 0.47f, 1.00f };
        style.Colors[ImGuiCol_WindowBg]               = { 0.11f, 0.15f, 0.17f, 1.00f };
        style.Colors[ImGuiCol_ChildBg]                = { 0.15f, 0.18f, 0.22f, 1.00f };
        style.Colors[ImGuiCol_PopupBg]                = { 0.08f, 0.08f, 0.08f, 0.94f };
        style.Colors[ImGuiCol_Border]                 = { 0.08f, 0.10f, 0.12f, 1.00f };
        style.Colors[ImGuiCol_BorderShadow]           = { 0.00f, 0.00f, 0.00f, 0.00f };
        style.Colors[ImGuiCol_FrameBg]                = { 0.20f, 0.25f, 0.29f, 1.00f };
        style.Colors[ImGuiCol_FrameBgHovered]         = { 0.12f, 0.20f, 0.28f, 1.00f };
        style.Colors[ImGuiCol_FrameBgActive]          = { 0.09f, 0.12f, 0.14f, 1.00f };
        style.Colors[ImGuiCol_TitleBg]                = { 0.09f, 0.12f, 0.14f, 0.65f };
        style.Colors[ImGuiCol_TitleBgActive]          = { 0.08f, 0.10f, 0.12f, 1.00f };
        style.Colors[ImGuiCol_TitleBgCollapsed]       = { 0.00f, 0.00f, 0.00f, 0.51f };
        style.Colors[ImGuiCol_MenuBarBg]              = { 0.15f, 0.18f, 0.22f, 1.00f };
        style.Colors[ImGuiCol_ScrollbarBg]            = { 0.02f, 0.02f, 0.02f, 0.39f };
        style.Colors[ImGuiCol_ScrollbarGrab]          = { 0.20f, 0.25f, 0.29f, 1.00f };
        style.Colors[ImGuiCol_ScrollbarGrabHovered]   = { 0.18f, 0.22f, 0.25f, 1.00f };
        style.Colors[ImGuiCol_ScrollbarGrabActive]    = { 0.09f, 0.21f, 0.31f, 1.00f };
        style.Colors[ImGuiCol_CheckMark]              = { 0.28f, 0.56f, 1.00f, 1.00f };
        style.Colors[ImGuiCol_SliderGrab]             = { 0.28f, 0.56f, 1.00f, 1.00f };
        style.Colors[ImGuiCol_SliderGrabActive]       = { 0.37f, 0.61f, 1.00f, 1.00f };
        style.Colors[ImGuiCol_Button]                 = { 0.20f, 0.25f, 0.29f, 1.00f };
        style.Colors[ImGuiCol_ButtonHovered]          = { 0.28f, 0.56f, 1.00f, 1.00f };
        style.Colors[ImGuiCol_ButtonActive]           = { 0.06f, 0.53f, 0.98f, 1.00f };
        style.Colors[ImGuiCol_Header]                 = { 0.20f, 0.25f, 0.29f, 0.55f };
        style.Colors[ImGuiCol_HeaderHovered]          = { 0.26f, 0.59f, 0.98f, 0.80f };
        style.Colors[ImGuiCol_HeaderActive]           = { 0.26f, 0.59f, 0.98f, 1.00f };
        style.Colors[ImGuiCol_Separator]              = { 0.20f, 0.25f, 0.29f, 1.00f };
        style.Colors[ImGuiCol_SeparatorHovered]       = { 0.10f, 0.40f, 0.75f, 0.78f };
        style.Colors[ImGuiCol_SeparatorActive]        = { 0.10f, 0.40f, 0.75f, 1.00f };
        style.Colors[ImGuiCol_ResizeGrip]             = { 0.26f, 0.59f, 0.98f, 0.25f };
        style.Colors[ImGuiCol_ResizeGripHovered]      = { 0.26f, 0.59f, 0.98f, 0.67f };
        style.Colors[ImGuiCol_ResizeGripActive]       = { 0.26f, 0.59f, 0.98f, 0.95f };
        style.Colors[ImGuiCol_Tab]                    = { 0.11f, 0.15f, 0.17f, 1.00f };
        style.Colors[ImGuiCol_TabHovered]             = { 0.26f, 0.59f, 0.98f, 0.80f };
        style.Colors[ImGuiCol_TabActive]              = { 0.20f, 0.25f, 0.29f, 1.00f };
        style.Colors[ImGuiCol_TabUnfocused]           = { 0.11f, 0.15f, 0.17f, 1.00f };
        style.Colors[ImGuiCol_TabUnfocusedActive]     = { 0.11f, 0.15f, 0.17f, 1.00f };
        style.Colors[ImGuiCol_PlotLines]              = { 0.61f, 0.61f, 0.61f, 1.00f };
        style.Colors[ImGuiCol_PlotLinesHovered]       = { 1.00f, 0.43f, 0.35f, 1.00f };
        style.Colors[ImGuiCol_PlotHistogram]          = { 0.90f, 0.70f, 0.00f, 1.00f };
        style.Colors[ImGuiCol_PlotHistogramHovered]   = { 1.00f, 0.60f, 0.00f, 1.00f };
        style.Colors[ImGuiCol_TextSelectedBg]         = { 0.26f, 0.59f, 0.98f, 0.35f };
        style.Colors[ImGuiCol_DragDropTarget]         = { 1.00f, 1.00f, 0.00f, 0.90f };
        style.Colors[ImGuiCol_NavHighlight]           = { 0.26f, 0.59f, 0.98f, 1.00f };
        style.Colors[ImGuiCol_NavWindowingHighlight]  = { 1.00f, 1.00f, 1.00f, 0.70f };
        style.Colors[ImGuiCol_NavWindowingDimBg]      = { 0.80f, 0.80f, 0.80f, 0.20f };
        style.Colors[ImGuiCol_ModalWindowDimBg]       = { 0.80f, 0.80f, 0.80f, 0.35f };
    }

    void VulkanRenderer::BeginNewImGuiFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void VulkanRenderer::UpdateImGuiData()
    {
        if (!Cursor::IsCursorShown())
        {
            ImGuiIO &io = ImGui::GetIO();
            io.MousePos = { -696969, -696969 };
        }
    }

    void VulkanRenderer::RenderImGui()
    {
        ImGui::Render();
    }

}