//
// Created by Nikolay Kanchevski on 11.05.24.
//

#pragma once

#include "../Editor/Editor.h"

namespace SierraEngine
{

    enum class EditorTheme : bool
    {
        Light,
        Dark
    };

    struct EditorRendererCreateInfo
    {
        EditorTheme theme = EditorTheme::Dark;

        uint32 concurrentFrameCount = 0;
        uint32 scaling = 1;

        const Sierra::RenderingContext& renderingContext;
        Sierra::CommandBuffer& commandBuffer;

        const Sierra::Image& templateOutputImage;
        Sierra::ResourceTable& resourceTable;
    };

    class EditorRenderer final : public Sierra::Renderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorRenderer(const EditorRendererCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update(Editor& editor, const Sierra::InputManager* inputManager = nullptr, const Sierra::CursorManager* cursorManager = nullptr, const Sierra::TouchManager* touchManager = nullptr);
        void Resize(uint32 width, uint32 height) override;
        void Render(Sierra::CommandBuffer& commandBuffer, const Sierra::Image& outputImage) override;

        /* --- DESTRUCTOR --- */
        ~EditorRenderer() override = default;

    private:
        Sierra::ImGuiRenderer imGuiRenderer;
        EntityID selectedEntity;

    };

}
