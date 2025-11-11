//
// Created by Nikolay Kanchevski on 3.08.24.
//

#pragma once

#include "EditorThemes.h"
#include "EditorWizard.h"
#include "../Rendering/Viewport.h"

namespace SierraEngine
{
    class Viewport;

    struct EditorCreateInfo
    {
        const Sierra::PlatformContext& platformContext;
        const RenderingContext& renderingContext;
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
        void Draw(Sierra::CommandBuffer& commandBuffer);

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
        const Sierra::PlatformContext* platformContext;
        const RenderingContext* renderingContext;

        Scene scene;
        std::vector<EntityID> selectedEntities = { };
        Sierra::HandleManager<ViewportID, Viewport> viewports = { };

        ImGuiStyle style = { };
        std::unique_ptr<EditorWizard> currentWizard = nullptr;

        void DrawMenuBar();
        ImGuiID DrawDockSpace();

        void OpenTextureSerializeWizard();
        void OpenMaterialSerializeWizard();

    };

}
