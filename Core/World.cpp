//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "World.h"

#include "Rendering/Vulkan/VulkanCore.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core
{

    /* --- POLLING METHODS --- */

    void World::Start()
    {
        Input::Start();
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
        using namespace Sierra::Engine::Components;

        auto camera = Camera::GetMainCamera();

        glm::vec3 rendererCameraPosition = { camera->transform.position.x, camera->transform.position.y * -1, camera->transform.position.z };
        glm::vec3 rendererCameraFrontDirection = { camera->GetFrontDirection().x, camera->GetFrontDirection().y * -1, camera->GetFrontDirection().z };
        glm::vec3 rendererCameraUpDirection = { camera->GetUpDirection().x, camera->GetUpDirection().y, camera->GetUpDirection().z };

        auto uniformData = renderer.GetUniformDataReference();
        uniformData.view = glm::lookAt(rendererCameraPosition, rendererCameraPosition + rendererCameraFrontDirection, rendererCameraUpDirection);
        uniformData.projection = glm::perspective(glm::radians(45.0f), (float) VulkanCore::GetSwapchainExtent().width / (float) VulkanCore::GetSwapchainExtent().height, camera->nearClip, camera->farClip);
        uniformData.projection[1][1] *= -1;
    }

    void World::UpdateRenderer(VulkanRenderer &renderer)
    {
        renderer.Render();
        renderer.UpdateWindow();
    }

    /* --- GETTER METHODS --- */

    /* --- DESTRUCTOR --- */
}
