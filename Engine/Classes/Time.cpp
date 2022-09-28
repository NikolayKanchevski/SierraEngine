//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Time.h"

namespace Sierra::Engine::Classes
{
    int Time::FPS;
    float Time::deltaTime;
    float Time::upTime;
    double Time::lastFrameTime;

    /* --- POLLING METHODS --- */
    void Time::Update()
    {
        double currentFrameTime = glfwGetTime();

        deltaTime = (float) (currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        FPS = (int) glm::ceil(1.0 / deltaTime);
        upTime = (float) currentFrameTime;
    }
}
