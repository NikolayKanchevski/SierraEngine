//
// Created by Nikolay Kanchevski on 11.05.24.
//

#pragma once

#include "../Editor/Editor.h"
#include "ImGui/ImGuiRenderer.h"

namespace SierraEngine
{

    struct EditorSurfaceCreateInfo
    {
        const Sierra::WindowManager& windowManager;
    };

    class EditorSurface final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorSurface(const DrawFrame& drawFrame, const EditorSurfaceCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        bool Update() const;
        void Render(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame, Editor& editor);
        void Present(const DrawFrame& drawFrame) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] Sierra::Window& GetWindow() noexcept { return *window; }
        [[nodiscard]] uint32 GetCurrentFrameIndex() const noexcept { return swapchain->GetCurrentFrameIndex(); };
        [[nodiscard]] uint32 GetConcurrentFrameCount() const noexcept { return swapchain->GetConcurrentFrameCount(); };

        /* --- COPY SEMANTICS --- */
        EditorSurface(const EditorSurface&) = delete;
        EditorSurface& operator=(const EditorSurface&) = delete;

        /* --- MOVE SEMANTICS --- */
        EditorSurface(EditorSurface&&) = delete;
        EditorSurface& operator=(EditorSurface&&) = delete;
        
        /* --- DESTRUCTOR --- */
        ~EditorSurface() noexcept = default;

    private:
        std::unique_ptr<Sierra::Window> window = nullptr;
        std::unique_ptr<Sierra::Swapchain> swapchain = nullptr;
        ImGuiRenderer imGuiRenderer;

        std::unique_ptr<Sierra::RenderPass> renderPass = nullptr;
        std::vector<std::unique_ptr<Sierra::Framebuffer>> framebuffers = { };

        bool swapchainResized = false;
        void CreateFramebuffers(const DrawFrame& drawFrame);

    };

}
