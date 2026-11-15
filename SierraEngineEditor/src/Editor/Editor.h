//
// Created by Nikolay Kanchevski on 3.08.24.
//

#pragma once

#include "EditorModal.h"
#include "EditorThemes.h"

#include "../Core/Project.h"
#include "../Rendering/Viewport.h"

#include "Panels/LogPanel.h"
#include "Panels/AssetBrowserPanel.h"

#include "../Platform/EditorPlatformContext.h"

namespace SierraEngine
{
    struct EditorCreateInfo
    {
        const EditorPlatformContext& platformContext;
        const Sierra::FileManager& fileManager;
        const RenderingContext& renderingContext;

        Project& project;
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
        explicit Editor(const EditorCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw(Sierra::CommandBuffer& commandBuffer, const Sierra::InputManager& inputManager);

        Viewport& CreateViewport(ViewportID& ID, const ViewportCreateInfo& createInfo);
        void ForEachViewport(const ViewportEnumerationPredicate& Predicate);
        bool DestroyViewport(ViewportID ID);

        /* --- GETTER METHODS --- */
        [[nodiscard]] Scene& GetScene() noexcept { return scene; }
        [[nodiscard]] const Scene& GetScene() const noexcept { return scene; }

        [[nodiscard]] std::optional<EntityID> GetSelectedEntity() const noexcept { return !selectedEntities.empty() ? std::optional(selectedEntities[0]) : std::nullopt; }
        [[nodiscard]] std::span<const EntityID> GetSelectedEntities() const noexcept { return selectedEntities; }

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
        const EditorPlatformContext* platformContext;
        const Sierra::FileManager* fileManager;
        const RenderingContext* renderingContext;

        Scene scene;
        AssetBrowserPanel assetBrowserPanel;
        LogPanel logPanel;
        std::vector<EntityID> selectedEntities = { };
        Sierra::HandleManager<ViewportID, Viewport> viewports = { };

        ImGuiStyle style = { };
        std::unique_ptr<EditorModal> currentWizard = nullptr;

        void DrawMenuBar();
        ImGuiID DrawDockSpace();

        void OpenTextureSerializeWizard(const std::filesystem::path* outputDirectoryPath = nullptr);
        void OpenMaterialSerializeWizard(const std::filesystem::path* outputDirectoryPath = nullptr);
        void OpenModelSerializeWizard(const std::filesystem::path* outputDirectoryPath = nullptr);

    };

}
