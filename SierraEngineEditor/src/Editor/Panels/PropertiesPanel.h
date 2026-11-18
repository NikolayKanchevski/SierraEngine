//
// Created by Nikolay Kanchevski on 28.10.24.
//

#pragma once

namespace SierraEngine
{

    struct PropertiesPanelDrawInfo
    {
        Scene& scene;
        std::optional<EntityID> selectedEntity = std::nullopt;
    };

    namespace PropertiesPanel
    {
        void Draw(const PropertiesPanelDrawInfo& drawInfo);
    }

}