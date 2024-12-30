//
// Created by Nikolay Kanchevski on 15.05.24.
//

#pragma once

#include "../EditorPanel.h"

namespace SierraEngine
{

    class HierarchyPanel final : public EditorPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        HierarchyPanel() noexcept = default;

        /* --- POLLING METHODS --- */
        void Draw(const Scene& scene);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsEntitySelected() const noexcept { return !selectedEntities.empty(); }
        [[nodiscard]] std::optional<EntityID> GetSelectedEntity() const noexcept { return IsEntitySelected() ? std::optional(selectedEntities.front()) : std::nullopt; }
        [[nodiscard]] std::span<const EntityID> GetSelectedEntities() const noexcept { return selectedEntities; }

        /* --- COPY SEMANTICS --- */
        HierarchyPanel(const HierarchyPanel&) = delete;
        HierarchyPanel& operator=(const HierarchyPanel&) = delete;

        /* --- MOVE SEMANTICS --- */
        HierarchyPanel(HierarchyPanel&&) noexcept = default;
        HierarchyPanel& operator=(HierarchyPanel&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~HierarchyPanel() noexcept override = default;

    private:
        std::vector<EntityID> selectedEntities = { };
        void DrawEntity(const Scene& scene, EntityID entityID);

    };

}
