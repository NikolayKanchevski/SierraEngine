//
// Created by Nikolay Kanchevski on 16.12.22.
//

#pragma once

#include <cstdint>
#include <imgui.h>
#include <glm/vec2.hpp>

namespace Sierra::Core::Rendering::UI
{

    class ImGuiCore
    {
    public:
        /* --- POLLING METHODS --- */
        static void SetSceneViewSize(uint32_t newWidth, uint32_t newHeight);

        /* --- GETTER METHODS --- */
        [[nodiscard]] static inline uint32_t GetSceneViewWidth() { return instance.sceneViewWidth; }
        [[nodiscard]] static inline uint32_t GetSceneViewHeight() { return instance.sceneViewHeight; }

        [[nodiscard]] static inline ImGuiStyle GetDefaultStyle() { return instance.defaultStyle; }

        /* --- DESTRUCTOR --- */
    private:
        ImGuiCore();
        static ImGuiCore instance;

        uint32_t sceneViewWidth = 1;
        uint32_t sceneViewHeight = 1;


        ImGuiStyle defaultStyle;

    };

}
