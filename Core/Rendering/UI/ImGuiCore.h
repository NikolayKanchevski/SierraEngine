//
// Created by Nikolay Kanchevski on 16.12.22.
//

#pragma once

namespace Sierra::Core::Rendering::UI
{

    class ImGuiCore
    {
    public:
        /* --- POLLING METHODS --- */
        static bool SetSceneViewSize(uint newWidth, uint newHeight);
        static void SetSceneViewPosition(float xPosition, float yPosition);

        /* --- GETTER METHODS --- */
        [[nodiscard]] static inline float GetSceneViewPositionX() { return instance.sceneViewPositionX; }
        [[nodiscard]] static inline float GetSceneViewPositionY() { return instance.sceneViewPositionY; }

        [[nodiscard]] static inline uint GetSceneViewWidth() { return instance.sceneViewWidth; }
        [[nodiscard]] static inline uint GetSceneViewHeight() { return instance.sceneViewHeight; }

        [[nodiscard]] static inline ImGuiStyle GetDefaultStyle() { return instance.defaultStyle; }
    private:
        ImGuiCore();
        static ImGuiCore instance;

        float sceneViewPositionX = 1;
        float sceneViewPositionY = 1;

        uint sceneViewWidth = 1;
        uint sceneViewHeight = 1;

        ImGuiStyle defaultStyle;
    };

}
