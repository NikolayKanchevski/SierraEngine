//
// Created by Nikolay Kanchevski on 15.05.24.
//

#pragma once

namespace SierraEngine
{

    struct HierarchyPanelDrawInfo
    {
        Scene& scene;
        std::vector<EntityID>& selectedEntities;
    };

    namespace HierarchyPanel
    {
        void Draw(const HierarchyPanelDrawInfo& drawInfo);
    }

}
