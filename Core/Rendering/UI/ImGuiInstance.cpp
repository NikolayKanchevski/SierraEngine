//
// Created by Nikolay Kanchevski on 23.12.22.
//


#include "ImGuiInstance.h"

#include "../Vulkan/VK.h"
#include "../../../Engine/Classes/Cursor.h"

namespace Sierra::Rendering
{

    /* --- CONSTRUCTORS --- */

    ImGuiInstance::ImGuiInstance(const ImGuiInstanceCreateInfo &createInfo)
        : window(createInfo.window), hasImGuizmoLayer(createInfo.createImGuizmoLayer)
    {
        // Get style
        if (createInfo.givenImGuiStyle == nullptr)
        {
            // Set default ImGui style
            imGuiStyle.Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            imGuiStyle.Colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
            imGuiStyle.Colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
            imGuiStyle.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
            imGuiStyle.Colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
            imGuiStyle.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            imGuiStyle.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
            imGuiStyle.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
            imGuiStyle.Colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            imGuiStyle.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            imGuiStyle.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            imGuiStyle.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
            imGuiStyle.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            imGuiStyle.Colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
            imGuiStyle.Colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            imGuiStyle.Colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            imGuiStyle.Colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
            imGuiStyle.Colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
            imGuiStyle.Colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            imGuiStyle.Colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            imGuiStyle.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
            imGuiStyle.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
            imGuiStyle.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
            imGuiStyle.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
            imGuiStyle.WindowPadding                     = ImVec2(8.00f, 8.00f);
            imGuiStyle.FramePadding                      = ImVec2(5.00f, 2.00f);
            imGuiStyle.CellPadding                       = ImVec2(6.00f, 6.00f);
            imGuiStyle.ItemSpacing                       = ImVec2(6.00f, 6.00f);
            imGuiStyle.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
            imGuiStyle.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
            imGuiStyle.IndentSpacing                     = 25;
            imGuiStyle.ScrollbarSize                     = 15;
            imGuiStyle.GrabMinSize                       = 10;
            imGuiStyle.WindowBorderSize                  = 1;
            imGuiStyle.ChildBorderSize                   = 1;
            imGuiStyle.PopupBorderSize                   = 1;
            imGuiStyle.FrameBorderSize                   = 1;
            imGuiStyle.TabBorderSize                     = 1;
            imGuiStyle.WindowRounding                    = 0; // 7
            imGuiStyle.ChildRounding                     = 4;
            imGuiStyle.FrameRounding                     = 3;
            imGuiStyle.PopupRounding                     = 4;
            imGuiStyle.ScrollbarRounding                 = 9;
            imGuiStyle.GrabRounding                      = 3;
            imGuiStyle.LogSliderDeadzone                 = 4;
            imGuiStyle.TabRounding                       = 4;
        }
        else
        {
            imGuiStyle = *createInfo.givenImGuiStyle;
        }

        // Create ImGui context
        imGuiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(imGuiContext);

        ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void* user_data) {
            return vkGetInstanceProcAddr(VK::GetInstance(), function_name);
        });

        // Create GLFW backend for context
        ImGui_ImplGlfw_InitForVulkan(createInfo.window->GetCoreWindow(), true);

        // Set up Vulkan backend creation info
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = VK::GetInstance();
        initInfo.PhysicalDevice = VK::GetDevice()->GetPhysicalDevice();
        initInfo.Device = VK::GetDevice()->GetLogicalDevice();
        initInfo.Queue = VK::GetDevice()->GetGraphicsQueue();
        initInfo.DescriptorPool = VK::GetImGuiDescriptorPool();
        initInfo.MinImageCount = VK::GetDevice()->GetMaxConcurrentFramesCount();
        initInfo.ImageCount = VK::GetDevice()->GetMaxConcurrentFramesCount();
        initInfo.MSAASamples = static_cast<VkSampleCountFlagBits>(createInfo.sampling);

        // Create the Vulkan backend
        ImGui_ImplVulkan_Init(&initInfo, createInfo.swapchain->GetRenderPass()->GetVulkanRenderPass());

        // Set settings
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags = createInfo.imGuiConfigFlags;
        io.ConfigDragClickToInputText = true;

        if (!createInfo.fontCreateInfos.empty())
        {
            // Load font file
            for (const auto &fontCreateInfo : createInfo.fontCreateInfos)
            {
                io.Fonts->AddFontFromFileTTF(fontCreateInfo.fontFilePath.c_str(), fontCreateInfo.fontSize);
            }

            // ImGui sometimes bugs out and doesn't assign its default font, so we are doing it manually here
            io.FontDefault = io.Fonts->Fonts[0];

            // Upload font file to shader
            auto commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();
            ImGui_ImplVulkan_CreateFontsTexture(commandBuffer->GetVulkanCommandBuffer());
            VK::GetDevice()->EndSingleTimeCommands(commandBuffer);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }

        if (createInfo.createImGuizmoLayer)
        {
            ImGuizmo::Style &imGuizmoStyle = ImGuizmo::GetStyle();
            imGuizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_X] = ImVec4(0.000f, 0.666f, 0.000f, 1.000f);
            imGuizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_Y] = ImVec4(0.666f, 0.000f, 0.000f, 1.000f);
            imGuizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_Z] = ImVec4(0.000f, 0.000f, 0.666f, 1.000f);
        }
    }

    UniquePtr<ImGuiInstance> ImGuiInstance::Create(const ImGuiInstanceCreateInfo &createInfo)
    {
        return std::make_unique<ImGuiInstance>(createInfo);
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

        if (!Engine::Cursor::IsCursorShown()) ImGui::GetIO().MousePos = { -696969, -696969 };

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

    }

}