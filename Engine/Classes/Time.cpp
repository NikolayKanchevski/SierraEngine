//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Time.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace Sierra::Engine::Classes
{
    int Time::FPS;
    float Time::deltaTime;
    float Time::upTime;
    double Time::lastFrameTime;

    float Time::fpsUpdateCounter = FPS_UPDATE_PERIOD;

    /* --- POLLING METHODS --- */
    void Time::Update()
    {
        double currentFrameTime = glfwGetTime();

        deltaTime = (float) (currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        upTime = (float) currentFrameTime;

//        if (fpsUpdateCounter >= FPS_UPDATE_PERIOD)
//        {
//            fpsUpdateCounter = 0;
//            FPS = (int) glm::ceil(1.0 / deltaTime);
//        }
//        else
//        {
//            fpsUpdateCounter += deltaTime;
//        }

        FPS = (int) glm::ceil(1.0 / deltaTime);
    }
}
