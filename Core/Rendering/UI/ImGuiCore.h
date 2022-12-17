//
// Created by Nikolay Kanchevski on 16.12.22.
//

#pragma once

#include <cstdint>

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

        /* --- DESTRUCTOR --- */
    private:
        ImGuiCore() = default;
        static ImGuiCore instance;

        uint32_t sceneViewWidth = 1;
        uint32_t sceneViewHeight = 1;

    };

}
