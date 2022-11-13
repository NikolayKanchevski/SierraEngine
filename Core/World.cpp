//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "World.h"

#include "Rendering/Vulkan/VulkanCore.h"
#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/Cursor.h"
#include "../Engine/Classes/Input.h"
#include "../Engine/Components/Camera.h"
#include "../Engine/Components/Mesh.h"

using namespace Sierra::Engine::Classes;
using namespace Sierra::Engine::Components;
using Sierra::Engine::Components::Relationship;

namespace Sierra::Core
{
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
        // Update camera
        Camera *camera = Camera::GetMainCamera();
        Transform &cameraTransform = enttRegistry.get<Transform>(camera->GetEnttEntity());
        glm::vec3 rendererCameraPosition = { cameraTransform.position.x, -cameraTransform.position.y, cameraTransform.position.z };
        glm::vec3 rendererCameraFrontDirection = { camera->GetFrontDirection().x, -camera->GetFrontDirection().y, camera->GetFrontDirection().z };
        glm::vec3 rendererCameraUpDirection = { camera->GetUpDirection().x, -camera->GetUpDirection().y, camera->GetUpDirection().z };

        // Update uniform data
        auto uniformData = VulkanCore::GetUniformDataPtr();
        uniformData->view = glm::lookAt(rendererCameraPosition, rendererCameraPosition + rendererCameraFrontDirection, rendererCameraUpDirection);
        uniformData->projection = glm::perspective(glm::radians(camera->fov), (float) VulkanCore::GetSwapchainExtent().width / (float) VulkanCore::GetSwapchainExtent().height, camera->nearClip, camera->farClip);
        uniformData->projection[1][1] *= -1;
        uniformData->directionalLightCount = DirectionalLight::GetDirectionalLightCount();
        uniformData->pointLightCount = PointLight::GetPointLightCount();

        // Update storage data
        auto storageData = VulkanCore::GetStorageDataPtr();
        auto enttMeshView = World::GetEnttRegistry().view<Mesh>();
        for (auto enttEntity : enttMeshView)
        {
            Mesh &mesh = enttMeshView.get<Mesh>(enttEntity);
            storageData->objectDatas[mesh.GetMeshID()].model = mesh.GetModelMatrix();
        }

        auto enttDirectionalLightView = World::GetEnttRegistry().view<DirectionalLight>();
        for (auto enttEntity : enttDirectionalLightView)
        {
            DirectionalLight &directionalLight = enttDirectionalLightView.get<DirectionalLight>(enttEntity);
            DirectionalLight::ShaderDirectionalLight lightData = directionalLight;
            storageData->directionalLights[directionalLight.GetID()] = lightData;
        }

        auto enntPointLightView = World::GetEnttRegistry().view<PointLight>();
        for (auto enttEntity : enntPointLightView)
        {
            PointLight &pointLight = enntPointLightView.get<PointLight>(enttEntity);
            PointLight::ShaderPointLight lightData = pointLight;
            storageData->pointLights[pointLight.GetID()] = lightData;
        }
    }

    void World::UpdateRenderer(VulkanRenderer &renderer)
    {
        renderer.UpdateWindow();
        renderer.Render();
    }

    /* --- DESTRUCTOR --- */
}
