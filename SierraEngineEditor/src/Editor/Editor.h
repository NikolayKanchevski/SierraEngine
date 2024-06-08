//
// Created by Nikolay Kanchevski on 11.05.24.
//

#pragma once

namespace SierraEngine
{

    enum class EditorTheme : bool
    {
        Light,
        Dark
    };

    struct EditorCreateInfo
    {
        EditorTheme theme = EditorTheme::Dark;

        uint32 concurrentFrameCount = 0;
        uint32 scaling = 1;

        const Sierra::RenderingContext &renderingContext;
        Sierra::CommandBuffer &commandBuffer;

        const Sierra::Image &templateOutputImage;
        Sierra::ResourceTable &resourceTable;
    };

    class Editor final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Editor(const EditorCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update(Scene &scene, const Sierra::InputManager* inputManager = nullptr, const Sierra::CursorManager* cursorManager = nullptr, const Sierra::TouchManager* touchManager = nullptr);
        void Resize(uint32 width, uint32 height, uint32 scaling);
        void Render(Sierra::CommandBuffer &commandBuffer, const Sierra::Image &outputImage);

        /* --- OPERATORS --- */
        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        /* --- DESTRUCTOR --- */
        ~Editor() = default;

    private:
        Sierra::ImGuiRenderTask imGuiRenderTask;
        Entity selectedEntity = { };

    };

}
