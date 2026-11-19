//
// Created by Nikolay Kanchevski on 3.08.24.
//

#pragma once

#include "EditorFrame.h"
#include "EditorThemes.h"
#include "../Core/ApplicationFrame.h"

#include "../Rendering/Viewport.h"

#include "Panels/LogPanel.h"
#include "Panels/FrameInfoPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/AssetBrowserPanel.h"

namespace SierraEngine
{
    struct EditorCreateInfo
    {
        Sierra::Window& window;
        EditorTheme theme = EditorTheme::Dark;
    };

    using ViewportID = Sierra::Handle<uint32>;

    class Editor final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using ViewportEnumerationPredicate = std::function<void(Viewport&)>;

        /* --- CONSTRUCTORS --- */
        explicit Editor(const ApplicationFrame& applicationFrame, const EditorCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame);

        Viewport& CreateViewport(ViewportID& ID, const ViewportCreateInfo& createInfo);
        void ForEachViewport(const ViewportEnumerationPredicate& Predicate);
        bool DestroyViewport(ViewportID ID);

        /* --- GETTER METHODS --- */
        [[nodiscard]] Scene& GetScene() noexcept { return scene; }
        [[nodiscard]] const Scene& GetScene() const noexcept { return scene; }

        // [[nodiscard]] std::optional<EntityID> GetSelectedEntity() const noexcept { return !selectedEntities.empty() ? std::optional(selectedEntities[0]) : std::nullopt; }
        // [[nodiscard]] std::span<const EntityID> GetSelectedEntities() const noexcept { return selectedEntities; }

        [[nodiscard]] Viewport* GetViewport(const ViewportID ID) noexcept { return viewports.GetItem(ID); }
        [[nodiscard]] const Viewport* GetViewport(const ViewportID ID) const noexcept { return viewports.GetItem(ID); }

        /* --- COPY SEMANTICS --- */
        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        /* --- MOVE SEMANTICS --- */
        Editor(Editor&&) noexcept = default;
        Editor& operator=(Editor&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Editor() noexcept = default;

    private:
        Scene scene;

        ModalManager modalManager = { };

        bool refreshAssetBrowserPanel = false;
        AssetBrowserPanel assetBrowserPanel;

        LogPanel logPanel;
        HierarchyPanel hierarchyPanel = { };
        FrameInfoPanel frameInfoPanel = { };

        Sierra::HandleManager<ViewportID, Viewport> viewports = { };

        ImGuiStyle style = { };

        [[nodiscard]] EditorFrame Bind();
        ImGuiID DrawDockSpace(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame) const;

    };

}
