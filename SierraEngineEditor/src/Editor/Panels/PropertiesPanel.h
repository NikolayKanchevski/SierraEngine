//
// Created by Nikolay Kanchevski on 28.10.24.
//

#pragma once

namespace SierraEngine
{

    struct PropertiesPanelDrawInfo
    {
        Scene& scene;
        EntityID* selectedEntity = nullptr;
    };

    namespace PropertiesPanel
    {
        void Draw(const PropertiesPanelDrawInfo& drawInfo);
    }

}