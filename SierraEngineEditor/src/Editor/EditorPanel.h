//
// Created by Nikolay Kanchevski on 22.10.24.
//

#pragma once

namespace SierraEngine
{

    class EditorPanel
    {
    public:
        /* --- COPY SEMANTICS --- */
        EditorPanel(const EditorPanel&) = delete;
        EditorPanel& operator=(const EditorPanel&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~EditorPanel() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        EditorPanel() noexcept = default;

        /* --- CONSTANTS --- */
        constexpr static ImGuiWindowFlags DEFAULT_WINDOW_FLAGS = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNav;

        /* --- MOVE SEMANTICS --- */
        EditorPanel(EditorPanel&&) noexcept = default;
        EditorPanel& operator=(EditorPanel&&) noexcept = default;

    };

}