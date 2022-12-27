//
// Created by Nikolay Kanchevski on 23.12.22.
//

#pragma once

#include <imgui.h>

#include "../Window.h"
#include "../Vulkan/Abstractions/Swapchain.h"

using namespace Rendering::Vulkan;

namespace Sierra::Core::Rendering::UI
{

    struct ImGuiInstanceCreateInfo
    {
        std::unique_ptr<Window> &window;
        std::unique_ptr<Swapchain> &swapchain;

        float fontSize = 18.0f;
        const char* fontFilePath = "Fonts/PTSans.ttf";

        ImGuiStyle *givenImGuiStyle;
        ImGuiConfigFlags imGuiConfigFlags = ImGuiConfigFlags_DockingEnable;
        Sampling sampling = MSAAx1;
    };

    class ImGuiInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        ImGuiInstance(const ImGuiInstanceCreateInfo &createInfo);
        static std::unique_ptr<ImGuiInstance> Create(ImGuiInstanceCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void BeginNewImGuiFrame();
        void EndImGuiFrame();
        void Render();
        void RenderDrawData(VkCommandBuffer commandBuffer);

        /* --- DESTRUCTOR --- */
        void Destroy();
        ImGuiInstance(const ImGuiInstance &) = delete;
        ImGuiInstance &operator=(const ImGuiInstance &) = delete;

    private:
        std::unique_ptr<Window> &window;

        ImGuiStyle imGuiStyle;
        ImGuiContext *imGuiContext;
        VkDescriptorPool descriptorPool;

        static inline bool imGuiFrameBegan = false;
    };

}
