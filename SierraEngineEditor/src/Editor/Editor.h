//
// Created by Nikolay Kanchevski on 3.08.24.
//

#pragma once


#include "EditorThemes.h"
#include "EditorWizard.h"
#include "Panels/ViewportPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"

namespace SierraEngine
{

    struct EditorCreateInfo
    {
        const Sierra::Device& device;
        const Sierra::PlatformContext& platformContext;

        EditorTheme theme = EditorTheme::Dark;

        Scene& scene;
        Sierra::ResourceTable& resourceTable;
    };

    using ViewportID = Sierra::Handle<uint32>;

    class Editor final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using ViewportCallback = std::function<void(ViewportPanel&...)>;

        /* --- CONSTRUCTORS --- */
        explicit Editor(const EditorCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw(Sierra::CommandBuffer& commandBuffer);

        ViewportID CreateViewport(const ViewportCreateInfo& createInfo);
        bool DestroyViewport(ViewportID ID);

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Sierra::Device& GetDevice() const noexcept { return device; }
        [[nodiscard]] const Sierra::PlatformContext& GetPlatformContext() const noexcept { return platformContext; }
        [[nodiscard]] Sierra::ResourceTable& GetResourceTable() noexcept { return resourceTable; }

        [[nodiscard]] std::span<const ViewportID> GetViewports() const noexcept { return viewportIDs; }
        [[nodiscard]] ViewportPanel* GetViewport(const ViewportID ID) noexcept { return const_cast<ViewportPanel*>(const_cast<const Editor*>(this)->GetViewport(ID)); }
        [[nodiscard]] const ViewportPanel* GetViewport(const ViewportID ID) const noexcept { return ID < viewports.size() && viewports[ID].has_value() ? &*viewports[ID] : nullptr; }

        [[nodiscard]] Scene& GetScene() noexcept { return scene; }
        [[nodiscard]] const Scene& GetScene() const noexcept { return scene; }

        /* --- COPY SEMANTICS --- */
        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        /* --- MOVE SEMANTICS --- */
        Editor(Editor&&) = delete;
        Editor& operator=(Editor&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Editor() noexcept = default;

    private:
        const Sierra::Device& device;
        const Sierra::PlatformContext& platformContext;

        Sierra::ResourceTable& resourceTable;
        Scene& scene;

        ImGuiStyle style = { };
        std::unique_ptr<EditorWizard> currentWizard = nullptr;

        HierarchyPanel hierarchy = { };
        PropertiesPanel propertiesPanel = { };

        std::vector<ViewportID> viewportIDs = { };
        Sierra::IndexPool<ViewportID> viewportIndexPool = { };
        std::vector<std::optional<ViewportPanel>> viewports = { };

        void DrawMenuBar();
        ImGuiID DrawDockSpace();

    };

}
