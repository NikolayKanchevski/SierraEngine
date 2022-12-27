//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include "../../Engine/Include.h"

#define DRAW_IMGUI_HIERARCHY

class Application
{
public:
    /* --- POLLING METHODS --- */
    void Start();

private:
    Camera camera;
    float yaw = -90.0f, pitch = 0.0f;
    const float CAMERA_MOVE_SPEED = 15.0f, CAMERA_LOOK_SPEED = 0.1f, GAMEPAD_CAMERA_LOOK_SPEED = 0.5f;

    PointLight* pointLight;
    std::vector<std::unique_ptr<Model>> tankModels;

    void RenderLoop(std::unique_ptr<MainVulkanRenderer> &renderer);

    void UpdateObjects();
    void DoCameraMovement();

    void DisplayUI(std::unique_ptr<MainVulkanRenderer> &renderer);
    void ListDeeper(Relationship &relationship, const uint32_t iteration);
};