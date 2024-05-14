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
        std::unique_ptr<Sierra::CommandBuffer> &commandBuffer;

        const std::unique_ptr<Sierra::Image> &templateOutputImage;
        std::unique_ptr<Sierra::ResourceTable> &resourceTable;
    };

    class Editor final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Editor(const EditorCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update(Scene &scene, const std::optional<std::reference_wrapper<const Sierra::InputManager>> &inputManager = std::nullopt, const std::optional<std::reference_wrapper<const Sierra::CursorManager>> &cursorManager = std::nullopt, const std::optional<std::reference_wrapper<const Sierra::TouchManager>> &touchManager = std::nullopt);
        void Resize(uint32 width, uint32 height, uint32 scaling);
        void Render(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer, const std::unique_ptr<Sierra::Image> &outputImage);

        /* --- OPERATORS --- */
        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        /* --- DESTRUCTOR --- */
        ~Editor() = default;

    private:
        Sierra::ImGuiRenderTask imGuiRenderTask;

    };

}
