//
// Created by Nikolay Kanchevski on 24.10.24.
//

#pragma once

#include "../../Rendering/Viewport.h"

namespace SierraEngine
{

    struct ViewportPanelDrawInfo
    {
        std::string_view title = "Viewport";
        Viewport& viewport;
    };

    namespace ViewportPanel
    {
        void Draw(const ViewportPanelDrawInfo& drawInfo);
    }

}