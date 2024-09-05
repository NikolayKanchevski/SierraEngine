//
// Created by Nikolay Kanchevski on 3.08.24.
//

#pragma once

namespace SierraEngine
{

    struct EditorCreateInfo
    {
        Scene& scene;
    };

    struct EditorViewport
    {
        std::unique_ptr<Sierra::Image> image;
    };

    class Editor final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Editor(const EditorCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] Scene& GetScene() { return scene; }
        [[nodiscard]] std::span<const EditorViewport> GetViewports() const { return viewports; }

        /* --- COPY SEMANTICS --- */
        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        /* --- MOVE SEMANTICS --- */
        Editor(Editor&&) = delete;
        Editor& operator=(Editor&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Editor() = default;

    private:
        Scene& scene;
        std::vector<EditorViewport> viewports;

    };

}
