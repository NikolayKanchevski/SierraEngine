//
// Created by Nikolay Kanchevski on 28.10.24.
//

#pragma once

#include "../EditorPanel.h"

namespace SierraEngine
{

    class PropertiesPanel final : public EditorPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        PropertiesPanel() noexcept = default;

        /* --- POLLING METHODS --- */
        void Draw(std::optional<EntityID> entityID, Scene& scene);

        /* --- COPY SEMANTICS --- */
        PropertiesPanel(const PropertiesPanel&) = delete;
        PropertiesPanel& operator=(const PropertiesPanel&) = delete;

        /* --- MOVE SEMANTICS --- */
        PropertiesPanel(PropertiesPanel&&) noexcept = default;
        PropertiesPanel& operator=(PropertiesPanel&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~PropertiesPanel() noexcept override = default;
    };

}