//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "InternalComponents.h"
#include "../../Core/Rendering/UI/ImGuiCore.h"

using Sierra::Core::Rendering::UI::ImGuiCore;

namespace Sierra::Engine::Components
{
    Camera *Camera::mainCamera = nullptr;

    /* --- CONSTRUCTORS --- */

    Camera::Camera()
    {
        if (mainCamera != nullptr) return;

        SetAsMain();
    }

    /* --- SETTER METHODS --- */

    void Camera::SetAsMain()
    {
        mainCamera = this;
    }

    /* --- GETTER METHODS --- */

    glm::vec3 Camera::GetFrontDirection() const
    {
        Transform &transform = GetComponent<Transform>();

        float cosYaw = glm::cos(glm::radians(transform.rotation.x));
        float sinYaw = glm::sin(glm::radians(transform.rotation.x));
        float cosPitch = glm::cos(glm::radians(transform.rotation.y));
        float sinPitch = glm::sin(glm::radians(transform.rotation.y));

        glm::vec3 direction;
        direction.x = cosYaw * cosPitch;
        direction.y = sinPitch;
        direction.z = sinYaw * cosPitch;

        return glm::normalize(direction);
    }

    glm::mat4x4 Camera::GetViewMatrix() const
    {
        Transform &transform = GetComponent<Transform>();
        glm::vec3 frontDirection = GetFrontDirection();

        glm::vec3 rendererCameraPosition = { transform.position.x, -transform.position.y, transform.position.z };
        glm::vec3 rendererCameraFrontDirection = { frontDirection.x, frontDirection.y, frontDirection.z };
        glm::vec3 rendererCameraUpDirection = { upDirection.x, upDirection.y, upDirection.z };

        return glm::lookAtRH(rendererCameraPosition, rendererCameraPosition + rendererCameraFrontDirection, rendererCameraUpDirection);
    }

    glm::mat4x4 Camera::GetProjectionMatrix() const
    {
        glm::mat4x4 matrix = glm::perspectiveRH(glm::radians(fov), (float) ImGuiCore::GetSceneViewWidth() / (float) ImGuiCore::GetSceneViewHeight(), nearClip, farClip);
        matrix[1][1] *= -1;
        return matrix;
    }

}
