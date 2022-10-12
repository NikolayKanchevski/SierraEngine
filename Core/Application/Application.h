//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include "../World.h"
#include "../Rendering/Window.h"
#include "../../Core/Rendering/Vulkan/Renderer/VulkanRenderer.h"

#include "../../Engine/Classes/Math.h"
#include "../../Engine/Components/Camera.h"

using namespace Sierra::Engine::Components;
using namespace Sierra::Core::Rendering::Vulkan;

using namespace Sierra::Core;
using namespace Sierra::Core::Rendering;
using namespace Sierra::Engine::Classes;

class Application
{
public:
    /* --- POLLING METHODS --- */
    void Start();
    void RenderLoop(VulkanRenderer &renderer);

    void DisplayUI(VulkanRenderer &renderer);
    void DoCameraMovement();

private:
    Camera camera = Camera();
    float yaw = -90.0f, pitch = 0.0f;
    const float CAMERA_MOVE_SPEED = 15.0f, CAMERA_LOOK_SPEED = 0.1f, GAMEPAD_CAMERA_LOOK_SPEED = 0.5f;

};