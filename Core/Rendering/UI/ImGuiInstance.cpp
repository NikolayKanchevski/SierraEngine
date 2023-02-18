//
// Created by Nikolay Kanchevski on 23.12.22.
//


#include "ImGuiInstance.h"

#include "ImGuiCore.h"
#include "../../../Engine/Classes/Cursor.h"

using Sierra::Engine::Classes::Cursor;

namespace Sierra::Core::Rendering::UI
{

    /* --- CONSTRUCTORS --- */

    ImGuiInstance::ImGuiInstance(const ImGuiInstanceCreateInfo &createInfo)
        : window(createInfo.window), hasImGuizmoLayer(createInfo.createImGuizmoLayer)
    {
        // Get style
        if (createInfo.givenImGuiStyle == nullptr) imGuiStyle = ImGuiCore::GetDefaultStyle();
        else imGuiStyle = *createInfo.givenImGuiStyle;

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
        initInfo.DescriptorPool = VK::GetImGuiDescriptorPool();
        initInfo.MinImageCount = createInfo.swapchain->GetMaxConcurrentFramesCount();
        initInfo.ImageCount = createInfo.swapchain->GetMaxConcurrentFramesCount();
        initInfo.MSAASamples = (VkSampleCountFlagBits) createInfo.sampling;

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
                auto fontData = File::ReadBinaryFile(fontCreateInfo.fontFilePath);
                io.Fonts->AddFontFromMemoryTTF(fontData.data(), fontData.size(), fontCreateInfo.fontSize);
            }

            // Upload font file to shader
            VkCommandBuffer commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();
            ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
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

    UniquePtr<ImGuiInstance> ImGuiInstance::Create(ImGuiInstanceCreateInfo createInfo)
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

    }

}