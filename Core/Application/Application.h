//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include "../Rendering/Window.h"
#include "../Rendering/Vulkan/Renderer/VulkanRenderer.h"
#include "../../Core/World.h"

#include "../../Engine/Classes/Math.h"
#include "../../Engine/Classes/Cursor.h"
#include "../../Engine/Classes/Input.h"
#include "../../Engine/Classes/Time.h"
#include "../../Engine/Classes/Entity.h"
#include "../../Engine/Components/Model.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/Components/Lighting/DirectionalLight.h"

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;

using namespace Sierra::Core;
using namespace Sierra::Core::Rendering;
using namespace Sierra::Core::Rendering::Vulkan;

class Application
{
public:
    /* --- POLLING METHODS --- */
    void Start();

private:
    Camera camera;
    float yaw = -90.0f, pitch = 0.0f;
    const float CAMERA_MOVE_SPEED = 15.0f, CAMERA_LOOK_SPEED = 0.1f, GAMEPAD_CAMERA_LOOK_SPEED = 0.5f;

    std::vector<std::unique_ptr<Model>> tankModels;
    PointLight* pointLight;

    void RenderLoop(VulkanRenderer &renderer);

    void UpdateObjects();
    void DoCameraMovement();

    void DisplayUI(VulkanRenderer &renderer);
    void ListDeeper(Relationship &relationship, const uint32_t iteration);
};