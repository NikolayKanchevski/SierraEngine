//
// Created by Nikolay Kanchevski on 11.05.24.
//

#pragma once

#include "../Editor/Editor.h"

namespace SierraEngine
{

    struct EditorSurfaceCreateInfo
    {
        Editor& editor;
        Sierra::CommandBuffer& commandBuffer;
    };

    class EditorSurface final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorSurface(const EditorSurfaceCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        bool Update();
        void Render(Sierra::CommandBuffer& commandBuffer);
        void Present(Sierra::CommandBuffer& commandBuffer);

        /* --- GETTER METHODS --- */
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
        Editor& editor;
        std::optional<Sierra::ImGuiRenderer> imGuiRenderer = std::nullopt;

        std::unique_ptr<Sierra::Window> window = nullptr;
        std::unique_ptr<Sierra::Swapchain> swapchain = nullptr;
        std::unique_ptr<Sierra::RenderPass> renderPass = nullptr;

        std::vector<std::unique_ptr<Sierra::Framebuffer>> framebuffers = { };
        void CreateFramebuffers();

    };

}
