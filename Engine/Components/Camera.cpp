//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Camera.h"


#include "Transform.h"
#include "../../Core/Rendering/UI/ImGuiCore.h"
#include "../../Core/Rendering/UI/ImGuiUtilities.h"

using Sierra::Core::Rendering::UI::ImGuiCore;

namespace Sierra::Engine::Components
{
    /* --- CONSTRUCTORS --- */

    void Camera::OnAddComponent()
    {
        CalculateViewMatrix();
        CalculateProjectionMatrix();

        GetComponent<Transform>().PushOnChangeCallback([this]{
            CalculateViewMatrix();
        });

        if (mainCamera == entt::null) SetAsMain();
    }

    /* --- POLLING METHODS --- */

    void Camera::OnDrawUI()
    {
        GUI::BeginProperties();

        GUI::FloatProperty("FOV:", fov);
        GUI::FloatProperty("Near Clip:", nearClip);
        GUI::FloatProperty("Far Clip:", farClip);

        GUI::EndProperties();
    }

    /* --- SETTER METHODS --- */

    void Camera::SetAsMain()
    {
        mainCamera = enttEntity;
    }

    /* --- GETTER METHODS --- */

    Vector3 Camera::GetFrontDirection() const
    {
        Transform &transform = GetComponent<Transform>();

        float cosYaw = glm::cos(glm::radians(transform.GetRotation().x));
        float sinYaw = glm::sin(glm::radians(transform.GetRotation().x));
        float cosPitch = glm::cos(glm::radians(transform.GetRotation().y));
        float sinPitch = glm::sin(glm::radians(transform.GetRotation().y));

        Vector3 direction;
        direction.x = cosYaw * cosPitch;
        direction.y = sinPitch;
        direction.z = sinYaw * cosPitch;

        return glm::normalize(direction);
    }

    float Camera::GetYaw() const
    {
        return GetComponent<Transform>().GetWorldRotation().x;
    }

    float Camera::GetPitch() const
    {
        return GetComponent<Transform>().GetWorldRotation().y;
    }

    float Camera::GetRoll() const
    {
        return GetComponent<Transform>().GetWorldRotation().z;
    }

    Vector3 Camera::GetYawPitchRoll() const
    {
        return GetComponent<Transform>().GetWorldRotation();
    }

    Matrix4x4 Camera::GetViewMatrix()
    {
        return viewMatrix;
    }

    Matrix4x4 Camera::GetProjectionMatrix()
    {
        // TODO: Add a callback to only recalculate this on resize
        CalculateProjectionMatrix();

        return projectionMatrix;
    }

    void Camera::CalculateViewMatrix()
    {
        Transform &transform = GetComponent<Transform>();
        Vector3 frontDirection = GetFrontDirection();

        Vector3 rendererCameraPosition = transform.GetWorldPositionUpInverted();

        Vector3 rendererCameraFrontDirection = { frontDirection.x, -frontDirection.y, frontDirection.z };
        Vector3 rendererCameraUpDirection = { upDirection.x, upDirection.y, upDirection.z };

        viewMatrix = glm::lookAtRH(rendererCameraPosition, rendererCameraPosition + rendererCameraFrontDirection, rendererCameraUpDirection);
    }

    void Camera::CalculateProjectionMatrix()
    {
        projectionMatrix = glm::perspectiveRH(glm::radians(fov), static_cast<float>(ImGuiCore::GetSceneViewWidth()) / static_cast<float>(ImGuiCore::GetSceneViewHeight()), nearClip, farClip);
        projectionMatrix[1][1] *= -1;
    }

}
