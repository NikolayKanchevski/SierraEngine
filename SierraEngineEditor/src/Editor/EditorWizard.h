//
// Created by Nikolay Kanchevski on 7.11.24.
//

#pragma once

namespace SierraEngine
{

    class EditorWizard
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Draw(bool& open) = 0;

        /* --- COPY SEMANTICS --- */
        EditorWizard(const EditorWizard&) = delete;
        EditorWizard& operator=(const EditorWizard&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~EditorWizard() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        EditorWizard() noexcept = default;

        /* --- POLLING METHODS --- */
        void DrawShadow();

        /* --- CONSTANTS --- */
        constexpr static ImGuiWindowFlags DEFAULT_WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
        constexpr static ImGuiTableFlags DEFAULT_HEADER_TABLE_FLAGS = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV;
        constexpr static ImGuiTableFlags DEFAULT_BODY_TABLE_FLAGS = ImGuiTableFlags_PadOuterX | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInner;
        constexpr static ImGuiTreeNodeFlags DEFAULT_BODY_TREE_FLAGS = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_FramePadding;

        /* --- MOVE SEMANTICS --- */
        EditorWizard(EditorWizard&&) noexcept = default;
        EditorWizard& operator=(EditorWizard&&) noexcept = default;

    };

}