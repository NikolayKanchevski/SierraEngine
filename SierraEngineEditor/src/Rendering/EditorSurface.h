//
// Created by Nikolay Kanchevski on 11.05.24.
//

#pragma once

#include "../Editor/Editor.h"

namespace SierraEngine
{

    struct EditorSurfaceCreateInfo
    {
        const Sierra::PlatformContext& platformContext;
        const RenderingContext& renderingContext;

        Sierra::CommandBuffer& commandBuffer;
    };

    class EditorSurface final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorSurface(const EditorSurfaceCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        bool Update() const;
        void Render(Sierra::CommandBuffer& commandBuffer, Editor& editor);
        void Present(Sierra::CommandBuffer& commandBuffer) const;

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
        const RenderingContext* renderingContext;

        std::unique_ptr<Sierra::Window> window = nullptr;
        std::unique_ptr<Sierra::Swapchain> swapchain = nullptr;
        std::unique_ptr<Sierra::RenderPass> renderPass = nullptr;
        std::optional<Sierra::ImGuiRenderer> imGuiRenderer = std::nullopt;

        std::vector<std::unique_ptr<Sierra::Framebuffer>> framebuffers = { };
        void CreateFramebuffers();

    };

}
