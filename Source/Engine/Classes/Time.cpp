//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Time.h"


namespace Sierra::Engine
{

    /* --- POLLING METHODS --- */
    void Time::Update()
    {
        double currentFrameTime = glfwGetTime();

        deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        upTime = static_cast<float>(currentFrameTime);
        FPS = static_cast<uint32>(glm::ceil(1.0 / deltaTime));
    }
}
