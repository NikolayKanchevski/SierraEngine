//
// Created by Nikolay Kanchevski on 16.12.22.
//

#include "ImGuiCore.h"

namespace Sierra::Core::Rendering::UI
{
    ImGuiCore ImGuiCore::instance;

    /* --- POLLING METHODS --- */

    void ImGuiCore::SetSceneViewSize(const uint32_t newWidth, const uint32_t newHeight)
    {
        instance.sceneViewWidth = newWidth;
        instance.sceneViewHeight = newHeight;
    }

}

/* --- CONSTRUCTORS --- */

/* --- SETTER METHODS --- */

/* --- GETTER METHODS --- */

/* --- DESTRUCTOR --- */
