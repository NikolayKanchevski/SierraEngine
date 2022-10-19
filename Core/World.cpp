//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "World.h"

#include "Rendering/Vulkan/VulkanCore.h"
#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Classes/Input.h"
#include "../Engine/Components/Camera.h"
#include "../Engine/Components/InternalComponents.h"
#include <iostream>

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;
using Sierra::Engine::Components::Relationship;

namespace Sierra::Core
{
    entt::registry World::enttRegistry;

    /* --- POLLING METHODS --- */

    void World::Start()
    {
        Input::Start();
        Cursor::Start();
    }

    void World::Prepare(VulkanRenderer &renderer)
    {
        Time::Update();
        Cursor::Update();
        Input::Update();

        renderer.Prepare();
    }

    void World::Update(VulkanRenderer &renderer)
    {
        UpdateObjects(renderer);
        UpdateRenderer(renderer);
    }

    void World::Shutdown()
    {

    }

    /* --- SETTER METHODS --- */

    void World::UpdateObjects(VulkanRenderer &renderer)
    {
        Camera *camera = Camera::GetMainCamera();

        Transform &cameraTransform = enttRegistry.get<Transform>(camera->GetEnttEntity());
        glm::vec3 rendererCameraPosition = { cameraTransform.position.x, -cameraTransform.position.y, cameraTransform.position.z };
        glm::vec3 rendererCameraFrontDirection = { camera->GetFrontDirection().x, -camera->GetFrontDirection().y, camera->GetFrontDirection().z };
        glm::vec3 rendererCameraUpDirection = { camera->GetUpDirection().x, -camera->GetUpDirection().y, camera->GetUpDirection().z };

        auto uniformData = renderer.GetUniformDataReference();
        uniformData->view = glm::lookAt(rendererCameraPosition, rendererCameraPosition + rendererCameraFrontDirection, rendererCameraUpDirection);
        uniformData->projection = glm::perspective(glm::radians(camera->fov), (float) VulkanCore::GetSwapchainExtent().width / (float) VulkanCore::GetSwapchainExtent().height, camera->nearClip, camera->farClip);
        uniformData->projection[1][1] *= -1;
    }

    void World::UpdateRenderer(VulkanRenderer &renderer)
    {
        renderer.UpdateWindow();
        renderer.Render();
    }

    /* --- DESTRUCTOR --- */
}
