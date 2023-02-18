//
// Created by Nikolay Kanchevski on 23.12.22.
//

#pragma once

#include "../Window.h"
#include "../Vulkan/Abstractions/Swapchain.h"

using namespace Rendering::Vulkan;

namespace Sierra::Core::Rendering::UI
{

    struct FontCreateInfo
    {
        String fontFilePath = File::OUTPUT_FOLDER_PATH + "Fonts/PTSans.ttf";
        float fontSize = 18.0f;
    };

    struct ImGuiInstanceCreateInfo
    {
        UniquePtr<Window> &window;
        UniquePtr<Swapchain> &swapchain;

        std::vector<FontCreateInfo> fontCreateInfos { FontCreateInfo() };

        ImGuiStyle *givenImGuiStyle;
        ImGuiConfigFlags imGuiConfigFlags = ImGuiConfigFlags_DockingEnable;
        Sampling sampling = Sampling::MSAAx1;

        bool createImGuizmoLayer = false;
    };

    class ImGuiInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        ImGuiInstance(const ImGuiInstanceCreateInfo &createInfo);
        static UniquePtr<ImGuiInstance> Create(ImGuiInstanceCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] ImGuiContext* GetImGuiContext() const { return imGuiContext; };
        [[nodiscard]] bool HasImGuizmoLayer() const { return hasImGuizmoLayer; };

        /* --- POLLING METHODS --- */
        void BeginNewImGuiFrame();
        void EndImGuiFrame();
        void Render();

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(ImGuiInstance);

    private:
        UniquePtr<Window> &window;

        ImGuiStyle imGuiStyle;
        ImGuiContext *imGuiContext;

        bool hasImGuizmoLayer;

        static inline bool imGuiFrameBegan = false;
    };

}
