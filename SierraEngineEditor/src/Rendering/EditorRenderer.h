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

        const Sierra::Device& device;
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

        /* --- COPY SEMANTICS --- */
        EditorRenderer(const EditorRenderer&) = delete;
        EditorRenderer& operator=(const EditorRenderer&) = delete;

        /* --- MOVE SEMANTICS --- */
        EditorRenderer(EditorRenderer&&) = delete;
        EditorRenderer& operator=(EditorRenderer&&) = delete;
        
        /* --- DESTRUCTOR --- */
        ~EditorRenderer() noexcept override = default;

    private:
        Sierra::ImGuiRenderer imGuiRenderer;
        EntityID selectedEntity;

    };

}
