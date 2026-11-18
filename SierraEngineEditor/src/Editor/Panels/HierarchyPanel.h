//
// Created by Nikolay Kanchevski on 15.05.24.
//

#pragma once

namespace SierraEngine
{

    class HierarchyPanel final
    {
    public:
        /* --- CONSTRUCTORS --- */
        HierarchyPanel() noexcept = default;

        /* --- POLLING METHODS --- */
        void Draw(Scene& scene);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::optional<EntityID> GeSelectedEntity() const noexcept { return !selectedEntities.empty() ? std::optional(selectedEntities.front()) : std::nullopt; }
        [[nodiscard]] std::span<const EntityID> GetSelectedEntities() const noexcept { return selectedEntities; }

        /* --- COPY SEMANTICS --- */
        HierarchyPanel(const HierarchyPanel&) = delete;
        HierarchyPanel& operator=(const HierarchyPanel&) = delete;

        /* --- MOVE SEMANTICS --- */
        HierarchyPanel(HierarchyPanel&&) noexcept = default;
        HierarchyPanel& operator=(HierarchyPanel&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~HierarchyPanel() noexcept = default;

    private:
        std::vector<EntityID> selectedEntities = { };
        void DrawEntity(Scene& scene, EntityID entity);

    };

}
