//
// Created by Nikolay Kanchevski on 23.12.22.
//

#pragma once

#include "../Window.h"
#include "../../../Engine/Classes/File.h"
#include "../Abstractions/Swapchain.h"

namespace Sierra::Rendering
{

    struct FontCreateInfo
    {
        String fontFilePath;
        float fontSize = 18.0f;
    };

    struct ImGuiInstanceCreateInfo
    {
        UniquePtr<Window> &window;
        UniquePtr<RenderPass> &renderPass;

        const std::vector<FontCreateInfo> &fontCreateInfos;

        ImGuiStyle *givenImGuiStyle;
        ImGuiConfigFlags imGuiConfigFlags = ImGuiConfigFlags_DockingEnable;
        Sampling sampling = Sampling::MSAAx1;

        bool createImGuizmoLayer = false;
    };

    class ImGuiInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ImGuiInstance(const ImGuiInstanceCreateInfo &createInfo);
        static UniquePtr<ImGuiInstance> Create(const ImGuiInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] ImGuiContext* GetImGuiContext() const { return imGuiContext; };
        [[nodiscard]] bool HasImGuizmoLayer() const { return hasImGuizmoLayer; };

        /* --- POLLING METHODS --- */
        void BeginNewImGuiFrame();
        void Render();
        void EndImGuiFrame();

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
